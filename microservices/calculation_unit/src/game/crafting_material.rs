use serde::{Deserialize, Serialize};

/// materials used for crafting something
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct CraftingMaterial {
	pub copper: f64,
	pub steel: f64,
	pub gold: f64,
	pub wood: f64,
}

impl CraftingMaterial {
	/// chainable altering operation to subract
	pub fn sub(&mut self, other: &Self) -> &mut Self {
		self.copper -= other.copper;
		self
	}
	pub fn add(&mut self, other: &Self) -> &mut Self {
		self.copper += other.copper;
		self
	}
	pub fn new_copper(copper: f64) -> Self {
		Self {
			copper,
			..Default::default()
		}
	}
}
