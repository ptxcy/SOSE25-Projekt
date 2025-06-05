use super::{
	action::{AsRaw, SafeAction}, building_region::BuildingRegion, coordinate::Coordinate, orbit::OrbitInfo, planet_util::calculate_planet
};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct Planet {
	name: String,
	position: Coordinate,
	building_regions: Vec<BuildingRegion>,
}

impl Planet {
	pub fn update(
		&self,
		time: f64,
		orbit_info_map: &HashMap<String, fn(f64) -> OrbitInfo>,
	) -> SafeAction {
		let get_planet = orbit_info_map.get(&self.name).unwrap();
		let orbit_info = &(*get_planet)(time);
		SafeAction::SetCoordinate {
			coordinate: self.position.raw_mut(),
			other: calculate_planet(&orbit_info),
		}
	}
	pub fn new(name: &str) -> Self {
		Self {
			name: name.to_string(),
			..Default::default()
		}
	}
}
