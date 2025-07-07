use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct RequestData {
    pub connect: bool,
    pub move_to: i8, // -1 down, 0 nothing, 1 up
    pub lobby: Option<String>,
}

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct ClientMessage {
    pub request_data: RequestData,
    pub username: String,
}

impl ClientMessage {
    pub fn connect(username: &str) -> Self {
        ClientMessage {
            request_data: RequestData { connect: true, move_to: 0, lobby: None },
            username: username.to_string(),
        }
    }
    pub fn move_to(value: i8, username: &str) -> Self {
        ClientMessage { request_data: RequestData {
    		connect: false,
    		move_to: value,
            lobby: None,
    	}, username: username.to_string() }
    }
}
