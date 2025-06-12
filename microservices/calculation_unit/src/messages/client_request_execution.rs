use std::collections::HashMap;

use super::client_message::{
	ClientRequest, DummySetVelocity, SetSpaceshipTarget,
};
use crate::{
	game::{
		action::{AsRaw, SafeAction, UnsafeAction}, calculation_unit::ServerMessageSenderChannel, coordinate::Coordinate, dummy::DummyObject, game_objects::GameObjects, id_counter::IdCounter, planet::OrbitInfoMap, player::Player, spaceship::Spaceship
	},
	logger::log_with_time,
};

fn set_client_fps(
	username: &String,
	server_message_senders: &HashMap<String, ServerMessageSenderChannel>,
	fps: f64,
) -> std::result::Result<SafeAction, String> {
	match server_message_senders.get(username) {
		Some(client) => Ok(SafeAction::SetF64(
			client.update_threshold.raw_mut(),
			1. / fps,
		)),
		None => {
			return Err(format!(
				"couldnt find servermessagesenderchannel of id {}",
				username
			));
		}
	}
}

fn spawn_dummy(
	player: &Player,
	game_objects: &GameObjects,
	name: &String,
	id_counter: &mut IdCounter,
) -> std::result::Result<Vec<SafeAction>, String> {
	// spawn dummy
	log_with_time("spawn dummy");
	let actions = DummyObject::craft(player, &"name".to_string(), id_counter);
	Ok(actions)
}

fn dummy_set_velocity(
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

fn set_spaceship_target(
	game_objects: &GameObjects,
	value: &SetSpaceshipTarget,
	julian_day: f64,
	orbit_info_map: &OrbitInfoMap,
	username: &String,
) -> std::result::Result<SafeAction, String> {
	let spaceship =
		if let Some(s) = game_objects.spaceships.get(&value.spaceship_id) {
			s
		} else {
			return Err(format!(
				"spaceship with id {} not found",
				value.spaceship_id
			));
		};
	if &spaceship.owner != username {
		return Err(format!(
			"user {} tried to control spaceship with owner {}",
			username, spaceship.owner
		));
	}
	let planet = if let Some(p) = game_objects.planets.get(value.planet) {
		p
	} else {
		return Err(format!("planet with index {} not found", value.planet));
	};
	let target =
		spaceship.fly_to_get_target(planet, julian_day, orbit_info_map);
	Ok(SafeAction::SetCoordinate {
		coordinate: spaceship.target.raw_mut(),
		other: target,
	})
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
		spaceship_id_counter: &mut IdCounter,
		julian_day: f64,
		orbit_info_map: &OrbitInfoMap,
	) -> std::result::Result<(), String> {
		let mut actions = Vec::<SafeAction>::new();
		let mut unsafe_actions = Vec::<UnsafeAction>::new();

		let player = if let Some(player) = game_objects.players.get(username) {
			player
		} else {
			return Err("player with that username not found".to_string());
		};

		if let Some(value) = &self.dummy_set_velocity {
			actions.push(dummy_set_velocity(username, game_objects, value)?);
		}
		if let Some(value) = &self.spawn_dummy {
			actions.append(&mut spawn_dummy(
				player,
				game_objects,
				value,
				dummy_id_counter,
			)?);
			actions.push(SafeAction::SpawnSpaceship(Spaceship::new(
				username,
				0.3,
				spaceship_id_counter,
				Coordinate::default()
			)));
		}
		if let Some(value) = &self.set_client_fps {
			actions.push(set_client_fps(
				username,
				server_message_senders,
				*value,
			)?);
		}
		if let Some(value) = &self.set_spaceship_target {
			actions.push(set_spaceship_target(
				game_objects,
				value,
				julian_day,
				orbit_info_map,
				username,
			)?);
		}
		// TEMP later not possible to spawn like this
		if let Some(value) = &self.spawn_spaceship {
			let spaceship = Spaceship::new(username, 0.3, spaceship_id_counter, value.clone());
			actions.push(SafeAction::SpawnSpaceship(spaceship));
		}
		// TEMP later not possible to delete like this
		if let Some(value) = &self.delete_spaceship {
			if let Some(a) = game_objects.spaceships.get(value) {
				unsafe_actions.push(UnsafeAction::DeleteSpaceship(*value));
			}
			else {
				log_with_time("cant delete this spaceship");
			}
		}
		if let Some(value) = &self.connect {
			log_with_time(format!("a new connection with id {}", value));
		}

		// execute actions
		let go = game_objects.raw_mut();
		for action in actions {
			action.execute(go);
		}
		for action in unsafe_actions {
			action.execute(go);
		}
		Ok(())
	}
}
