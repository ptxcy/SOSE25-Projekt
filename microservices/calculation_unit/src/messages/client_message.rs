use serde::{Serialize, Deserialize};


#[derive(Serialize, Deserialize, Debug)]
pub struct ClientMessage {
	pub author: String,
	pub content: String,
}

impl ClientMessage {
}
