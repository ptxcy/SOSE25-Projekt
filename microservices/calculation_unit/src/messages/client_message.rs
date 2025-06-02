use super::websocket_format::RequestInfo;
use crate::game::coordinate::Coordinate;
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct DummySetVelocity {
	pub id: String,
	pub position: Coordinate,
}

/// sets the incoming message rate, which is mostly objects that are going to be rendered, so fps
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct SetClientFPS {
	pub fps: f64,
}

/// request of the client to change something or execute something
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct ClientRequest {
	pub set_client_fps: Option<SetClientFPS>,
	pub spawn_dummy: Option<String>,
	pub dummy_set_velocity: Option<DummySetVelocity>,
	pub connect: Option<String>,
}

/// message that the client sends to the calculation unit containing a request to do something
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct ClientMessage {
	pub request_info: RequestInfo,
	pub request_data: ClientRequest,
	pub username: String,
}
