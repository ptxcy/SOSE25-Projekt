
mod network;
mod client_message;
mod server_message;
mod websocket_format;
mod logger;

use std::time::{SystemTime, UNIX_EPOCH};

use client_message::ServerMessage;
use logger::Loggable;
use server_message::ClientMessage;
use warp::Filter;
use futures::{StreamExt, SinkExt};
use warp::ws::{Message, WebSocket};

const CLOSE_MESSAGE: &str = "closing connection to websocket client";

#[tokio::main]
async fn main() {
	let now = SystemTime::now();
	let millis = now.duration_since(UNIX_EPOCH).expect("time went backwards").as_millis();
	println!("current time: {}", millis);

	let ws_route_json = warp::path!("test")
		.and(warp::ws())
		.map(|ws: warp::ws::Ws| ws.on_upgrade(handle_ws_json));

	let ws_route_msgpack = warp::path!("msgpack")
		.and(warp::ws())
		.map(|ws: warp::ws::Ws| ws.on_upgrade(handle_ws_msgpack));

	let static_files = warp::fs::dir("public");

	let routes = ws_route_msgpack
		.or(ws_route_json)
		.or(static_files)
		.with(warp::cors().allow_any_origin());

	warp::serve(routes).run(([0, 0, 0, 0], 8082)).await;
}

async fn handle_ws_json(ws: WebSocket) {
	let (mut tx, mut rx) = ws.split();
	while let Some(Ok(msg)) = rx.next().await {
		if let Ok(text) = msg.to_str() {
			println!("Received: {}", text);
			let client_message: ClientMessage = serde_json::from_str::<ClientMessage>(text).log().expect(CLOSE_MESSAGE);
			let server_message = ServerMessage::response_to(&client_message);
			let response = serde_json::to_string(&server_message).log().expect(CLOSE_MESSAGE);
			if tx.send(Message::text(&response)).await.is_err() {
				break;
			}
		}
	}
}

async fn handle_ws_msgpack(ws: WebSocket) {
	let (mut tx, mut rx) = ws.split();
	while let Some(Ok(msg)) = rx.next().await {
		let msgpack_bytes = msg.into_bytes();
		let client_message = rmp_serde::from_slice::<ClientMessage>(&msgpack_bytes[..]).log().expect(CLOSE_MESSAGE);
		let received = serde_json::to_string(&client_message).log().expect(CLOSE_MESSAGE);
		println!("Received: {}", received);

		let server_message = ServerMessage::response_to(&client_message);
		let response = rmp_serde::to_vec(&server_message).log().expect(CLOSE_MESSAGE);
		if tx.send(Message::binary(response)).await.is_err() {
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
