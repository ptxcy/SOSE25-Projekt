use crate::{
	get_time,
	logger::{Loggable, log_with_time},
	messages::{
		client_message::ClientMessage,
		server_message::{SendObjectData, SendServerMessage},
		websocket_format::RequestInfo,
	},
};
use std::{collections::HashMap, time::Instant};
use tokio::sync::mpsc::*;

use super::{
	game_objects::GameObjects,
	id_counter::IdCounter,
	orbit::initialize_orbit_info_map,
	planet_util::{get_timefactor, julian_day},
	player::Player,
};

/// container of the sender where the calculation unit game thread can send servermessages to the calculation units websocket handling thread
pub struct ServerMessageSenderChannel {
	pub username: String,
	pub sender: Sender<Vec<u8>>,
	// FPS
	pub update_threshold: f64,
	pub tick_counter: f64,
}

impl ServerMessageSenderChannel {
	pub fn new(username: String, sender: Sender<Vec<u8>>) -> Self {
		Self {
			username,
			sender,
			// default 60 fps value till updated
			update_threshold: 1. / 60.,
			tick_counter: 0.,
		}
	}
}

/// send message to all client receivers
pub async fn broadcast(
	senders: &mut HashMap<String, ServerMessageSenderChannel>,
	game_objects: &GameObjects,
	delta_seconds: f64,
) {
	let mut to_be_removed = Vec::<String>::new();
	// send messages to all
	for (i, (username, sender_channel)) in senders.iter_mut().enumerate() {
		let server_message = SendServerMessage {
			request_info: RequestInfo::new(get_time() as f64),
			request_data: SendObjectData::prepare_for(username, game_objects),
		};
		sender_channel.tick_counter += delta_seconds;
		if sender_channel.tick_counter >= sender_channel.update_threshold {
			// send message to client
			sender_channel.tick_counter = 0.;
			let msgpack_bytes =
				rmp_serde::to_vec(&server_message).log().unwrap();
			if let Err(e) = sender_channel.sender.send(msgpack_bytes).await {
				log_with_time(format!("Failed to send message: {}", e));
				// remove connection
				to_be_removed.push(username.clone());
			}
		}
	}
	// remove senders that are closed
	for i in to_be_removed.iter().rev() {
		senders.remove(i);
	}
}

/// starting the game / game loop
pub async fn start(
	mut sender_receiver: Receiver<ServerMessageSenderChannel>,
	mut client_message_receiver: Receiver<ClientMessage>,
) {
	// client channels
	let mut server_message_senders =
		HashMap::<String, ServerMessageSenderChannel>::new();

	// initialise game objects
	let mut game_objects = GameObjects::new();
	let orbit_map = initialize_orbit_info_map();

	// delta time init
	let mut last_time = Instant::now();
	let mut julian_day = julian_day(2025, 5, 30);
	let time_scale = 1.;

	// id counter for object creation
	let mut dummy_id_counter = IdCounter::new();
	let mut spaceship_id_counter = IdCounter::new();

	// game loop
	loop {
		// receive new players / sender for server messages
		while let Ok(sender) = sender_receiver.try_recv() {
			let username = sender.username.clone();
			log_with_time(format!("getting sender {}", username));
			let player = Player::new(username.clone());
			game_objects.players.insert(username.clone(), player);
			server_message_senders.insert(username, sender);
		}

		// delta time calculation here
		let now = Instant::now();
		let delta_time = now.duration_since(last_time);
		last_time = now;
		let delta_seconds = delta_time.as_secs_f64();
		let delta_ingame_days = delta_seconds * time_scale;

		// receive client messages
		while let Ok(client_message) = client_message_receiver.try_recv() {
			let result = client_message
				.request_data
				.execute(
					&client_message.username,
					&mut game_objects,
					&mut server_message_senders,
					&mut dummy_id_counter,
					&mut spaceship_id_counter,
					julian_day,
					delta_ingame_days,
					&orbit_map,
				)
				.log();
		}

		// game logic calculation
		// update multithreadd
		GameObjects::update(
			&mut game_objects,
			delta_ingame_days,
			get_timefactor(julian_day),
			&orbit_map,
		)
		.log()
		.unwrap();

		julian_day += delta_ingame_days;

		// sending message
		// log_with_time(format!("broadcasting to clients"));
		broadcast(&mut server_message_senders, &game_objects, delta_seconds)
			.await;

		// let other tokio tasks do stuff
		tokio::task::yield_now().await;
	}
}
