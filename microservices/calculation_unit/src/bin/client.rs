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
use url::Url;

pub fn request_spawn(id: &String) -> Vec<u8> {
	// Example ClientMessage to send
	let client_message = ClientMessage {
		request_data: ClientRequest::new_spawn_dummy(id),
		..Default::default()
	};

	// Serialize ClientMessage to MessagePack
	let serialized_message =
		rmp_serde::to_vec(&client_message).expect("Failed to serialize message");
	serialized_message
}

pub fn request_set_fps(id: &String, fps: f64) -> Vec<u8> {
	// Example ClientMessage to send
	let client_message = ClientMessage {
		request_data: ClientRequest::new_set_client_fps(SetClientFPS {
			id: id.clone(),
			fps,
		}),
		..Default::default()
	};

	// Serialize ClientMessage to MessagePack
	let serialized_message =
		rmp_serde::to_vec(&client_message).expect("Failed to serialize message");
	serialized_message
}

pub fn request_move(id: &String) -> Vec<u8> {
	// Example ClientMessage to send
	let client_message = ClientMessage {
		request_data: ClientRequest::new_dummy_set_velocity(DummySetVelocity {
			id: id.clone(),
			position: Coordinate {
				x: 2.,
				y: 0.,
				z: 0.,
			},
		}),
		..Default::default()
	};

	// Serialize ClientMessage to MessagePack
	let serialized_message =
		rmp_serde::to_vec(&client_message).expect("Failed to serialize message");
	serialized_message
}

#[tokio::main]
async fn main() {
	let args = env::args().collect::<Vec<String>>();

	if args.len() != 2 {
		panic!("no id args, start using cargo run --bin client -- dummy1");
	}

	let id = args[1].clone();

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
	let id_clone = id.clone();
	tokio::spawn(async move {
		let mut write = write_clone.lock().await;
		// spawn dummy_1
		let serialized_message = request_spawn(&id_clone);
		write
			.send(Message::Binary(Bytes::from(serialized_message)))
			.await
			.expect("Failed to send message");
		log_with_time(format!("Message sent to server! trying to spawn"));

		let serialized_message = request_set_fps(&id_clone, 5.);
		write
			.send(Message::Binary(Bytes::from(serialized_message)))
			.await
			.expect("Failed to send message");
		log_with_time(format!("Message sent to server! trying to set fps to 5"));

		// move dummy_1
		loop {
			let serialized_message = request_move(&id_clone);
			write
				.send(Message::Binary(Bytes::from(serialized_message)))
				.await
				.expect("Failed to send message");
		}
	});

	// Read messages from the server
	while let Some(msg) = read.next().await {
		match msg {
			Ok(Message::Binary(data)) => {
				// Deserialize MessagePack to ClientMessage
				match rmp_serde::from_slice::<ServerMessage>(&data) {
					Ok(client_message) => {
						log_with_time(format!(
							"Received: {:?}",
							client_message.request_data.game_objects
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
