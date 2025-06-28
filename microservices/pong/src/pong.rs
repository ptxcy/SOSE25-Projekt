use std::{collections::HashMap, thread::JoinHandle, time::Instant};

use crate::{action::{ActionWrapper, AsRaw}, client_message::ClientMessage, server_message::{GameObjects, ObjectData, ServerMessage, ServerMessageSenderChannel}};
use calculation_unit::logger::{log_with_time, Loggable};
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
    client_message_receiver: Receiver<ClientMessage>,
) {
	// client channels
	let mut server_message_senders =
		HashMap::<String, ServerMessageSenderChannel>::new();

	// initialise game objects
	let mut game_objects = GameObjects::new(30);

	// delta time init
	let mut last_time = Instant::now();

    // game loop
    loop {
		let now = Instant::now();
		let delta_time = now.duration_since(last_time);
		last_time = now;
		let delta_seconds = delta_time.as_secs_f64();

		// receive new players / sender for server messages
		while let Ok(sender) = server_message_sender_receiver.try_recv() {
			let username = sender.user_id.clone();
			log_with_time(format!("getting sender {}", username));
			// let player = Player::new(username.clone());
			// game_objects.players.insert(username.clone(), player);
			server_message_senders.insert(username, sender);
		}

		let (action_sender, action_receiver) = std::sync::mpsc::channel::<ActionWrapper>();
		let mut threads = Vec::<JoinHandle<()>>::new();

		threads.push(update_balls(action_sender.clone(), game_objects.raw(), delta_seconds));

		for thread in threads {
			thread.join().unwrap();
		}

		while let Ok(action) = action_receiver.try_recv() {
			action.execute(game_objects.raw_mut());
		}

    	broadcast(&mut server_message_senders, &game_objects, delta_seconds).await;
        tokio::task::yield_now().await;
    }
}

pub fn update_balls(sender: std::sync::mpsc::Sender<ActionWrapper>, go: *const GameObjects, delta_seconds: f64) -> JoinHandle<()> {
	std::thread::spawn({
		let game_objects = unsafe {&(*go)};
		move || {
			for ball in game_objects.balls.iter() {
				let actions = ball.update(game_objects, delta_seconds);
				for action in actions {
					sender.send(action).unwrap();
				}
			}
		}
	})
}
