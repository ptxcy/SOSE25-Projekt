use crate::{game::coordinate::Coordinate, get_time, logger::Loggable, messages::{client_message::{ClientMessage, ClientRequest}, server_message::{ObjectData, ServerMessage}, websocket_format::RequestInfo}};
use tokio::sync::mpsc::*;
use std::{collections::HashMap, sync::Arc, time::Instant};

use super::{dummy::DummyObject, game_objects::GameObjects};

pub struct ServerMessageSenderChannel {
	pub id: String,
	pub sender: Sender<Arc<ServerMessage>>,
	pub update_threshold: f64,
	pub tick_counter: f64,
}

impl ServerMessageSenderChannel {
    pub fn new(id: String, sender: Sender<Arc<ServerMessage>>) -> Self {
    	Self {
    		id,
	        sender,
	        // default 60 fps value till updated
	        update_threshold: 1. / 60.,
	        tick_counter: 0.,
	    }
    }
}

// send message to all client receivers
pub fn broadcast(senders: &mut Vec<ServerMessageSenderChannel>, message: &ServerMessage, delta_seconds: f64) {
	let shared_message = Arc::new(message.clone()); // Wrap the message in an Arc
	let mut to_be_removed = Vec::<usize>::new();
	// send messages to all
	for (i, sender_channel) in senders.iter_mut().enumerate() {
		sender_channel.tick_counter += delta_seconds;
		if sender_channel.tick_counter >= sender_channel.update_threshold {
			// send message to client
			println!("trying to send to client");
			sender_channel.tick_counter = 0.;
			let message_clone = Arc::clone(&shared_message);
			if let Err(e) = sender_channel.sender.try_send(message_clone) {
				match e {
					error::TrySendError::Full(_) => {
						eprintln!("Failed to send message: {}", e);
					},
					error::TrySendError::Closed(_) => {
						eprintln!("Failed to send message: {}", e);
						// remove connection
						to_be_removed.push(i);
					},
				};
			}
		}
		else {
			// println!("tick not there yet {} - {}", sender_channel.tick_counter, sender_channel.update_threshold);
		}
	}
	// remove senders that are closed
	for i in to_be_removed.iter().rev() {
		senders.remove(*i);
	}
}

pub fn update_game(game_objects: &mut GameObjects, delta_seconds: f64) -> std::result::Result<(), String> {
	update_dummies(&mut game_objects.dummies, delta_seconds)?;
	Ok(())
}

pub fn update_dummies(dummies: &mut HashMap<String, DummyObject>, delta_seconds: f64) -> std::result::Result<(), String> {
	for (id, dummy) in dummies.iter_mut() {
		dummy.position.addd(&dummy.velocity, delta_seconds);
	}
	Ok(())
}

pub async fn start(mut sender_receiver: Receiver<ServerMessageSenderChannel>, mut client_message_receiver: Receiver<ClientMessage>) {
	// client channels
	let mut server_message_senders = Vec::<ServerMessageSenderChannel>::new();

	// initialise game objects
	let mut game_objects = GameObjects::new();

	// delta time init
	let mut last_time = Instant::now();
	
	// game loop
	loop {
		// get new client channels
		while let Ok(sender) = {sender_receiver.try_recv()} {
			server_message_senders.push(sender);
		}

		// delta time calculation here
		let now = Instant::now();
		let delta_time = now.duration_since(last_time);
		last_time = now;
		let delta_seconds = delta_time.as_secs_f64();

		// receive client input
		while let Ok(client_message) = client_message_receiver.try_recv() {
			let result = client_message.request_data.execute(&mut game_objects, delta_seconds).log();
		}

		// game logic calculation
		let update_result = update_game(&mut game_objects, delta_seconds).log();

		// creating message for sending
		let object_data = ObjectData {
			game_objects
		};
		let server_message = ServerMessage {
			request_info: RequestInfo::new(get_time() as f64),
			request_data: object_data,
		};

		// sending message
		broadcast(&mut server_message_senders, &server_message, delta_seconds);

		// retrieving ownership of data
		game_objects = server_message.request_data.game_objects;
	}
}
