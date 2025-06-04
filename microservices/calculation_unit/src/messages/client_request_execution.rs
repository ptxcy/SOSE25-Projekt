use std::collections::HashMap;

use super::client_message::{ClientRequest, DummySetVelocity, SetClientFPS};
use crate::{
	game::{
		action::AsRaw, calculation_unit::ServerMessageSenderChannel, dummy::DummyObject, game_objects::GameObjects, gametraits::{Buyer, Craftable}, player::Player
	},
	logger::log_with_time,
};

pub fn set_client_fps(
	username: &String,
	server_message_senders: &mut HashMap<String, ServerMessageSenderChannel>,
	value: &SetClientFPS,
) -> std::result::Result<(), String> {
	match server_message_senders.get_mut(username) {
		Some(client) => {
			client.update_threshold = 1. / value.fps;
		}
		None => {
			return Err(format!(
				"couldnt find servermessagesenderchannel of id {}",
				username
			));
		}
	};
	Ok(())
}

pub fn spawn_dummy(
	username: &String,
	game_objects: &mut GameObjects,
	name: &String,
	id_counter: &mut usize,
) -> std::result::Result<(), String> {
	// spawn dummy
	log_with_time("spawn dummy");
	let go = game_objects as *mut GameObjects;
	let player = game_objects.players.get_mut(username).unwrap();
	DummyObject::craft(player, &"name".to_string(), go, id_counter);
	Ok(())
}

pub fn dummy_set_velocity(
	username: &String,
	game_objects: &mut GameObjects,
	value: &DummySetVelocity,
) -> std::result::Result<(), String> {
	let dummies = &mut game_objects.dummies;
	match dummies.get_mut(&value.id) {
		Some(dummy) => {
			if dummy.owner == *username {
				dummy.velocity.set(&value.position);
			} else {
				return Err(format!(
					"{} tried moving dummy {} that is not owned",
					username, dummy.id
				));
			}
		}
		None => {
			// TODO handle connection if connection tried to move non existent object
			return Err(format!(
				"tried moving dummy but dummy doesnt exist by this id: {}",
				value.id
			));
		}
	};
	Ok(())
}

impl ClientRequest {
	pub fn new_connect(username: &str) -> Self {
		Self {
			connect: Some(username.to_string()),
			..Default::default()
		}
	}
	/// create a new client requests to set the wanted fps of a client
	pub fn new_set_client_fps(value: SetClientFPS) -> Self {
		Self {
			set_client_fps: Some(value),
			..Default::default()
		}
	}
	/// create a new client requests to spawn a dummy
	pub fn new_spawn_dummy(name: &str) -> Self {
		Self {
			spawn_dummy: Some(name.to_owned()),
			..Default::default()
		}
	}
	/// create a new client requests to set the velocity of a dummy
	pub fn new_dummy_set_velocity(value: DummySetVelocity) -> Self {
		Self {
			dummy_set_velocity: Some(value),
			..Default::default()
		}
	}
	/// executes a clients input data on the game
	pub fn execute(
		&self,
		username: &String,
		game_objects: &mut GameObjects,
		server_message_senders: &mut HashMap<
			String,
			ServerMessageSenderChannel,
		>,
		delta_seconds: f64,
		id_counter: &mut usize,
	) -> std::result::Result<(), String> {
		if let Some(value) = &self.dummy_set_velocity {
			dummy_set_velocity(username, game_objects, value)?;
		} else if let Some(value) = &self.spawn_dummy {
			spawn_dummy(username, game_objects, value, id_counter)?;
		} else if let Some(value) = &self.set_client_fps {
			set_client_fps(username, server_message_senders, value)?;
		} else if let Some(value) = &self.connect {
			log_with_time(format!("a new connection with id {}", value));
		}
		Ok(())
	}
}
