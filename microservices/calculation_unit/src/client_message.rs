use serde::{Serialize, Deserialize};

use crate::server_message::ClientMessage;

#[derive(Serialize, Deserialize, Debug)]
pub struct ServerMessage {
	pub author: String,
	pub content: String,
}

impl ServerMessage {
	pub fn response_to(client_message: &ClientMessage) -> Self {
		ServerMessage {
			author: "server".to_owned(),
			content: format!("hey {}", client_message.author).to_owned(),
		}
	}
}
