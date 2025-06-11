use serde::{Deserialize, Serialize};

use crate::logger::log_with_time;

use super::{
	action::{AsRaw, SafeAction},
	coordinate::Coordinate,
	crafting_material::CraftingMaterial,
	gametraits::{Craftable, IsOwned, Spawnable},
	id_counter::IdCounter,
	planet::{OrbitInfoMap, Planet},
	planet_util::get_timefactor,
};

#[derive(Deserialize, Serialize, Debug, Clone, Default)]
pub struct Spaceship {
	pub id: usize,
	pub owner: String,
	pub speed: f64,
	pub velocity: Coordinate,
	pub position: Coordinate,
	pub target: Coordinate,
	pub docking_mode: bool,
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
	pub fn new(owner: &String, speed: f64, id_counter: &mut IdCounter) -> Self {
		let ship = Self {
			id: id_counter.assign(),
			owner: owner.clone(),
			speed,
			docking_mode: true,
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
		for _ in 0..100 {
			planet_positon =
				planet.get_position_at(get_timefactor(d), orbit_info_map);
			let new_duration_to = self.duration_to(&planet_positon);
			let diff = (duration_to - new_duration_to).abs();
			if diff <= std::f64::EPSILON * 2. {
				break;
			}
			duration_to = new_duration_to;
			d = julian_day + duration_to;
		}
		planet_positon
	}
	pub fn duration_to(&self, target: &Coordinate) -> f64 {
		let mut my_position = self.position.c();
		let distance_vec = my_position.to(target);
		let distance = distance_vec.norm();
		distance / self.speed
	}
	pub fn update(&self, delta_days: f64) -> Vec<SafeAction> {
		let mut actions = Vec::<SafeAction>::new();

		// no need to execute if arrived
		if self.position == self.target {
			return actions;
		}

		// update velocity and position
		let mut newv = self.position.clone();
		newv.to(&self.target).normalize(self.speed);

		actions.push(SafeAction::SetCoordinate {
			coordinate: self.velocity.raw_mut(),
			other: newv,
		});
		actions.push(SafeAction::AddCoordinate {
			coordinate: self.position.raw_mut(),
			other: self.velocity.c(),
			multiplier: delta_days,
		});

		// check if could arrive in this frame
		let mut distance_to_target = self.position.c();
		distance_to_target.to(&self.target);
		let norm = distance_to_target.norm();

		let frame_flight_distance = self.velocity.norm() * delta_days;
		if norm < frame_flight_distance {
			log_with_time("spaceship arrived at destination");
			actions.push(SafeAction::SetCoordinate {
				coordinate: self.position.raw_mut(),
				other: self.target.clone(),
			});
			actions.push(SafeAction::SetCoordinate {
				coordinate: self.velocity.raw_mut(),
				other: Coordinate::default(),
			});
		}

		actions
	}
}
