
use serde::{Deserialize, Serialize};
use crate::game::game_objects::GameObjects;
use super::websocket_format::RequestInfo;

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ObjectData {
    pub game_objects: GameObjects,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ServerMessage {
    pub request_info: RequestInfo,
    pub request_data: ObjectData,
}

impl ServerMessage {
    pub fn test() -> Self {
        Self {
            request_info: Default::default(),
            request_data: ObjectData { game_objects: GameObjects::new() },
        }
    }
}
