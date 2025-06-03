use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct Player {
	username: String,
	money: f32,
}

impl Player {
	pub fn new(username: String) -> Self {
		Self {
			username,
			money: 0.,
		}
	}
}
