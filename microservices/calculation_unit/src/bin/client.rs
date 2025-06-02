use std::env;
use std::sync::Arc;

use bytes::Bytes;
use calculation_unit::{
	game::coordinate::Coordinate,
	logger::log_with_time,
	messages::{
		client_message::{ClientMessage, ClientRequest, DummySetVelocity, SetClientFPS},
		server_message::ServerMessage,
	},
};
use futures_util::{SinkExt, stream::StreamExt};
use tokio::sync::Mutex;
use tokio_tungstenite::{connect_async, tungstenite::protocol::Message};

#[tokio::main]
async fn main() {
	let args = env::args().collect::<Vec<String>>();

	if args.len() != 3 {
		panic!("no id args, start using cargo run --bin client -- dummy1 username");
	}

	let dummy_id = args[1].clone();
	let username = args[2].clone();

	println!("username {} joins and spawn a dummy {}", username, dummy_id);

	// Define the WebSocket server URL
	let url = "ws://127.0.0.1:8082/msgpack";

	// Connect to the WebSocket server
	let (ws_stream, _) = connect_async(url)
		.await
		.expect("Failed to connect to WebSocket server");

	log_with_time(format!("Connected to WebSocket server!"));

	let (write, mut read) = ws_stream.split();
	let write = Arc::new(Mutex::new(write));

	// Send a message to the server
	let write_clone = Arc::clone(&write);
	tokio::spawn(async move {
		let dummy_id_clone = dummy_id.clone();
		let mut write = write_clone.lock().await;
		// request connection so that client is able to receive messages
		let serialized_message = request_connect(&username);
		write
			.send(Message::Binary(Bytes::from(serialized_message)))
			.await
			.expect("Failed to send message");
		log_with_time(format!("Message sent to server! trying to connect"));

		// spawn dummy_1
		let serialized_message = request_spawn(&dummy_id_clone, &username);
		write
			.send(Message::Binary(Bytes::from(serialized_message)))
			.await
			.expect("Failed to send message");
		log_with_time(format!("Message sent to server! trying to spawn"));

		let serialized_message = request_set_fps(&dummy_id_clone, 5., &username);
		write
			.send(Message::Binary(Bytes::from(serialized_message)))
			.await
			.expect("Failed to send message");
		log_with_time(format!("Message sent to server! trying to set fps to 5"));

		// move dummy_1
		let serialized_message = request_move(&dummy_id_clone, &username);
		write
			.send(Message::Binary(Bytes::from(serialized_message)))
			.await
			.expect("Failed to send message");
	});

	// Read messages from the server
	while let Some(msg) = read.next().await {
		match msg {
			Ok(Message::Binary(data)) => {
				// Deserialize MessagePack to ClientMessage
				match rmp_serde::from_slice::<ServerMessage>(&data) {
					Ok(server_message) => {
						log_with_time(format!(
							"Received: {:?}",
							server_message.request_data.game_objects
						));
					}
					Err(e) => log_with_time(format!("Failed to deserialize message: {}", e)),
				}
			}
			Ok(other) => log_with_time(format!("Received non-binary message: {:?}", other)),
			Err(e) => {
				log_with_time(format!("Error reading message: {}", e));
				break;
			}
		}
	}
}

pub fn request_connect(username: &String) -> Vec<u8> {
	let client_message = ClientMessage {
		request_data: ClientRequest::new_connect(username),
		username: username.clone(),
		..Default::default()
	};

	let serialized_message =
		rmp_serde::to_vec(&client_message).expect("Failed to serialize message");
	serialized_message
}

pub fn request_spawn(id: &String, username: &String) -> Vec<u8> {
	let client_message = ClientMessage {
		request_data: ClientRequest::new_spawn_dummy(id),
		username: username.clone(),
		..Default::default()
	};

	let serialized_message =
		rmp_serde::to_vec(&client_message).expect("Failed to serialize message");
	serialized_message
}

pub fn request_set_fps(id: &String, fps: f64, username: &String) -> Vec<u8> {
	let client_message = ClientMessage {
		request_data: ClientRequest::new_set_client_fps(SetClientFPS {
			fps,
		}),
		username: username.clone(),
		..Default::default()
	};

	let serialized_message =
		rmp_serde::to_vec(&client_message).expect("Failed to serialize message");
	serialized_message
}

pub fn request_move(id: &String, username: &String) -> Vec<u8> {
	let client_message = ClientMessage {
		request_data: ClientRequest::new_dummy_set_velocity(DummySetVelocity {
			id: id.clone(),
			position: Coordinate {
				x: 2.,
				y: 0.,
				z: 0.,
			},
		}),
		username: username.clone(),
		..Default::default()
	};

	let serialized_message =
		rmp_serde::to_vec(&client_message).expect("Failed to serialize message");
	serialized_message
}
