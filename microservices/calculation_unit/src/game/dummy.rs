use serde::{Deserialize, Serialize};

use crate::logger::log_with_time;

use super::{
	coordinate::Coordinate,
	crafting_material::CraftingMaterial,
	game_objects::DummyMap,
	gametraits::{Craftable, Crafter, IsOwned, Spawnable},
	id_counter::IdCounter,
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
	pub fn new(
		owner: &String,
		name: &String,
		id_counter: &mut IdCounter,
		position: Coordinate,
	) -> Self {
		let dummy = Self {
			owner: owner.clone(),
			name: name.clone(),
			id: id_counter.assign(),
			position,
			..Default::default()
		};
		dummy
	}

	pub fn craft<'a, T: Crafter>(
		crafter: &mut T,
		name: &'a String,
		dummies: &'a mut DummyMap,
		id_counter: &'a mut IdCounter,
	) -> &'a mut Self {
		log_with_time("crafting a dummy with 49 copper");
		// use materials
		crafter.get_crafting_material_mut().sub(&Self::get_cost());

		// create object
		let dummy = DummyObject::new(
			crafter.get_owner(),
			name,
			id_counter,
			crafter.spawn_at(),
		);
		let id = dummy.id;
		dummies.insert(id, dummy);
		dummies.get_mut(&id).unwrap()
	}
}

impl Craftable for DummyObject {
	fn get_cost() -> CraftingMaterial {
		CraftingMaterial { copper: 49. }
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

impl Spawnable for DummyObject {
	fn into_game_objects(
		self,
		game_objects: &mut super::game_objects::GameObjects,
	) {
		game_objects.dummies.insert(self.id, self);
	}
}
