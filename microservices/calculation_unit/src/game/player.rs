use serde::{Deserialize, Serialize};

use super::{
	coordinate::Coordinate,
	gametraits::{Buyer, Crafter, IsOwned},
};

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct Player {
	pub username: String,
	pub money: f64,
	pub crafting_material: CraftingMaterial,
}

impl IsOwned for Player {
	fn get_owner<'a>(&'a self) -> &'a String {
		&self.username
	}

	fn get_owner_mut<'a>(&'a mut self) -> &'a mut String {
		&mut self.username
	}
}

impl Buyer for Player {
	fn get_money_mut(&mut self) -> &mut f64 {
		&mut self.money
	}
	fn get_position(&self) -> Coordinate {
		// TODO spawn on base or something
		Coordinate::default()
	}

	fn get_owner(&self) -> &String {
		&self.username
	}
}

impl Crafter for Player {
	fn get_crafting_material_mut(&mut self) -> &mut CraftingMaterial {
		&mut self.crafting_material
	}
	fn get_position(&self) -> Coordinate {
		// TODO spawn on base or something
		Coordinate::default()
	}
}

impl Player {
	pub fn new(username: String) -> Self {
		let player = Self {
			username,
			..Default::default()
		};
		player
	}
}

/// materials used for crafting something
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct CraftingMaterial {
	// TODO
}
