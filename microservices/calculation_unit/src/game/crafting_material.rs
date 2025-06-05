use serde::{Deserialize, Serialize};

/// materials used for crafting something
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct CraftingMaterial {
	pub copper: f64,
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
}
