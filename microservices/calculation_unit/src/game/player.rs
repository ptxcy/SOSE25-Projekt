use serde::{Deserialize, Serialize};

use super::{
	coordinate::Coordinate,
	crafting_material::CraftingMaterial,
	gametraits::{Buyer, Crafter, IsOwned, Spawner},
};

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct Player {
	pub username: String,
	pub money: f64,
	pub crafting_material: CraftingMaterial,

	#[serde(skip)]
	pub selected_building_region: Option<i32>,
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
}

impl Crafter for Player {
	fn get_crafting_material_mut(&mut self) -> &mut CraftingMaterial {
		&mut self.crafting_material
	}

	fn get_crafting_material(&self) -> &CraftingMaterial {
		&self.crafting_material
    }
}

impl Player {
	pub fn new(username: String) -> Self {
		let player = Self {
			username,
			crafting_material: CraftingMaterial::new_copper(500.),
			..Default::default()
		};
		player
	}
}

impl Spawner for Player {
	fn spawn_at(&self) -> Coordinate {
		Coordinate::default()
	}
}
