use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct RequestData {
    pub connect: bool,
}

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct ClientMessage {
    pub request_data: RequestData,
    pub user_id: String,
}
