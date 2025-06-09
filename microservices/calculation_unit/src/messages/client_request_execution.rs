use std::collections::HashMap;

use super::client_message::{ClientRequest, DummySetVelocity, SetClientFPS};
use crate::{
	game::{
		action::{AsRaw, SafeAction},
		calculation_unit::ServerMessageSenderChannel,
		dummy::DummyObject,
		game_objects::GameObjects,
		id_counter::IdCounter,
		spaceship::Spaceship,
	},
	logger::log_with_time,
};

pub fn set_client_fps(
	username: &String,
	server_message_senders: &HashMap<String, ServerMessageSenderChannel>,
	value: &SetClientFPS,
) -> std::result::Result<SafeAction, String> {
	match server_message_senders.get(username) {
		Some(client) => Ok(SafeAction::SetF64(
			client.update_threshold.raw_mut(),
			1. / value.fps,
		)),
		None => {
			return Err(format!(
				"couldnt find servermessagesenderchannel of id {}",
				username
			));
		}
	}
}

pub fn spawn_dummy(
	username: &String,
	game_objects: &GameObjects,
	name: &String,
	id_counter: &mut IdCounter,
) -> std::result::Result<Vec<SafeAction>, String> {
	// spawn dummy
	log_with_time("spawn dummy");
	let player = game_objects.players.get(username).unwrap();
	let actions = DummyObject::craft(player, &"name".to_string(), id_counter);
	Ok(actions)
}

pub fn dummy_set_velocity(
	username: &String,
	game_objects: &GameObjects,
	value: &DummySetVelocity,
) -> std::result::Result<SafeAction, String> {
	let dummies = &game_objects.dummies;
	match dummies.get(&value.id) {
		Some(dummy) => {
			if dummy.owner == *username {
				Ok(SafeAction::SetCoordinate {
					coordinate: dummy.velocity.raw_mut(),
					other: value.position.clone(),
				})
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
	}
}

impl ClientRequest {
	/// executes a clients input data on the game
	pub fn execute(
		&self,
		username: &String,
		game_objects: &GameObjects,
		server_message_senders: &mut HashMap<
			String,
			ServerMessageSenderChannel,
		>,
		dummy_id_counter: &mut IdCounter,
	) -> std::result::Result<(), String> {
		let mut actions = Vec::<SafeAction>::new();

		if let Some(value) = &self.dummy_set_velocity {
			actions.push(dummy_set_velocity(username, game_objects, value)?);
		} else if let Some(value) = &self.spawn_dummy {
			actions.append(&mut spawn_dummy(
				username,
				game_objects,
				value,
				dummy_id_counter,
			)?);
			actions.push(SafeAction::SpawnSpaceship(Spaceship::new(
				username,
				0.3,
				dummy_id_counter,
			)));
		} else if let Some(value) = &self.set_client_fps {
			actions.push(set_client_fps(
				username,
				server_message_senders,
				value,
			)?);
		} else if let Some(value) = &self.connect {
			log_with_time(format!("a new connection with id {}", value));
		}

		// execute actions
		let go = game_objects.raw_mut();
		for action in actions {
			action.execute(go);
		}
		Ok(())
	}
}
