use std::collections::HashMap;

use serde::{Deserialize, Serialize};

use super::dummy::DummyObject;

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct GameObjects {
	pub dummies: HashMap<String, DummyObject>,
}

impl GameObjects {
	pub fn new() -> Self {
		Self {
			dummies: HashMap::<String, DummyObject>::new(),
		}
	}
}
