use serde::{Deserialize, Serialize};

use crate::logger::log_with_time;

use super::{
	action::{ActionBox, AsRaw, SafeAction, SubValue},
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
		crafter: &T,
		name: &'a String,
		id_counter: &'a mut IdCounter,
	) -> Vec<ActionBox> {
		log_with_time("crafting a dummy with 49 copper");
		// use materials
		let mut actions = Vec::<ActionBox>::new();
		// actions.push(Box::new(SafeAction::ReduceCraftingMaterial {
		// 	crafter: crafter as *const dyn Crafter as *mut dyn Crafter,
		// 	cost: Self::get_cost(),
		// }));
		actions.push(SubValue::new(crafter.get_crafting_material().raw_mut(), Self::get_cost()));

		// create object
		let dummy = DummyObject::new(
			crafter.get_owner(),
			name,
			id_counter,
			crafter.spawn_at(),
		);
		let id = dummy.id;
		actions.push(Box::new(dummy.into_game_objects()));
		actions
	}
}

impl Craftable for DummyObject {
	fn get_cost() -> CraftingMaterial {
		CraftingMaterial::new_copper(49.)
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
	fn into_game_objects(self) -> SafeAction {
		SafeAction::SpawnDummy(self)
	}
}
