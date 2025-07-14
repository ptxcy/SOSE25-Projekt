#include "websocket.h"
#ifdef FEAT_MULTIPLAYER

// ----------------------------------------------------------------------------------------------------
// HTTP Adapter

/**
 *	helper to encode into base64
 *	\param input: string to encode into base64
 *	\returns input encoded into base64
 */
static const char *chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
string _encode(const std::string &input)
{
	string result;
	int val = 0;
	int valb = -6;
	for (unsigned char c : input)
	{
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0)
		{
			result.push_back(chars[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6)
		result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);

	// Add padding
	while (result.size() % 4)
		result.push_back('=');
	return result;
}

/**
 *	contruct adapter address and store for further use
 *	\param host: adapter host
 *	\param port: adapter port
 */
HTTPAdapter::HTTPAdapter(string &host, string &port)
	: m_Addr("http://" + host + ':' + port)
{
}

/**
 *	create a user for login
 *	\param username: username for login
 *	\param password: password for login
 *	\returns true if user has been created successfully
 */
bool HTTPAdapter::create_user(string &username, string &password)
{
	string body = R"({"username":")" + username + R"(","password":")" + password + R"("})";
	COMM_LOG("%s,%s", m_Addr.c_str(), body.c_str());
	cpr::Response response = cpr::Post(
		cpr::Url{m_Addr + "/user"},
		cpr::Header{{"Content-Type", "application/json"}},
		cpr::Body{body});
	COMM_LOG("[ADAPTER] user creation response -> %s (Status: %ld)", response.text.c_str(), response.status_code);
	return response.status_code == 200;
}

/**
 *	authenticate user on server
 *	\param username: username for authentication
 *	\param password: password for authentication
 *	\returns authentication token
 */
string HTTPAdapter::authenticate_on_server(string &username, string &password)
{
	string basicAuth = "Basic " + _encode(username + ":" + password);
	cpr::Response response = cpr::Get(
		cpr::Url{m_Addr + "/authenticate"},
		cpr::Header{{"Authorization", basicAuth}});
	COMM_LOG("[ADAPTER] server auth response -> %s (Status: %ld)", response.text.c_str(), response.status_code);

	string authHeader = response.header["Authorization"];
	if (response.status_code == 200 && authHeader.find("Bearer ") == 0)
		return authHeader;
	return "";
}

/**
 *	create or join a lobby
 *	\param lobby_name: name of the lobby
 *	\param lobby_password: password of the lobby
 *	\param jwt_token: authentication token for lobby interaction
 *	\param create: true if new lobby should be created, false if existing lobby should be joined
 *	\returns lobby connection status
 */
LobbyStatus HTTPAdapter::open_lobby(string &lobby_name, string &lobby_password, string &jwt_token, bool create)
{
	string body = R"({"lobbyName":")" + lobby_name + R"(")";
	body += R"(,"lobbyPassword":")" + lobby_password + R"(")";
	body += "}";

	// create lobby
	cpr::Response response;
	if (create)
	{
		response = cpr::Post(
			cpr::Url{m_Addr + "/lobbys"},
			cpr::Header{{"Authorization", jwt_token}, {"Content-Type", "application/json"}},
			cpr::Body{body});
		COMM_LOG("lobby creation response -> %s (Status: %ld)", response.text.c_str(), response.status_code);
	}

	// join lobby
	else
	{
		response = cpr::Put(
			cpr::Url{m_Addr + "/lobbys"},
			cpr::Header{{"Authorization", jwt_token}, {"Content-Type", "application/json"}},
			cpr::Body{body});
		COMM_LOG("lobby join response -> %s (Status: %ld)", response.text.c_str(), response.status_code);
	}

	return (LobbyStatus)((u32)LOBBY_USER_REFUSED + (response.status_code == 200 || response.status_code == 409));
}

// ----------------------------------------------------------------------------------------------------
// Websocket Connection

/**
 *	function to handle websocket download traffic
 *	\param c: websocket data
 *	NOTE this is meant to be executed in a subthread, so the queues are filled asynchronously
 */
void _handle_websocket_download(Websocket* c)
{
	while (c->running)
	{
		try
		{
			// kill when there is no connection
			if (!c->ws.is_open())
			{
                COMM_ERR("WebSocket ist nicht mehr offen!");
                return;
            }

			// receive raw data
			boost::beast::flat_buffer response_buffer;
			c->ws.read(response_buffer);
			auto data = response_buffer.data();
			c->mutex_msgdata_raw.lock();
			c->raw_data = static_cast<char*>(data.data());
			c->data_size = data.size();
			c->parsing_signal.proceed(true);
			c->mutex_msgdata_raw.unlock();
		}
		catch (const msgpack::insufficient_bytes &e) { COMM_ERR("incomplete data -> %s", e.what()); }
		catch (const std::exception &e) { COMM_ERR("parsing server response -> %s", e.what()); }
	}
	COMM_MSG(LOG_CYAN,"closing download thread");
}

/**
 *	function to handle websocket upload traffic
 *	\param c: websocket data
 *	NOTE this is meant to be executed in a subthread, so the queues are filled asynchronously
 */
void _handle_websocket_upload(Websocket* c)
{
	while (c->running)
	{
		// check client message update
		c->mutex_client_messages.lock();
		if (!c->client_messages.size())
		{
			c->mutex_client_messages.unlock();
			continue;
		}

		// upload client message
		try
		{
			ClientMessage outMsg = c->client_messages.front();
			c->client_messages.pop();
			c->mutex_client_messages.unlock();
			msgpack::sbuffer msg_buffer;
			msgpack::pack(msg_buffer, outMsg);
			c->ws.write(boost::asio::buffer(msg_buffer.data(), msg_buffer.size()));
			c->mutex_client_messages.unlock();
		}
		catch (const std::exception &e)
		{
			COMM_ERR("sending upload -> %s", e.what());
		}
	}
	COMM_MSG(LOG_CYAN,"closing upload thread");
}

/**
 *	function to handle raw download data parsing, because msgpack parser is slow as hell
 *	\param c: pointer to websocket memory
 *	NOTE this is meant to be executed in a subthread
 */
void _handle_websocket_parsing(Websocket* c)
{
	while (c->running)
	{
		c->parsing_signal.wait();
		c->parsing_signal.stall();

		// read raw message data
		msgpack::unpacker unpacker = msgpack::unpacker();
		c->mutex_msgdata_raw.lock();
		unpacker.reserve_buffer(c->data_size);
		memcpy(unpacker.buffer(),c->raw_data,c->data_size);
		unpacker.buffer_consumed(c->data_size);
		c->mutex_msgdata_raw.unlock();

		// parse message data
		ServerMessage message;
		try
		{
			if (!unpacker.next(c->oh)) continue;
			msgpack::object obj = c->oh.get();
			//COMM_LOG("received MessagePack Object %s",(std::ostringstream()<<obj).str().c_str());
			obj.convert(message);
		}
		catch (const std::exception &e) { continue; }

		// §shuffle around 64-bit misread into 8-bit format to fit msgpack bitwise
#ifdef PROJECT_PONG
		vector<u8> gob = vector<u8>(message.request_data.game_objects.size());
		for (u32 i=0;i<message.request_data.game_objects.size();i++)
			gob[i] = (u8)message.request_data.game_objects[i];
		const char* bgob = reinterpret_cast<const char*>(&gob[0]);
		unpacker.reserve_buffer(gob.size());
		memcpy(unpacker.buffer(),bgob,gob.size());
		unpacker.buffer_consumed(gob.size());
		GameObject go;

		// §subparsing game data
		try
		{
			if (!unpacker.next(c->ohb)) continue;
			msgpack::object kek = c->ohb.get();
			//COMM_LOG("received MessagePack Object %s",(std::ostringstream()<<kek).str().c_str());
			kek.convert(go);
		}
		catch (const std::exception& e) { continue; }
#endif

		// excluding relevant memory for writing process
		c->mutex_server_state.lock();
		c->server_state = message;
		c->game_objects = go;
		c->state_update = true;
		c->mutex_server_state.unlock();
	}
	COMM_MSG(LOG_CYAN,"closing parsing thread");
}

/**
 *	encode url parameters
 *	\param value: parameter value to encode
 *	\returns encoded parameter
 */
std::string _url_encode(string &value)
{
	std::ostringstream escaped;
	escaped.fill('0');
	escaped << std::hex;

	for (char c : value)
	{
		if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~')
		{
			escaped << c;
		}
		else
		{
			escaped << '%' << std::setw(2) << std::uppercase << int((unsigned char)c);
		}
	}

	return escaped.str();
}

/**
 *	run the necessary setup to update & process the websocket queues
 *	\param host: websocket host
 *	\param port_ap: adapter port
 *	\param port_ws: websocket port
 *	\param name: username for connection
 *	\param pass: connection password
 *	\param lnom: lobby name
 *	\param lpass: lobby password
 *	\param creator: true if connection attempt creates a new lobby, false if user joins a created lobby
 */
void Websocket::connect(string host,string port_ad,string port_ws,string name,string pass,string lnom,
						string lpass,bool create)
{
	username = name;

	// adapter connection
	HTTPAdapter __Adapter = HTTPAdapter(host,port_ad);
	COMM_ERR_COND(!__Adapter.create_user(name,pass),"user creation did not work");
	string token = __Adapter.authenticate_on_server(name,pass);
	lobby_status = __Adapter.open_lobby(lnom,lpass,token,create);

	// websocket connection
	try
	{
		COMM_LOG("Connecting to WebSocket server at %s:%s...", host.c_str(), port_ws.c_str());
		auto results = boost::asio::ip::tcp::resolver{ioc}.resolve(host, port_ws);
		auto ep = boost::asio::connect(ws.next_layer(), results);
		//ws.handshake(host + ':' + std::to_string(ep.port()), "/calculate?authToken=" + _url_encode(token));
		ws.handshake(host + ':' + std::to_string(ep.port()), "/msgpack");
		ws.binary(true);
		COMM_SCC("Connected to server successfully!");
		// FIXME find out if the ep.port call has merit and if not replace it by predefined parameter

		// start traffic handler
		m_HandleWebsocketDownload = std::thread(_handle_websocket_download,this);
		m_HandleWebsocketDownload.detach();
		m_HandleWebsocketUpload = std::thread(_handle_websocket_upload,this);
		m_HandleWebsocketUpload.detach();
		m_HandleWebsocketParsing = std::thread(_handle_websocket_parsing,this);
		m_HandleWebsocketParsing.detach();
	}
	catch (std::exception const &e) { COMM_ERR("Connection Error: %s", e.what()); }
}
// FIXME extensive usage of try-catch statements is very slow

/**
 *	receive the next server message if possible
 *	\returns next server message
 */
#ifdef PROJECT_PONG
GameObject
#elif
ServerMessage
#endif
	Websocket::receive_message()
{
	mutex_server_state.lock();

#ifdef PROJECT_PONG
	GameObject msg = std::move(game_objects);
#elif
	ServerMessage msg = std::move(server_state);
#endif

	state_update = false;
	mutex_server_state.unlock();
	return std::move(msg);
}
// FIXME in sending and receiving like this there is a lot of copying going on. not ideal cpu usage
//		yes, compensating this with std lingo is bad practice and shows architectural misunderstandings

/**
 *	send client message
 *	\param msg: client message that will be added to the sending queue
 */
void Websocket::send_message(ClientMessage msg)
{
	mutex_client_messages.lock();
	client_messages.push(std::move(msg));
	mutex_client_messages.unlock();
}

/**
 *	close the websocket traffic processes
 */
void Websocket::exit()
{
	running = false;
}

#endif
