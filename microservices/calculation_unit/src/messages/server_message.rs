use super::websocket_format::RequestInfo;
use crate::game::game_objects::{GameObjects, GameObjectsReceive, SendGameObjects};
use serde::{Deserialize, Serialize};

/// data that is going to be rendered
#[derive(Serialize, Debug, Clone)]
pub struct ObjectData {
	pub target_user_id: String,
	pub game_objects: GameObjects,
}

#[derive(Deserialize, Debug, Clone)]
pub struct ObjectDataReceive {
	pub target_user_id: String,
	pub game_objects: GameObjectsReceive,
}

#[derive(Serialize, Debug, Clone)]
pub struct SendObjectData<'a> {
	pub target_user_id: String,
	pub game_objects: SendGameObjects<'a>,
}

impl<'a> SendObjectData<'a> {
	pub fn prepare_for(user: &String, game_objects: &'a GameObjects) -> Self {
		Self {
			target_user_id: user.clone(),
			game_objects: game_objects.prepare_for(user),
		}
	}
}

/// message that is being sent from server to clients via auth proxy
#[derive(Serialize, Debug, Clone)]
pub struct ServerMessage {
	pub request_info: RequestInfo,
	pub request_data: ObjectData,
}

#[derive(Deserialize, Debug, Clone)]
pub struct ServerMessageReceive {
	pub request_info: RequestInfo,
	pub request_data: ObjectDataReceive,
}

#[derive(Serialize, Debug, Clone)]
pub struct SendServerMessage<'a> {
	pub request_info: RequestInfo,
	pub request_data: SendObjectData<'a>,
}
