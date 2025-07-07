use serde::{Deserialize, Serialize};


#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct Score {
	pub player1: u16,
	pub player2: u16,
}
