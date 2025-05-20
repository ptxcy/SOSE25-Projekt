use std::env;

use calculation_unit::{
	game::coordinate::Coordinate,
	messages::{
		client_message::{ClientMessage, ClientRequest, DummySetVelocity},
		server_message::ServerMessage,
	},
};
use futures_util::{SinkExt, stream::StreamExt};
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
	let url = Url::parse("ws://127.0.0.1:8082/msgpack").expect("Invalid WebSocket URL");

	// Connect to the WebSocket server
	let (ws_stream, _) = connect_async(url)
		.await
		.expect("Failed to connect to WebSocket server");

	println!("Connected to WebSocket server!");

	let (mut write, mut read) = ws_stream.split();

	// Send a message to the server
	tokio::spawn(async move {
		// spawn dummy_1
		let serialized_message = request_spawn(&id);
		write
			.send(Message::Binary(serialized_message))
			.await
			.expect("Failed to send message");
		println!("Message sent to server! trying to spawn");

		// move dummy_1
		loop {
			let serialized_message = request_move(&id);
			write
				.send(Message::Binary(serialized_message))
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
						println!("Received: {:?}", client_message.request_data.game_objects);
					}
					Err(e) => eprintln!("Failed to deserialize message: {}", e),
				}
			}
			Ok(other) => println!("Received non-binary message: {:?}", other),
			Err(e) => {
				eprintln!("Error reading message: {}", e);
				break;
			}
		}
	}
}
