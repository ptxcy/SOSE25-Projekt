use std::collections::HashMap;

use serde::{Deserialize, Serialize};

use crate::{game::{dummy::DummyObject, game_objects::GameObjects}, get_time};

use super::{client_message::ClientMessage, websocket_format::RequestInfo};

// TODO object data
#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ObjectData {
    pub game_objects: GameObjects,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ServerMessage {
    pub request_info: RequestInfo,
    pub request_data: ObjectData,
}
