use calculation_unit::{
	game::calculation_unit::ServerMessageSenderChannel,
	get_time,
	logger::Loggable,
	messages::{client_message::ClientMessage, server_message::ServerMessage},
};
use futures::{SinkExt, StreamExt};
use std::sync::Arc;
use tokio::sync::mpsc::*;
use warp::Filter;
use warp::ws::{Message, WebSocket};

// async main gets started on program start
#[tokio::main]
async fn main() {
	// dummy print for time
	let millis = get_time();
	println!("current time: {}", millis);

	let (server_message_sender_sender, server_message_sender_receiver) =
		channel::<ServerMessageSenderChannel>(32);
	let (client_message_sender, client_message_receiver) = channel::<ClientMessage>(32);

	// calculation task
	tokio::spawn(async move {
		calculation_unit::game::calculation_unit::start(
			server_message_sender_receiver,
			client_message_receiver,
		)
		.await;
	});

	let ws_route_msgpack = warp::path!("msgpack")
		.and(warp::ws())
		.map(move |ws: warp::ws::Ws| {
			// channels for ServerMessages which update this client
			let sender_sender_clone = server_message_sender_sender.clone();
			let client_message_sender_clone = client_message_sender.clone();

			ws.on_upgrade(move |websocket| {
				handle_ws_msgpack(websocket, client_message_sender_clone, sender_sender_clone)
			})
		});

	let routes = ws_route_msgpack
		// .or(static_files)
		.with(warp::cors().allow_any_origin());

	warp::serve(routes).run(([0, 0, 0, 0], 8082)).await;
}

pub fn send_client_message(client_message_sender: &Sender<ClientMessage>, message: &ClientMessage) {
	let client_message_clone = message.clone();
	let client_message_sender_clone = client_message_sender.clone();
	tokio::spawn(async move {
		client_message_sender_clone
			.send(client_message_clone)
			.await
			.log()
			.unwrap();
	});
}

pub fn parse_client_message(message: Message) -> Result<ClientMessage, rmp_serde::decode::Error> {
	let msgpack_bytes = message.into_bytes();
	let client_message = rmp_serde::from_slice::<ClientMessage>(&msgpack_bytes[..]);
	match &client_message {
		Ok(m) => {
			// TEMP test for client
			println!("{:?}", m);
		}
		Err(_) => {
			println!("received non message pack message");
		}
	};
	client_message
}

// actual message handling
async fn handle_ws_msgpack(
	ws: WebSocket,
	client_message_sender: Sender<ClientMessage>,
	sender_sender: Sender<ServerMessageSenderChannel>,
) {
	let (mut websocket_tx, mut websocket_rx) = ws.split();
	let (server_message_tx, mut server_message_rx) = channel::<Arc<ServerMessage>>(32);

	println!("new websocket client connected");

	// receiving messages from async client
	tokio::spawn(async move {
		while let Some(Ok(msg)) = websocket_rx.next().await {
			let client_message = match parse_client_message(msg) {
				Ok(m) => m,
				Err(_) => continue,
			};

			match &client_message.request_data {
				calculation_unit::messages::client_message::ClientRequest::SpawnDummy { id } => {
					// send the server_message_tx to the calculation task
					let sender_sender = sender_sender.clone();
					let server_message_tx = server_message_tx.clone();
					let id = id.clone();
					tokio::spawn(async move {
						println!("new connection");
						let result = sender_sender
							.send(ServerMessageSenderChannel::new(id, server_message_tx))
							.await
							.logm("failed to send server_message_tx");
					});
				}
				_ => {}
			};

			// send client message to calculation task
			send_client_message(&client_message_sender, &client_message);
		}
	});

	// sending messages from calculation_unit to client async
	while let Some(msg) = server_message_rx.recv().await {
		// println!("got something from calc");
		let response = match std::panic::catch_unwind(|| {
			let msgpack_bytes = rmp_serde::to_vec(&*msg).log().unwrap();
			msgpack_bytes
		}) {
			Ok(r) => r,
			Err(_) => continue,
		};
		if websocket_tx.send(Message::binary(response)).await.is_err() {
			break;
		}
	}
}
