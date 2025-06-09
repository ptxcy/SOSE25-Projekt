use serde::{Deserialize, Serialize};

use crate::logger::log_with_time;

use super::{
	action::{AsRaw, SafeAction},
	coordinate::Coordinate,
	crafting_material::CraftingMaterial,
	game_objects::SpaceshipMap,
	gametraits::{Craftable, IsOwned, Spawnable},
	id_counter::IdCounter,
	planet::{OrbitInfoMap, Planet},
	planet_util::get_timefactor,
};

#[derive(Serialize, Debug, Clone, Default)]
pub struct Spacestation {
	spaceships: SpaceshipMap,
}

impl Spacestation {
	pub fn depart(&mut self, id: usize, spaceships: *mut SpaceshipMap) {
		match self.spaceships.remove(&id) {
			Some(ship) => unsafe {
				(*spaceships).insert(id, ship);
			},
			None => {
				log_with_time(format!(
					"no spaceship that can depart with that id: {}",
					id
				));
			}
		};
	}
}

#[derive(Deserialize, Serialize, Debug, Clone, Default)]
pub struct Spaceship {
	pub id: usize,
	pub owner: String,
	pub speed: f64,
	pub velocity: Coordinate,
	pub position: Coordinate,
}

impl Craftable for Spaceship {
	fn get_cost() -> super::crafting_material::CraftingMaterial {
		CraftingMaterial::new_copper(2000.)
	}
}

impl IsOwned for Spaceship {
	fn get_owner<'a>(&'a self) -> &'a String {
		&self.owner
	}

	fn get_owner_mut<'a>(&'a mut self) -> &'a mut String {
		&mut self.owner
	}
}

impl Spawnable for Spaceship {
	fn into_game_objects(self) -> SafeAction {
		SafeAction::SpawnSpaceship(self)
	}
}

impl Spaceship {
	pub fn arrive(self, spacestation: &mut Spacestation) {
		spacestation.spaceships.insert(self.id, self);
	}
	pub fn new(owner: &String, speed: f64, id_counter: &mut IdCounter) -> Self {
		let ship = Self {
			id: id_counter.assign(),
			owner: owner.clone(),
			speed,
			..Default::default()
		};
		ship
	}
	pub fn fly_to_get_target(
		&self,
		planet: &Planet,
		julian_day: f64,
		orbit_info_map: &OrbitInfoMap,
	) -> Coordinate {
		let mut d = julian_day;
		let mut duration_to = 0.;
		let mut planet_positon = Coordinate::default();
		loop {
			planet_positon =
				planet.get_position_at(get_timefactor(d), orbit_info_map);
			let new_duration_to = self.duration_to(&planet_positon);
			if (duration_to - new_duration_to).abs() <= std::f64::EPSILON * 2. {
				break;
			}
			duration_to = new_duration_to;
			d += duration_to;
		}
		planet_positon
	}
	pub fn duration_to(&self, target: &Coordinate) -> f64 {
		let mut my_position = self.position.c();
		let distance_vec = my_position.to(target);
		let distance = distance_vec.norm();
		distance / self.speed
	}
	pub fn update(&self, delta_days: f64) -> SafeAction {
		SafeAction::AddCoordinate {
			coordinate: self.position.raw_mut(),
			other: self.velocity.c(),
			multiplier: delta_days,
		}
	}
}
