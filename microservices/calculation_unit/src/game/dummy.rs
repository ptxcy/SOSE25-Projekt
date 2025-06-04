use serde::{Deserialize, Serialize};

use super::{
	coordinate::Coordinate, crafting_material::CraftingMaterial, gametraits::{Craftable, IsOwned}
};

/// dummy object for square rendering
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct DummyObject {
	pub owner: String, // username
	pub id: usize,
	pub name: String,
	pub position: Coordinate,
	pub velocity: Coordinate,
}

impl DummyObject {
	pub fn new(owner: &String, name: &String, id_counter: &mut usize) -> Self {
		let dummy = Self {
			owner: owner.clone(),
			name: name.clone(),
			id: *id_counter,
			..Default::default()
		};
		*id_counter += 1;
		dummy
	}
}

impl Craftable for DummyObject {
    fn get_cost(&self) -> &CraftingMaterial {
    	&CraftingMaterial{}
    }
}

impl IsOwned for DummyObject {
	fn get_owner_mut<'a>(&'a mut self) -> &'a mut String {
		&mut self.owner
	}

	fn get_owner<'a>(&'a self) -> &'a String {
		&self.owner
	}
}
