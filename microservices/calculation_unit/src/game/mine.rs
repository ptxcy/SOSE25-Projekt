use std::sync::{Arc, Mutex};

use serde::{Deserialize, Serialize};

use super::{
	action::SafeAction,
	building_region::BuildingRegion,
	crafting_material::CraftingMaterial,
	gametraits::{Craftable, IsOwned, Spawnable},
};

#[derive(Serialize, Debug, Clone)]
pub struct Mine {
	pub owner: String,
	pub storage: CraftingMaterial,

	#[serde(skip)]
	region: *mut BuildingRegion,
	#[serde(skip)]
	validation: Arc<Mutex<bool>>,
}

#[derive(Deserialize, Debug, Clone)]
pub struct MineReceive {
	pub owner: String,
	pub storage: CraftingMaterial,
}

impl IsOwned for Mine {
	fn get_owner<'a>(&'a self) -> &'a String {
		&self.owner
	}

	fn get_owner_mut<'a>(&'a mut self) -> &'a mut String {
		&mut self.owner
	}
}

impl Craftable for Mine {
	fn get_cost() -> CraftingMaterial {
		CraftingMaterial::new_copper(100.)
	}
}

impl Mine {
	pub fn new(owner: &String, building_region: *mut BuildingRegion) -> Self {
		Self {
			owner: owner.to_string(),
			region: building_region,
			storage: Default::default(),
			validation: Arc::new(Mutex::new(true)),
		}
	}
}

impl Spawnable for Mine {
	fn into_game_objects(self) -> SafeAction {
		SafeAction::SpawnMine {
			region: self.region,
			mine: self,
		}
	}
}
