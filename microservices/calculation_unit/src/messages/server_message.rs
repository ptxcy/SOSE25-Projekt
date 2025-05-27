use super::websocket_format::RequestInfo;
use crate::game::game_objects::GameObjects;
use serde::{Deserialize, Serialize};

/// data that is going to be rendered
#[derive(Serialize, Debug, Clone)]
pub struct ObjectData<'a> {
	pub target_user_id: &'a String,
	pub game_objects: &'a GameObjects,
}

impl<'a> ObjectData<'a> {
	pub fn prepare_for(user: &'a String, game_objects: &'a GameObjects) -> Self {
		Self {
			target_user_id: user,
			game_objects: game_objects.prepare_for(user),
		}
	}
}

/// message that is being sent from server to clients via auth proxy
#[derive(Serialize, Debug, Clone)]
pub struct ServerMessage<'a> {
	pub request_info: RequestInfo,
	pub request_data: ObjectData<'a>,
}
