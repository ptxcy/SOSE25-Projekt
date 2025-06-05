use serde::{Deserialize, Serialize};

use super::{coordinate::Coordinate, crafting_material::CraftingMaterial, factory::Factory};

/// region to craft / placing buildings such as factories on
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct BuildingRegion {
	pub relative_position: Coordinate,
	pub factories: Vec<Factory>,
	pub factory_profit: CraftingMaterial,
}

impl BuildingRegion {
	pub fn new(relative_position: Coordinate) -> Self {
		Self {
			relative_position,
			..Default::default()
		}
	}
}
