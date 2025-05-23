use crate::{
	get_time,
	logger::{Loggable, log_with_time},
	messages::{
		client_message::ClientMessage,
		server_message::{ObjectData, ServerMessage},
		websocket_format::RequestInfo,
	},
};
use std::{collections::HashMap, sync::Arc, time::Instant};
use tokio::sync::mpsc::*;

use super::{
	action::{AsRaw, SafeAction},
	dummy::DummyObject,
	game_objects::GameObjects,
};

pub struct ServerMessageSenderChannel {
	pub id: String,
	pub sender: Sender<Arc<ServerMessage>>,
	// FPS
	pub update_threshold: f64,
	pub tick_counter: f64,
}

impl ServerMessageSenderChannel {
	pub fn new(id: String, sender: Sender<Arc<ServerMessage>>) -> Self {
		Self {
			id,
			sender,
			// default 60 fps value till updated
			update_threshold: 1. / 2.,
			tick_counter: 0.,
		}
	}
}

// send message to all client receivers
pub async fn broadcast(
	senders: &mut HashMap<String, ServerMessageSenderChannel>,
	game_objects: &GameObjects,
	delta_seconds: f64,
) {
	// TODO user specific messages
	let server_message = ServerMessage {
		request_info: RequestInfo::new(get_time() as f64),
		request_data: ObjectData::prepare_for("all".to_owned(), game_objects),
	};
	let shared_message = Arc::new(server_message.clone()); // Wrap the message in an Arc
	let mut to_be_removed = Vec::<String>::new();
	// send messages to all
	for (i, (id, sender_channel)) in senders.iter_mut().enumerate() {
		sender_channel.tick_counter += delta_seconds;
		if sender_channel.tick_counter >= sender_channel.update_threshold {
			// send message to client
			// log_with_time(format!("trying to send to client"));
			sender_channel.tick_counter = 0.;
			let message_clone = Arc::clone(&shared_message);
			if let Err(e) = sender_channel.sender.send(message_clone).await {
				log_with_time(format!("Failed to send message: {}", e));
				// remove connection
				to_be_removed.push(id.clone());
			}
		} else {
		}
	}
	// remove senders that are closed
	for i in to_be_removed.iter().rev() {
		senders.remove(i);
	}
}

pub fn update_game(
	game_objects: &mut GameObjects,
	delta_seconds: f64,
) -> std::result::Result<(), String> {
	let mut actions = Vec::<SafeAction>::new();

	// get actions
	update_dummies(&mut actions, &mut game_objects.dummies, delta_seconds)?;

	// execute operations on data
	for action in actions {
		action.execute();
	}
	Ok(())
}

pub fn update_dummies(
	actions: &mut Vec<SafeAction>,
	dummies: &mut HashMap<String, DummyObject>,
	delta_seconds: f64,
) -> std::result::Result<(), String> {
	for (id, dummy) in dummies.iter_mut() {
		// dummy.position.addd(&dummy.velocity, delta_seconds);
		actions.push(SafeAction::AddCoordinate {
			coordinate: dummy.position.raw_mut(),
			other: dummy.velocity.raw(),
			multiplier: delta_seconds,
		});
	}
	Ok(())
}

pub async fn start(
	mut sender_receiver: Receiver<ServerMessageSenderChannel>,
	mut client_message_receiver: Receiver<ClientMessage>,
) {
	// client channels
	let mut server_message_senders = HashMap::<String, ServerMessageSenderChannel>::new();

	// initialise game objects
	let mut game_objects = GameObjects::new();

	// delta time init
	let mut last_time = Instant::now();

	// game loop
	loop {
		while let Ok(sender) = sender_receiver.try_recv() {
			log_with_time(format!("getting sender {}", sender.id));
			// if let Err(e) = sender.sender.try_send(Arc::new(ServerMessage::dummy())) {
			// 	log_with_time(format!("Failed to send initial dummy message: {}", e));
			// }

			server_message_senders.insert(sender.id.clone(), sender);
		}

		// delta time calculation here
		let now = Instant::now();
		let delta_time = now.duration_since(last_time);
		last_time = now;
		let delta_seconds = delta_time.as_secs_f64();

		// receive client input
		while let Ok(client_message) = client_message_receiver.try_recv() {
			let result = client_message
				.request_data
				.execute(&mut game_objects, &mut server_message_senders, delta_seconds)
				.log();
		}

		// game logic calculation
		let update_result = update_game(&mut game_objects, delta_seconds).log();

		// sending message
		// log_with_time(format!("broadcasting to clients"));
		broadcast(&mut server_message_senders, &game_objects, delta_seconds).await;

		// let other tokio tasks do stuff
		tokio::task::yield_now().await;
	}
}
