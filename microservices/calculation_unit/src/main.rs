
mod network;

use warp::Filter;
use futures::{StreamExt, SinkExt};
use warp::ws::{Message, WebSocket};
use serde::{Serialize, Deserialize};
use rmp_serde::{to_vec, from_slice};

#[derive(Serialize, Deserialize, Debug)]
struct ServerMessage {
	author: String,
	content: String,
}

#[derive(Serialize, Deserialize, Debug)]
struct ClientMessage {
	author: String,
	content: String,
}

async fn handle_ws_json(ws: WebSocket) {
	let (mut tx, mut rx) = ws.split();
	while let Some(Ok(msg)) = rx.next().await {
		if let Ok(text) = msg.to_str() {
			println!("Received: {}", text);
			let client_message: ClientMessage = serde_json::from_str::<ClientMessage>(text).unwrap();
			let server_message = ServerMessage {
				author: "server".to_owned(),
				content: format!("hey {}", client_message.author).to_owned(),
			};
			let response = serde_json::to_string(&server_message).unwrap();
			if tx.send(Message::text(&response)).await.is_err() {
				break;
			}
		}
	}
}

async fn handle_ws_msgpack(ws: WebSocket) {
	let (mut tx, mut rx) = ws.split();
	while let Some(Ok(msg)) = rx.next().await {
		if let Ok(text) = msg.to_str() {
			println!("Received: {}", text);
			let client_message: ClientMessage = serde_json::from_str::<ClientMessage>(text).unwrap();
			let server_message = ServerMessage {
				author: "server".to_owned(),
				content: format!("hey {}", client_message.author).to_owned(),
			};
			let response = serde_json::to_string(&server_message).unwrap();
			if tx.send(Message::text(&response)).await.is_err() {
				break;
			}
		}
	}
}

#[tokio::main]
async fn main() {
	let ws_route_json = warp::path!("api" / "test")
		.and(warp::ws())
		.map(|ws: warp::ws::Ws| ws.on_upgrade(handle_ws_json));

	let ws_route_msgpack = warp::path!("api" / "msgpack")
		.and(warp::ws())
		.map(|ws: warp::ws::Ws| ws.on_upgrade(handle_ws_msgpack));

	let static_files = warp::fs::dir("public");

	let routes = ws_route_msgpack
		.or(ws_route_json)
		.or(static_files)
		.with(warp::cors().allow_any_origin());

	warp::serve(routes).run(([0, 0, 0, 0], 8082)).await;
}

/*
// Create WebSocket connection.
const socket = new WebSocket(`ws://localhost:8082/api/test`);

// Connection opened
socket.addEventListener("open", (event) => {
  socket.send(JSON.stringify({author: "jonas", content: "hi"}));
});

// Listen for messages
socket.addEventListener("message", (event) => {
  console.log("Message from server ", event.data);
});
*/ 
