use crate::{
	get_time,
	logger::{Loggable, log_with_time},
	messages::{
		client_message::ClientMessage,
		server_message::{SendObjectData, SendServerMessage},
		websocket_format::RequestInfo,
	},
};
use std::{collections::HashMap, sync::Arc, time::Instant};
use tokio::sync::mpsc::*;

use super::{
	game_objects::GameObjects,
	orbit::initialize_orbit_info_map,
	planet::{get_timefactor, julian_day},
};

/// container of the sender where the calculation unit game thread can send servermessages to the calculation units websocket handling thread
pub struct ServerMessageSenderChannel {
	pub id: String,
	pub sender: Sender<Vec<u8>>,
	// FPS
	pub update_threshold: f64,
	pub tick_counter: f64,
}

impl ServerMessageSenderChannel {
	pub fn new(id: String, sender: Sender<Vec<u8>>) -> Self {
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
	let mut to_be_removed = Vec::<String>::new();
	// send messages to all
	for (i, (id, sender_channel)) in senders.iter_mut().enumerate() {
		let server_message = SendServerMessage {
			request_info: RequestInfo::new(get_time() as f64),
			request_data: SendObjectData::prepare_for(id, game_objects),
		};
		let shared_message = Arc::new(server_message);
		sender_channel.tick_counter += delta_seconds;
		if sender_channel.tick_counter >= sender_channel.update_threshold {
			// send message to client
			// log_with_time(format!("trying to send to client"));
			sender_channel.tick_counter = 0.;
			let message_clone = Arc::clone(&shared_message);
			let msgpack_bytes = rmp_serde::to_vec(&*message_clone).log().unwrap();
			if let Err(e) = sender_channel.sender.send(msgpack_bytes).await {
				log_with_time(format!("Failed to send message: {}", e));
				// remove connection
				to_be_removed.push(id.clone());
			}
		}
	}
	// remove senders that are closed
	for i in to_be_removed.iter().rev() {
		senders.remove(i);
	}
}

pub async fn start(
	mut sender_receiver: Receiver<ServerMessageSenderChannel>,
	mut client_message_receiver: Receiver<ClientMessage>,
) {
	// client channels
	let mut server_message_senders = HashMap::<String, ServerMessageSenderChannel>::new();

	// initialise game objects
	let mut game_objects = GameObjects::new();
	let orbit_map = initialize_orbit_info_map();

	// delta time init
	let mut last_time = Instant::now();
	let mut julian_day = julian_day(2025, 5, 30);
	let time_scale = 360.;

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

		// receive client messages
		while let Ok(client_message) = client_message_receiver.try_recv() {
			let result = client_message
				.request_data
				.execute(
					&mut game_objects,
					&mut server_message_senders,
					delta_seconds,
				)
				.log();
		}

		// game logic calculation
		let dummies = Arc::new(game_objects.dummies);
		let planets = Arc::new(game_objects.planets);
		GameObjects::update(
			Arc::clone(&dummies),
			Arc::clone(&planets),
			delta_seconds,
			get_timefactor(julian_day),
			&orbit_map,
		)
		.log()
		.unwrap();

		game_objects.dummies = Arc::try_unwrap(dummies).unwrap();
		game_objects.planets = Arc::try_unwrap(planets).unwrap();

		julian_day += delta_seconds * time_scale;

		// sending message
		// log_with_time(format!("broadcasting to clients"));
		broadcast(&mut server_message_senders, &game_objects, delta_seconds).await;

		// let other tokio tasks do stuff
		tokio::task::yield_now().await;
	}
}
