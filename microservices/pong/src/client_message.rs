use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct RequestData {
    pub connect: bool,
    pub move_to: i8, // -1 down, 0 nothing, 1 up
}

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct ClientMessage {
    pub request_data: RequestData,
    pub user_id: String,
}

impl ClientMessage {
    pub fn connect(username: &str) -> Self {
        ClientMessage {
            request_data: RequestData { connect: true, move_to: 0 },
            user_id: username.to_string(),
        }
    }
}
