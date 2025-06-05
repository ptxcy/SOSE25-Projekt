use super::{
	action::{AsRaw, SafeAction},
	building_region::{BuildingRegion, BuildingRegionReceive},
	coordinate::Coordinate,
	orbit::OrbitInfo,
	planet_util::calculate_planet,
};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[derive(Serialize, Debug, Clone, Default)]
pub struct Planet {
	pub name: String,
	pub position: Coordinate,
	pub building_regions: Vec<BuildingRegion>,
	pub size: f64,
}

#[derive(Deserialize, Debug, Clone, Default)]
pub struct PlanetReceive {
	pub name: String,
	pub position: Coordinate,
	pub building_regions: Vec<BuildingRegionReceive>,
	pub size: f64,
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
	pub fn new(name: &str, building_regions: Vec<BuildingRegion>) -> Self {
		Self {
			name: name.to_string(),
			building_regions,
			size: 1.,
			..Default::default()
		}
	}
	pub fn add_building_region(mut self, mut direction: Coordinate) -> Self {
		direction.normalize(self.size);
		self.building_regions.push(BuildingRegion::new(direction, &self as *const Planet));
		self
	}

	pub fn solar_system() -> Vec<Self> {
		let planets = vec![
			Planet::new("mercury", vec![])
				.add_building_region(Coordinate::new(1.0, -1.0, 0.0)),
			Planet::new("venus", vec![])
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
			Planet::new("earth", vec![])
				.add_building_region(Coordinate::new(1.0, -1.0, 0.0)),
		];
		planets
	}
}
