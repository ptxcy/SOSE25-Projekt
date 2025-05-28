use std::collections::HashMap;

use serde::{Deserialize, Serialize};

use super::dummy::DummyObject;

/// objects that are going to be rendered by client
#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct GameObjects {
	pub dummies: HashMap<String, DummyObject>,
}

// leight weight creating and no cloning needed
#[derive(Serialize, Debug, Clone)]
pub struct SendGameObjects<'a> {
	pub dummies: HashMap<&'a String, &'a DummyObject>,
}

impl GameObjects {
	pub fn new() -> Self {
		Self {
			dummies: HashMap::<String, DummyObject>::new(),
		}
	}
	pub fn prepare_for(&self, user: &String) -> SendGameObjects {
		SendGameObjects {
			dummies: self.dummies.iter().filter(|(id, dummy)| {
				false
			})
			.collect::<HashMap<&String, &DummyObject>>()
		}
	}
}
