use serde::{Deserialize, Serialize};

use super::{client_message::ClientMessage, websocket_format::RequestInfo};

#[derive(Serialize, Deserialize, Debug)]
pub struct ObjectData {}

#[derive(Serialize, Deserialize, Debug)]
pub struct ServerMessage {
    request_info: RequestInfo,
    request_data: ObjectData,
}

impl ServerMessage {
    pub fn respond_to(client_message: &ClientMessage) -> Self {
        Self {
            request_info: todo!(),
            request_data: todo!(),
        }
    }
}
