use serde::{Deserialize, Serialize};

use super::{coordinate::Coordinate, factory::Factory};

/// region to craft / placing buildings such as factories on
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct BuildingRegion {
	relative_position: Coordinate,
	factories: Vec<Factory>,
}

impl BuildingRegion {
	pub fn new(relative_position: Coordinate) -> Self {
		Self {
			relative_position,
			..Default::default()
		}
	}
}
