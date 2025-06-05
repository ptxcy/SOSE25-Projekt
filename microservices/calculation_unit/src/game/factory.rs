use serde::{Deserialize, Serialize};

use crate::logger::log_with_time;

use super::{
	building_region::BuildingRegion, crafting_material::CraftingMaterial, dummy::DummyObject, game_objects::DummyMap, gametraits::{Craftable, Crafter, IsOwned}
};

/// placed somewhere to mine resources / crafting materials
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct Factory {
	owner: String,
	/// resources the factory mines per second
	pub resources: CraftingMaterial,
}

impl IsOwned for Factory {
	fn get_owner<'a>(&'a self) -> &'a String {
		&self.owner
	}

	fn get_owner_mut<'a>(&'a mut self) -> &'a mut String {
		&mut self.owner
	}
}

impl Craftable for Factory {
	fn get_cost() -> CraftingMaterial {
		CraftingMaterial { copper: 400. }
	}
}

impl Factory {
	pub fn new(owner: &String, resources: CraftingMaterial) -> Self {
		Self {
		    owner: owner.to_string(),
		    resources,
		}
	}
	pub fn craft<'a, T: Crafter>(
		crafter: &mut T,
		building_region: &'a mut BuildingRegion,
	) -> &'a mut Self {
		log_with_time("crafting a dummy with 49 copper");
		// use materials
		crafter.get_crafting_material_mut().sub(&Self::get_cost());

		// create object
		let factory = Factory::new(crafter.get_owner(), building_region.factory_profit.clone());

		building_region.factories.push(factory);
		let index = building_region.factories.len() - 1;
	    &mut building_region.factories[index]
	}
}
