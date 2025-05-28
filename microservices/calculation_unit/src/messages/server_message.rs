use super::websocket_format::RequestInfo;
use crate::game::game_objects::GameObjects;
use serde::{Deserialize, Serialize};

/// data that is going to be rendered
#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ObjectData {
	pub target_user_id: String,
	pub game_objects: GameObjects,
}

impl ObjectData {
	pub fn prepare_for(user: &String, game_objects: &GameObjects) -> Self {
		Self {
			target_user_id: user.clone(),
			game_objects: game_objects.prepare_for(user),
		}
	}
}

/// message that is being sent from server to clients via auth proxy
#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ServerMessage {
	pub request_info: RequestInfo,
	pub request_data: ObjectData,
}

impl ServerMessage {
	pub fn test() -> Self {
		Self {
			request_info: Default::default(),
			request_data: ObjectData {
				game_objects: GameObjects::new(),
				target_user_id: "all".to_owned(),
			},
		}
	}
}
