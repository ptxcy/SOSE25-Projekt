use std::collections::HashMap;

use serde::{Deserialize, Serialize};

use crate::{game::dummy::DummyObject, get_time};

use super::{client_message::ClientMessage, websocket_format::RequestInfo};

// TODO object data
#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ObjectData {
    pub dummies: HashMap<String, DummyObject>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ServerMessage {
    pub request_info: RequestInfo,
    pub request_data: ObjectData,
}

// TEMP going to be removed after testing
impl ServerMessage {
    pub fn respond_to(client_message: &ClientMessage) -> Self {
        let mut updated_request_info = client_message.request_info.clone();
        // TODO set current send time
        updated_request_info.calculation_unit.sent_time = get_time() as f64;
        Self {
            request_info: updated_request_info,
            request_data: ObjectData {
                dummies: HashMap::new(),
            },
        }
    }
}
