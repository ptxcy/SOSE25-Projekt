use serde::{Deserialize, Serialize};

use crate::logger::log_with_time;

use super::{
	action::SafeAction,
	building_region::BuildingRegion,
	coordinate::Coordinate,
	crafting_material::CraftingMaterial,
	gametraits::{
		Craftable, Crafter, HasPosition, HasRelativePosition, IsOwned,
		Spawnable, Spawner,
	},
};

/// placed somewhere to mine resources / crafting materials
#[derive(Serialize, Debug, Clone)]
pub struct Factory {
	pub owner: String,
	pub storage: CraftingMaterial,

	#[serde(skip)]
	region: *mut BuildingRegion,
}

// Helper struct for deserialization
#[derive(Deserialize, Debug, Clone)]
pub struct FactoryReceive {
	pub owner: String,
	pub storage: CraftingMaterial,
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

impl Spawner for Factory {
	fn spawn_at(&self) -> Coordinate {
		self.get_position()
	}
}

impl Crafter for Factory {
	fn get_crafting_material_mut(&mut self) -> &mut CraftingMaterial {
		&mut self.storage
	}
}

impl Factory {
	pub fn new(owner: &String, region: *mut BuildingRegion) -> Self {
		Self {
			owner: owner.to_string(),
			region,
			storage: Default::default(),
		}
	}
	pub fn craft<'a, T: Crafter>(
		crafter: &mut T,
		building_region: *mut BuildingRegion,
	) -> SafeAction {
		log_with_time("crafting a dummy with 49 copper");
		// use materials
		crafter.get_crafting_material_mut().sub(&Self::get_cost());

		// create object
		let factory = Factory::new(crafter.get_owner(), building_region);
		factory.into_game_objects()
	}
}

impl Spawnable for Factory {
	fn into_game_objects(self) -> SafeAction {
		SafeAction::SpawnFactory {
			region: self.region,
			factory: self,
		}
	}
}

impl HasRelativePosition for Factory {
	type Parent = BuildingRegion;
	fn get_parent(&self) -> &Self::Parent {
		unsafe { &(*self.region) }
	}
	fn get_relative_position(&self) -> Coordinate {
		Coordinate::new(0., 0., 0.)
	}
}
