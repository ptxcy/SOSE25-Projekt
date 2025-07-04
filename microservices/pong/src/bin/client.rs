use std::sync::Arc;

use bytes::Bytes;
use calculation_unit::logger::log_with_time;
use futures::{lock::{Mutex, MutexGuard}, stream::SplitSink, SinkExt, StreamExt};
use macroquad::prelude::*;
use pong::server_message::ServerMessage;
use tokio::net::TcpStream;
use tokio_tungstenite::{connect_async, tungstenite::Message, MaybeTlsStream, WebSocketStream};

pub type Write<'a> = MutexGuard<'a, SplitSink<WebSocketStream<MaybeTlsStream<TcpStream>>, Message>>;

async fn send(mut write: Write<'_>, message: Vec<u8>) {
	write
		.send(Message::Binary(Bytes::from(message)))
		.await
		.expect("Failed to send message");
}

fn tungstenite() {
    let rt = tokio::runtime::Runtime::new().unwrap();
    rt.block_on(async {
		// Define the WebSocket server URL
		let url = "ws://127.0.0.1:9000/msgpack";

		// Connect to the WebSocket server
		let (ws_stream, _) = connect_async(url)
			.await
			.expect("Failed to connect to WebSocket server");

		let (write, mut read) = ws_stream.split();
		let write = Arc::new(Mutex::new(write));

		log_with_time(format!("Connected to WebSocket server!"));

		tokio::spawn(async move {
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
							Err(e) => log_with_time(format!(
								"Failed to deserialize message: {}",
								e
							)),
						}
					}
					Ok(other) => log_with_time(format!(
						"Received non-binary message: {:?}",
						other
					)),
					Err(e) => {
						log_with_time(format!("Error reading message: {}", e));
						break;
					}
				}
			}
		});
    })
}

#[macroquad::main("MyGame")]
async fn main() {
	tungstenite();

	let mut x = 20.0;
    loop {
        clear_background(RED);

        draw_line(40.0, 40.0, 100.0, 200.0, 15.0, BLUE);
        draw_rectangle(screen_width() / 2.0 - 60.0, 100.0, 120.0, 60.0, GREEN);

        draw_text("Hello, Macroquad!", 20.0, 20.0, 30.0, DARKGRAY);
        draw_circle(x, 100., 50., Color::new(50.,50.,50.,1.));

        x += 0.1;
        next_frame().await
    }
}
