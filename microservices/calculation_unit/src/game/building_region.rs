use std::ptr::null;

use serde::{Deserialize, Serialize};

use super::{
	coordinate::Coordinate, crafting_material::CraftingMaterial,
	factory::{Factory, FactoryReceive}, mine::{Mine, MineReceive}, planet::Planet,
};

/// region to craft / placing buildings such as factories on
#[derive(Serialize, Debug, Clone)]
pub struct BuildingRegion {
	pub relative_position: Coordinate,
	pub factories: Vec<Factory>,
	pub mines: Vec<Mine>,
	pub profit: CraftingMaterial,

	#[serde(skip)]
	planet: *const Planet,
}

#[derive(Deserialize, Debug, Clone, Default)]
pub struct BuildingRegionReceive {
	pub relative_position: Coordinate,
	pub factories: Vec<FactoryReceive>,
	pub mines: Vec<MineReceive>,
	pub profit: CraftingMaterial,
}

impl BuildingRegion {
	pub fn new(relative_position: Coordinate, planet: *const Planet) -> Self {
		Self {
			relative_position,
			planet,
		    factories: Default::default(),
		    mines: Default::default(),
		    profit: Default::default(),
		}
	}
}

unsafe impl Sync for BuildingRegion {}
unsafe impl Send for BuildingRegion {}
