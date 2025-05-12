use std::sync::Arc;
use calculation_unit::{
	game::calculation_unit::ServerMessageSenderChannel, get_time, logger::Loggable, messages::{
		client_message::ClientMessage,
		server_message::ServerMessage
	}
};
use warp::Filter;
use futures::{SinkExt, StreamExt};
use warp::ws::{Message, WebSocket};
use tokio::sync::mpsc::*;

// async main gets started on program start
#[tokio::main]
async fn main() {
	// dummy print for time
	let millis = get_time();
	println!("current time: {}", millis);

	let (server_message_sender_sender, server_message_sender_receiver) = channel::<ServerMessageSenderChannel>(32);
	let (client_message_sender, client_message_receiver) = channel::<ClientMessage>(32);

	// calculation task
	tokio::spawn(async move {
		calculation_unit::game::calculation_unit::start(server_message_sender_receiver, client_message_receiver).await;
	});


	let ws_route_msgpack = warp::path!("msgpack")
    .and(warp::ws())
    .map(move |ws: warp::ws::Ws| {
        // channels for ServerMessages which update this client
        let sender_sender_clone = server_message_sender_sender.clone();
		let client_message_sender_clone = client_message_sender.clone();


        ws.on_upgrade(move |websocket| handle_ws_msgpack(websocket, client_message_sender_clone, sender_sender_clone))
    });

	// let static_files = warp::fs::dir("public");

	let routes = ws_route_msgpack
		// .or(static_files)
		.with(warp::cors().allow_any_origin());

	warp::serve(routes).run(([0, 0, 0, 0], 8082)).await;
}

// actual message handling
async fn handle_ws_msgpack(ws: WebSocket, client_message_sender: Sender<ClientMessage>, sender_sender: Sender<ServerMessageSenderChannel>) {
	let (mut websocket_tx, mut websocket_rx) = ws.split();
    let (server_message_tx, mut server_message_rx) = channel::<Arc<ServerMessage>>(32);

	// receiving messages from async client
	tokio::spawn(async move {

		while let Some(Ok(msg)) = websocket_rx.next().await {
			let msgpack_bytes = msg.into_bytes();
			let client_message = match rmp_serde::from_slice::<ClientMessage>(&msgpack_bytes[..]).log() {
			    Ok(m) => m,
			    Err(_) => continue,
			};

			match &client_message.request_data {
		        calculation_unit::messages::client_message::ClientRequest::SpawnDummy { id } => {
					// send the server_message_tx to the calculation task
					let id = id.clone();
			        tokio::spawn(async move {
			        	println!("new connection");
						let server_message_tx_clone = server_message_tx.clone();
			            if let Err(e) = sender_sender.send(ServerMessageSenderChannel::new(id, server_message_tx_clone)).await {
			                eprintln!("Failed to send server_message_tx: {}", e);
			            }
			        });
			        // make sure only doing this once ever, second loop for other messages
					// send client message to calculation task
					let client_message_clone = client_message.clone();
					let client_message_sender_clone = client_message_sender.clone();
					tokio::spawn(async move {
						client_message_sender_clone.send(client_message_clone).await.log().unwrap();
					});
		            break;
		        },
		        _ => { }
		    };
			
			// send client message to calculation task
			let client_message_clone = client_message.clone();
			let client_message_sender_clone = client_message_sender.clone();
			tokio::spawn(async move {
				client_message_sender_clone.send(client_message_clone).await.log().unwrap();
			});
		}

		while let Some(Ok(msg)) = websocket_rx.next().await {
			let msgpack_bytes = msg.into_bytes();
			let client_message = match rmp_serde::from_slice::<ClientMessage>(&msgpack_bytes[..]).log() {
			    Ok(m) => m,
			    Err(_) => continue,
			};

			// let received = serde_json::to_string(&client_message).log().unwrap();
			// println!("Received: {}", received);
			
			// send client message to calculation task
			let client_message_clone = client_message.clone();
			let client_message_sender_clone = client_message_sender.clone();
			tokio::spawn(async move {
				client_message_sender_clone.send(client_message_clone).await.log().unwrap();
			});
		}
	});

	// sending messages from calculation_unit to client async
	while let Some(msg) = server_message_rx.recv().await {
		println!("got something from calc");
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

