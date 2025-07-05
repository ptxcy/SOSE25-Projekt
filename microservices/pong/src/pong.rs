use std::{collections::HashMap, thread::JoinHandle, time::Instant};

use crate::{action::{ActionWrapper, AsRaw}, client_message::ClientMessage, player::Player, server_message::{GameObjects, ObjectData, ServerMessage, ServerMessageSenderChannel}};
use calculation_unit::{game::coordinate::Coordinate, logger::{log_with_time, Loggable}};
use tokio::sync::mpsc::*;


/// send message to all client receivers
pub async fn broadcast(
	senders: &mut HashMap<String, ServerMessageSenderChannel>,
	game_objects: &GameObjects,
	delta_seconds: f64,
) {
	let mut to_be_removed = Vec::<String>::new();
	// send messages to all
	for (i, (username, sender_channel)) in senders.iter_mut().enumerate() {
		let server_message = ServerMessage {
			request_data: ObjectData::prepare_for(username, game_objects),
		};
		sender_channel.tick_counter += delta_seconds;
		if sender_channel.tick_counter >= sender_channel.update_threshold {
			// send message to client
			sender_channel.tick_counter = 0.;
			log_with_time("sending to client");
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

pub async fn start(
    mut server_message_sender_receiver: Receiver<ServerMessageSenderChannel>,
    mut client_message_receiver: Receiver<ClientMessage>,
) {
	// client channels
	let mut server_message_senders =
		HashMap::<String, ServerMessageSenderChannel>::new();

	// initialise game objects
	let mut game_objects = GameObjects::new(40);

	// delta time init
	let mut last_time = Instant::now();

    // game loop
    loop {
		let now = Instant::now();
		let delta_time = now.duration_since(last_time);
		last_time = now;
		let mut delta_seconds = delta_time.as_secs_f64();
		if delta_seconds > 0.1 {delta_seconds = 0.1;}

		// receive new players / sender for server messages
		while let Ok(sender) = server_message_sender_receiver.try_recv() {
			let username = sender.user_id.clone();
			log_with_time(format!("getting sender {}", username));
			let player = Player::new(game_objects.players.len() % 2 == 0);
			game_objects.players.insert(username.clone(), player);
			server_message_senders.insert(username, sender);
		}

		while let Ok(client_message) = client_message_receiver.try_recv() {
			let username = client_message.user_id;
			let player = game_objects.players.get_mut(&username).expect("player with username not found");
			if client_message.request_data.move_to == -1 {
				player.velocity = Coordinate::new(0., player.speed, 0.);
			}
			else if client_message.request_data.move_to == 1 {
				player.velocity = Coordinate::new(0., -player.speed, 0.);
			}
			else if client_message.request_data.move_to == 0 {
				player.velocity = Coordinate::new(0., 0., 0.);
			}
		}

		let (action_sender, action_receiver) = std::sync::mpsc::channel::<ActionWrapper>();
		let mut threads = Vec::<JoinHandle<()>>::new();

		if game_objects.players.len() == 2 {
			threads.push(update_balls(action_sender.clone(), game_objects.raw(), delta_seconds));
		}
		threads.push(update_players(action_sender.clone(), game_objects.raw(), delta_seconds));

		for thread in threads {
			thread.join().unwrap();
		}

		while let Ok(action) = action_receiver.try_recv() {
			action.execute(game_objects.raw_mut());
		}

		GameObjects::chunky(&game_objects.balls, &mut game_objects.chunks);

    	broadcast(&mut server_message_senders, &game_objects, delta_seconds).await;
        tokio::task::yield_now().await;
    }
}

pub fn update_balls(sender: std::sync::mpsc::Sender<ActionWrapper>, go: *const GameObjects, delta_seconds: f64) -> JoinHandle<()> {
	std::thread::spawn({
		let game_objects = unsafe {&(*go)};
		move || {
			let mut actions = Vec::<ActionWrapper>::with_capacity(game_objects.balls.len() * 2);
			for ball in game_objects.balls.iter() {
				ball.update(game_objects, delta_seconds, &mut actions);
			}
			for action in actions {
				sender.send(action).unwrap();
			}
		}
	})
}

pub fn update_players(sender: std::sync::mpsc::Sender<ActionWrapper>, go: *const GameObjects, delta_seconds: f64) -> JoinHandle<()> {
	std::thread::spawn({
		let game_objects = unsafe {&(*go)};
		move || {
			let mut actions = Vec::<ActionWrapper>::with_capacity(game_objects.players.len() * 2);
			for (id, player) in game_objects.players.iter() {
				player.update(game_objects, delta_seconds, &mut actions);
			}
			for action in actions {
				sender.send(action).unwrap();
			}
		}
	})
}
