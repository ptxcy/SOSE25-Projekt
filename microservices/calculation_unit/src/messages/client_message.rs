use super::websocket_format::RequestInfo;
use crate::game::{coordinate::Coordinate, dummy::DummyObject, game_objects::GameObjects};
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

impl ClientRequest {
	// executes a clients input data on the game
	pub fn set_client_fps(value: f64) -> Self {
		Self {
			set_client_fps: Some(value),
			..Default::default()
		}
	}
	pub fn spawn_dummy(id: &str) -> Self {
		Self {
			spawn_dummy: Some(id.to_owned()),
			..Default::default()
		}
	}
	pub fn dummy_set_velocity(value: DummySetVelocity) -> Self {
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
		let dummies = &mut game_objects.dummies;
		match self {
			// TEMP move dummy client by certain amount
			ClientRequest {
				set_client_fps: _,
				spawn_dummy: _,
				dummy_set_velocity: Some(DummySetVelocity { id, position }),
			} => {
				// TODO
				match dummies.get_mut(&id) {
					Some(dummy) => {
						dummy.velocity = position;
					}
					None => {
						// TODO handle connection if connection tried to move non existent object
						return Err(format!(
							"tried moving dummy but dummy doesnt exist by this id: {}",
							id
						));
					}
				};
			}

			// TEMP spawn dummy client
			ClientRequest {
				set_client_fps: _,
				spawn_dummy: Some(id),
				dummy_set_velocity: _,
			} => {
				// check if dummy with id already exists
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
				println!("spawn dummy client");
				let dummy = DummyObject {
					id: id.clone(),
					..Default::default()
				};
				dummies.insert(id, dummy);
			}
			_ => {}
		};
		Ok(())
	}
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ClientMessage {
	pub request_info: RequestInfo,
	pub request_data: ClientRequest,
}

impl Default for ClientMessage {
	fn default() -> Self {
		ClientMessage {
			request_info: Default::default(),
			request_data: Default::default(),
		}
	}
}
