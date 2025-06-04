use serde::{Deserialize, Serialize};

/// materials used for crafting something
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct CraftingMaterial {
	pub copper: f64,
}

impl CraftingMaterial {
	pub fn sub(&mut self, other: &Self) {
		self.copper -= other.copper;
	}
}

