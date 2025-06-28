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

impl ClientMessage {
    pub fn connect(username: &str) -> Self {
        ClientMessage {
            request_data: RequestData { connect: true },
            user_id: username.to_string(),
        }
    }
}
