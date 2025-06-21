use std::{cmp::Ordering, ops::{AddAssign, SubAssign}};

use serde::{Deserialize, Serialize};

/// materials used for crafting something
#[derive(Serialize, Deserialize, Debug, Clone, Default, Copy)]
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
	pub fn scale(&mut self, value: f64) -> &mut Self {
		self.copper *= value;
		self.steel *= value;
		self.gold *= value;
		self.wood *= value;
		self
	}
	pub fn mul(&mut self, other: &Self) -> &mut Self {
		self.copper *= other.copper;
		self.steel *= other.steel;
		self.gold *= other.gold;
		self.wood *= other.wood;
		self
	}
	pub fn trim(&mut self) -> &mut Self {
		let values = [self.copper, self.steel, self.gold, self.wood];
		let max = values.iter() .max_by(|a, b| a.partial_cmp(b).unwrap_or(Ordering::Equal)) .unwrap();
	    self.copper /= max;
		self.steel /= max;
		self.gold /= max;
		self.wood /= max;
		self
	}
	pub fn new_copper(copper: f64) -> Self {
		Self {
			copper,
			..Default::default()
		}
	}
}
