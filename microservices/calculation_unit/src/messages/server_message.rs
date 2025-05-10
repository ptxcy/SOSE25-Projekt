use serde::{Deserialize, Serialize};

use crate::{game::dummy::DummyObject, get_time};

use super::{client_message::ClientMessage, websocket_format::RequestInfo};

#[derive(Serialize, Deserialize, Debug, Clone)]
// TODO object data
pub struct ObjectData {
    dummies: Vec<DummyObject>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ServerMessage {
    request_info: RequestInfo,
    request_data: ObjectData,
}

impl ServerMessage {
    pub fn respond_to(client_message: &ClientMessage) -> Self {
        let mut updated_request_info = client_message.request_info.clone();
        // TODO set current send time
        updated_request_info.calculation_unit.sent_time = get_time() as f64;
        Self {
            request_info: updated_request_info,
            request_data: ObjectData {
                dummies: vec![],
            },
        }
    }
}
