// module structure

mod logger;
mod messages {
    pub mod server_message;
    pub mod websocket_format;
    pub mod client_message;
}

mod game {
    pub mod network;
    pub mod coordinate;
    pub mod calculation_unit;
    pub mod dummy;
}

// use structure

use std::{sync::Arc, time::{SystemTime, UNIX_EPOCH}};
use game::calculation_unit;
use messages::{client_message::ClientMessage, server_message::ServerMessage};
use logger::Loggable;
use warp::Filter;
use futures::{SinkExt, StreamExt};
use warp::ws::{Message, WebSocket};
use tokio::sync::mpsc::*;

pub fn get_time() -> u128 {
	let now = SystemTime::now();
	let millis = now.duration_since(UNIX_EPOCH).expect("time went backwards").as_millis();
	millis
}

// async main gets started on program start
#[tokio::main]
async fn main() {
	// dummy print for time
	let millis = get_time();
	println!("current time: {}", millis);

	let (server_message_sender_sender, server_message_sender_receiver) = channel::<Sender<Arc<ServerMessage>>>(32);
	let (client_message_sender, client_message_receiver) = channel::<ClientMessage>(1024);

	// calculation task
	tokio::spawn(async move {
		calculation_unit::start(server_message_sender_receiver, client_message_receiver).await;
	});


	/* let ws_route_json = warp::path!("test")
		.and(warp::ws())
		.map(|ws: warp::ws::Ws| ws.on_upgrade(handle_ws_json)); */

	let ws_route_msgpack = warp::path!("msgpack")
    .and(warp::ws())
    .map(move |ws: warp::ws::Ws| {
        // channels for ServerMessages which update this client
        let (server_message_tx, server_message_rx) = channel::<Arc<ServerMessage>>(32);
        let sender_sender_clone = server_message_sender_sender.clone();
		let client_message_sender_clone = client_message_sender.clone();

		// send the server_message_tx to the calculation task
        tokio::spawn(async move {
            if let Err(e) = sender_sender_clone.send(server_message_tx).await {
                eprintln!("Failed to send server_message_tx: {}", e);
            }
        });

        ws.on_upgrade(move |websocket| handle_ws_msgpack(websocket, server_message_rx, client_message_sender_clone))
    });

	// let static_files = warp::fs::dir("public");

	let routes = ws_route_msgpack
		// .or(static_files)
		.with(warp::cors().allow_any_origin());

	warp::serve(routes).run(([0, 0, 0, 0], 8082)).await;
}

// actual message handling
async fn handle_ws_msgpack(ws: WebSocket, mut server_message_rx: Receiver<Arc<ServerMessage>>, client_message_sender: Sender<ClientMessage>) {
	let (mut websocket_tx, mut websocket_rx) = ws.split();

	// receiving messages from async client
	tokio::spawn(async move {
		while let Some(Ok(msg)) = websocket_rx.next().await {
			match std::panic::catch_unwind(|| {
				let msgpack_bytes = msg.into_bytes();
				let client_message = rmp_serde::from_slice::<ClientMessage>(&msgpack_bytes[..]).log().unwrap();
				let received = serde_json::to_string(&client_message).log().unwrap();
				println!("Received: {}", received);
				
				// send clientmsg to calculation task
				let client_message_clone = client_message.clone();
				let client_message_sender_clone = client_message_sender.clone();
				tokio::spawn(async move {
					client_message_sender_clone.send(client_message_clone).await.log().unwrap();
				});

				// let server_message = ServerMessage::respond_to(&client_message);
				// let response = rmp_serde::to_vec(&server_message).log().unwrap();
				// response
			}) {
			    Ok(r) => r,
			    Err(e) => { continue; },
			};
			// if websocket_tx.send(Message::binary(response)).await.is_err() {
			// 	break;
			// }
		}
	});

	// sending messages from server to client async
	while let Some(msg) = server_message_rx.recv().await {
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

/*
// Create WebSocket connection.
const socket = new WebSocket(`ws://localhost:8082/test`);

// Connection opened
socket.addEventListener("open", (event) => {
  socket.send(JSON.stringify({author: "jonas", content: "hi"}));
});

// Listen for messages
socket.addEventListener("message", (event) => {
  console.log("Message from server ", event.data);
});
*/
