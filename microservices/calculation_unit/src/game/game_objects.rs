use std::collections::HashMap;

use serde::{Deserialize, Serialize};

use super::dummy::DummyObject;

/// objects that are going to be rendered by client
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
	pub fn prepare_for(&self, user: &String) -> Self {
		// TODO make more leight weight
		self.clone()
	}
}
