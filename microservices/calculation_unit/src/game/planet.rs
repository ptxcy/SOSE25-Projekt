use super::{
	action::{ActionWrapper, AsRaw, SafeAction, SetValue},
	building_region::{BuildingRegion, BuildingRegionReceive},
	coordinate::Coordinate,
	gametraits::HasPosition,
	orbit::OrbitInfo,
	planet_util::calculate_planet,
	spaceship::Spacestation,
};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

pub type OrbitInfoMap = HashMap<String, fn(f64) -> OrbitInfo>;

/// planet in solar system containing more information
#[derive(Serialize, Debug, Clone, Default)]
pub struct Planet {
	pub name: String,
	pub position: Coordinate,
	pub building_regions: Vec<BuildingRegion>,
	pub size: f64,
	pub spacestation: Spacestation,
}

/// planet in solar system containing more information for receing
#[derive(Deserialize, Debug, Clone, Default)]
pub struct PlanetReceive {
	pub name: String,
	pub position: Coordinate,
	pub building_regions: Vec<BuildingRegionReceive>,
	pub size: f64,
	pub spacestation: Spacestation,
}

impl Planet {
	pub fn get_position_at(
		&self,
		timefactor: f64,
		orbit_info_map: &OrbitInfoMap,
	) -> Coordinate {
		let get_planet = orbit_info_map.get(&self.name).unwrap();
		let orbit_info = &(*get_planet)(timefactor);
		calculate_planet(&orbit_info)
	}
	pub fn update(
		&self,
		timefactor: f64,
		orbit_info_map: &OrbitInfoMap,
	) -> ActionWrapper {
		let new_position = self.get_position_at(timefactor, orbit_info_map);
		// SafeAction::SetCoordinate {
		// 	coordinate: self.position.raw_mut(),
		// 	other: new_position,
		// }
		SetValue::new(self.position.raw_mut(), new_position)
	}
	pub fn new(name: &str, building_regions: Vec<BuildingRegion>) -> Self {
		Self {
			name: name.to_string(),
			building_regions,
			size: 1.,
			position: Coordinate::default(),
			spacestation: Spacestation::new(),
		}
	}
	pub fn add_building_region(mut self, mut direction: Coordinate) -> Self {
		direction.normalize(self.size);
		self.building_regions
			.push(BuildingRegion::new(direction, &self as *const Planet));
		self
	}

	pub fn solar_system() -> Vec<Self> {
		let planets = vec![
			Planet::new("mercury", vec![])
				.add_building_region(Coordinate::new(1.0, -1.0, 0.0)),
			Planet::new("venus", vec![])
				.add_building_region(Coordinate::new(1.0, -1.0, 0.0)),
			Planet::new("earth", vec![])
				.add_building_region(Coordinate::new(1.0, -1.0, 0.0)),
			Planet::new("mars", vec![])
				.add_building_region(Coordinate::new(1.0, -1.0, 0.0)),
			Planet::new("jupiter", vec![])
				.add_building_region(Coordinate::new(1.0, -1.0, 0.0)),
			Planet::new("saturn", vec![])
				.add_building_region(Coordinate::new(1.0, -1.0, 0.0)),
			Planet::new("uranus", vec![])
				.add_building_region(Coordinate::new(1.0, -1.0, 0.0)),
			Planet::new("neptune", vec![])
				.add_building_region(Coordinate::new(1.0, -1.0, 0.0)),
		];
		planets
	}
}

impl HasPosition for Planet {
	fn get_position(&self) -> Coordinate {
		self.position.clone()
	}
}
