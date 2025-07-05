use std::{env, sync::Arc};

use bytes::Bytes;
use calculation_unit::logger::log_with_time;
use futures::{lock::{Mutex, MutexGuard}, stream::SplitSink, SinkExt, StreamExt};
use macroquad::prelude::*;
use pong::{client_message::{ClientMessage, RequestData}, server_message::{GameObjects, ServerMessage}};
use tokio::net::TcpStream;
use tokio_tungstenite::{connect_async, tungstenite::Message, MaybeTlsStream, WebSocketStream};

pub type Write<'a> = MutexGuard<'a, SplitSink<WebSocketStream<MaybeTlsStream<TcpStream>>, Message>>;

async fn send(mut write: Write<'_>, message: ClientMessage) {
	let serialized = rmp_serde::to_vec(&message).expect("failed to serialize");
	write
		.send(Message::Binary(Bytes::from(serialized)))
		.await
		.expect("Failed to send message");
}

fn tungstenite(sender: std::sync::mpsc::Sender<GameObjects>, cm_receiver: std::sync::mpsc::Receiver<ClientMessage>, username: String) -> std::thread::JoinHandle<()> {
	std::thread::spawn(move || {
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

			send(write.lock().await, ClientMessage::connect(&username)).await;

			log_with_time(format!("Connected to WebSocket server!"));

			let handle = tokio::spawn(async move {
				while let Some(msg) = read.next().await {
					match msg {
						Ok(Message::Binary(data)) => {
							// Deserialize MessagePack to ClientMessage
							match rmp_serde::from_slice::<ServerMessage>(&data) {
								Ok(server_message) => {
									let game_objects = rmp_serde::from_slice::<GameObjects>(&server_message.request_data.game_objects);
									match game_objects {
							            Ok(go) => {
											// log_with_time(format!(
											// 	"game objects: {:?}",
											// 	go
											// ));
											sender.send(go).unwrap();
							            },
										Err(e) => log_with_time(format!(
											"Failed to deserialize inner message: {}",
											e
										)),
							        }
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

			while let Ok(cm) = cm_receiver.recv() {
				send(write.lock().await, cm).await;
			}

			let _ = handle.await;
	    })
	})
}

#[macroquad::main("MyGame")]
async fn main() {
	let args = std::env::args().collect::<Vec<String>>();

	if args.len() != 2 {
		log_with_time("wrong start arguments! cargo run -r --bin client <username>");
	}

	let user_id = args[1].clone();

    let (go_sender, go_receiver) = std::sync::mpsc::channel::<GameObjects>();
    let (cm_sender, cm_receiver) = std::sync::mpsc::channel::<ClientMessage>();
    let _ = tungstenite(go_sender, cm_receiver, user_id.clone());
    
    // Store current game state
    let mut current_game_state: Option<GameObjects> = None;
    
    loop {
        clear_background(BLACK);
        
        // Draw static elements
        // draw_line(40.0, 40.0, 100.0, 200.0, 15.0, BLUE);
        // draw_rectangle(screen_width() / 2.0 - 60.0, 100.0, 120.0, 60.0, GREEN);

        
        // Update game state - only process latest message
        while let Ok(go) = go_receiver.try_recv() {
        	current_game_state = Some(go);
        }

        if is_key_pressed(KeyCode::W) {
        	cm_sender.send(ClientMessage { request_data: RequestData {
        		connect: false,
        		move_to: 1,
        	}, user_id: user_id.clone() }).expect("couldnt send to tungstenite");
        }
        if is_key_released(KeyCode::W) {
        	cm_sender.send(ClientMessage { request_data: RequestData {
        		connect: false,
        		move_to: 0,
        	}, user_id: user_id.clone() }).expect("couldnt send to tungstenite");
        }
        if is_key_pressed(KeyCode::S) {
        	cm_sender.send(ClientMessage { request_data: RequestData {
        		connect: false,
        		move_to: -1,
        	}, user_id: user_id.clone() }).expect("couldnt send to tungstenite");
        }
        if is_key_released(KeyCode::S) {
        	cm_sender.send(ClientMessage { request_data: RequestData {
        		connect: false,
        		move_to: 0,
        	}, user_id: user_id.clone() }).expect("couldnt send to tungstenite");
        }
        

        // Draw current gamestate
        if let Some(go) = &current_game_state {
	        draw_text(&format!("score: {:?}", go.score), 20.0, 20.0, 30.0, DARKGRAY);

        	for ball in go.balls.iter() {
	            draw_circle(
	                ball.position.x as f32 + screen_width() / 2.0,
	                ball.position.y as f32 + screen_height() / 2.0,
	                ball.radius as f32,
	                Color::new(0.0, 1.0, 1.0, 1.0)
	            );
        	}
        	for line in go.lines.iter() {
        		draw_line(
		            line.0.x as f32 + screen_width() / 2.0,
		            line.0.y as f32 + screen_height() / 2.0,
		            line.1.x as f32 + screen_width() / 2.0,
		            line.1.y as f32 + screen_height() / 2.0,
		            2.,
		            Color::new(1., 0., 0., 1.)
		        );
        	}
        	for (id, player) in go.players.iter() {
        		for line in player.relative_lines.iter() {
	        		draw_line(
			            (player.position + line.0).x as f32 + screen_width() / 2.0,
			            (player.position + line.0).y as f32 + screen_height() / 2.0,
			            (player.position + line.1).x as f32 + screen_width() / 2.0,
			            (player.position + line.1).y as f32 + screen_height() / 2.0,
			            2.,
			            Color::new(1., 0.5, 1., 1.)
			        );
        		}
        	}
        }
        
        next_frame().await;
    }
}
