use super::websocket_format::RequestInfo;
use crate::{game::{coordinate::Coordinate, dummy::DummyObject, game_objects::GameObjects}, logger::log_with_time};
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct DummySetVelocity {
	pub id: String,
	pub position: Coordinate,
}

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct ClientRequest {
	pub set_client_fps: Option<f64>,
	pub spawn_dummy: Option<String>,
	pub dummy_set_velocity: Option<DummySetVelocity>,
}

pub fn set_client_fps(
	game_objects: &mut GameObjects,
	delta_seconds: f64,
	value: f64,
) -> std::result::Result<(), String> {
	// TODO 
	Ok(())
}
pub fn spawn_dummy(
	game_objects: &mut GameObjects,
	delta_seconds: f64,
	id: String,
) -> std::result::Result<(), String> {
	// check if dummy with id already exists
	let dummies = &mut game_objects.dummies;
	match dummies.get(&id) {
		Some(_) => {
			return Err(format!(
				"couldnt spawn dummy, dummy already exists! id: {}",
				id
			));
		}
		None => {}
	};

	// spawn dummy
	log_with_time("spawn dummy client");
	let dummy = DummyObject {
		id: id.clone(),
		..Default::default()
	};
	dummies.insert(id, dummy);
	Ok(())
}
pub fn dummy_set_velocity(
	game_objects: &mut GameObjects,
	delta_seconds: f64,
	value: DummySetVelocity,
) -> std::result::Result<(), String> {
	let dummies = &mut game_objects.dummies;
	match dummies.get_mut(&value.id) {
		Some(dummy) => {
			dummy.velocity = value.position;
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
	// executes a clients input data on the game
	pub fn new_set_client_fps(value: f64) -> Self {
		Self {
			set_client_fps: Some(value),
			..Default::default()
		}
	}
	pub fn new_spawn_dummy(id: &str) -> Self {
		Self {
			spawn_dummy: Some(id.to_owned()),
			..Default::default()
		}
	}
	pub fn new_dummy_set_velocity(value: DummySetVelocity) -> Self {
		Self {
			dummy_set_velocity: Some(value),
			..Default::default()
		}
	}
	pub fn execute(
		self,
		game_objects: &mut GameObjects,
		delta_seconds: f64,
	) -> std::result::Result<(), String> {
		if let Some(value) = self.dummy_set_velocity {
			dummy_set_velocity(game_objects, delta_seconds, value)?;
		} else if let Some(value) = self.spawn_dummy {
			spawn_dummy(game_objects, delta_seconds, value)?;
		} else if let Some(value) = self.set_client_fps {
			set_client_fps(game_objects, delta_seconds, value)?;
		}
		Ok(())
	}
}

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct ClientMessage {
	pub request_info: RequestInfo,
	pub request_data: ClientRequest,
}
