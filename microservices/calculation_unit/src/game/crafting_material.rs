use std::ops::{AddAssign, SubAssign};

use serde::{Deserialize, Serialize};

/// materials used for crafting something
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct CraftingMaterial {
	pub copper: f64,
	pub steel: f64,
	pub gold: f64,
	pub wood: f64,
}

impl SubAssign for CraftingMaterial {
    fn sub_assign(&mut self, rhs: Self) {
    	self.sub(&rhs);
    }
}

impl AddAssign for CraftingMaterial {
    fn add_assign(&mut self, rhs: Self) {
    	self.add(&rhs);
    }
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
