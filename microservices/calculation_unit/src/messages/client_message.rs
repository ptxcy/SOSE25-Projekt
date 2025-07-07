use super::websocket_format::RequestInfo;
use crate::game::coordinate::Coordinate;
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct DummySetVelocity {
	pub id: usize,
	pub position: Coordinate,
}

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct SetSpaceshipTarget {
	pub spaceship_id: usize,
	pub planet: usize,
}

/// request of the client to change something or execute something
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct ClientRequest {
	pub set_client_fps: Option<f64>,
	pub spawn_dummy: Option<String>,
	pub dummy_set_velocity: Option<DummySetVelocity>,
	pub connect: Option<String>,
	pub set_spaceship_target: Option<SetSpaceshipTarget>,
	// TEMP later not possible to spawn like this
	pub spawn_spaceship: Option<Coordinate>,
	// TEMP later not possible to delete like this
	pub delete_spaceship: Option<usize>,
	pub lobby: Option<String>,
}

/// message that the client sends to the calculation unit containing a request to do something
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct ClientMessage {
	pub request_info: RequestInfo,
	pub request_data: ClientRequest,
	pub username: String,
}
