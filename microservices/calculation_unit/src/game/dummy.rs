use serde::{Deserialize, Serialize};

use super::{
	coordinate::Coordinate,
	crafting_material::CraftingMaterial,
	gametraits::{Craftable, Crafter, IsOwned},
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
	fn get_cost() -> CraftingMaterial {
		CraftingMaterial { copper: 49. }
	}

	fn craft<'a, T: Crafter>(
		crafter: &mut T,
		name: &'a String,
		game_objects: &'a mut super::game_objects::GameObjects,
		id_counter: &'a mut usize,
	) -> &'a mut Self {
		// use materials
		crafter.get_crafting_material_mut().sub(&Self::get_cost());

		// create object
		let dummy = DummyObject::new(crafter.get_owner(), name, id_counter);
		let id = dummy.id;
		game_objects.dummies.insert(id, dummy);
		game_objects.dummies.get_mut(&id).unwrap()
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
