use serde::{Deserialize, Serialize};

use crate::logger::log_with_time;

use super::{
	action::{ActionBox, AddValue, AsRaw, SafeAction, SetValue, SubValue},
	coordinate::Coordinate,
	crafting_material::CraftingMaterial,
	gametraits::{Craftable, IsOwned, Spawnable},
	id_counter::IdCounter,
	planet::{OrbitInfoMap, Planet},
	planet_util::get_timefactor,
};

/// spacestation flying around each planet
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct Spacestation {
	parked: usize,
	capacity: usize,
}

impl Spacestation {
	pub fn new() -> Self {
		Self {
			parked: 0,
			capacity: 100000,
		}
	}
}

/// flying around the spaceship transporting troops and resources (incoming)
#[derive(Deserialize, Serialize, Debug, Clone, Default)]
pub struct Spaceship {
	pub id: usize,
	pub owner: String,
	pub speed: f64,
	pub velocity: Coordinate,
	pub position: Coordinate,
	pub target: Coordinate,
	pub docking_mode: bool,
	pub docking_at: Option<usize>,
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
	/// arriving at a spacestation which is on a planet
	pub fn arrive(
		&self,
		planet: &Planet,
		planet_index: usize,
	) -> Result<Vec<ActionBox>, String> {
		log_with_time(format!(
			"spaceship docking at space station on planet {}",
			planet_index
		));
		let mut actions = Vec::<ActionBox>::new();
		let spacestation = &planet.spacestation;
		if spacestation.parked >= spacestation.capacity {
			return Err(format!("space station is full, docking failed"));
		}
		// self.docking_at = Some(planet_index);
		actions.push(SetValue::new(
			self.docking_at.raw_mut(),
			Some(planet_index),
		));
		// spacestation.capacity += 1;
		actions.push(AddValue::new(spacestation.capacity.raw_mut(), 1));
		Ok(actions)
	}
	/// departing from a spacestation which is on a planet
	pub fn depart(&self, planet: &Planet) -> Vec<ActionBox> {
		let mut actions = Vec::<ActionBox>::new();
		// self.docking_at = None;
		actions.push(SetValue::new(self.docking_at.raw_mut(), None));
		// planet.spacestation.capacity -= 1;
		actions.push(SubValue::new(
			planet.spacestation.capacity.raw_mut(),
			1,
		));
		// self.position.set(&planet.position);
		// actions.push(SafeAction::SetCoordinate {
		// 	coordinate: self.position.raw_mut(),
		// 	other: planet.position.c(),
		// });
		actions.push(SetValue::new(self.position.raw_mut(), planet.position.c()));
		actions
	}
	/// default constructor
	pub fn new(owner: &String, speed: f64, id_counter: &mut IdCounter, position: Coordinate) -> Self {
		let ship = Self {
			id: id_counter.assign(),
			owner: owner.clone(),
			speed,
			docking_mode: true,
			position,
			..Self::default()
		};
		ship
	}
	/// calculaton of the destination point depending on a planets orbit
	pub fn fly_to_get_target(
		&self,
		planet: &Planet,
		julian_day: f64,
		orbit_info_map: &OrbitInfoMap,
	) -> Coordinate {
		let mut d = julian_day;
		let mut planet_duration = 0.;
		let mut planet_positon = Coordinate::default();
		for _ in 0..100 {
			planet_positon =
				planet.get_position_at(get_timefactor(d), orbit_info_map);
			let new_duration_to = self.duration_to(&planet_positon);
			let diff = (planet_duration - new_duration_to).abs();
			println!("diff {}", diff);
			if diff <= std::f64::EPSILON * 2. {
				break;
			}
			planet_duration = new_duration_to;
			d = julian_day + planet_duration;
		}
		planet_positon
	}
	/// duration for a spaceship to arrive at a coordinate
	pub fn duration_to(&self, target: &Coordinate) -> f64 {
		let mut my_position = self.position.c();
		let distance_vec = my_position.to(target);
		let length = distance_vec.norm();
		length / self.speed
	}
	/// update one spaceship (flying to target mostly)
	pub fn update(&self, delta_days: f64) -> Vec<ActionBox> {
		let mut actions = Vec::<ActionBox>::new();

		// no need to execute if arrived
		if self.position == self.target {
			return actions;
		}

		// update velocity and position
		let mut newv = self.position.clone();
		newv.to(&self.target).normalize(self.speed);

		actions.push(SetValue::new(self.velocity.raw_mut(), newv));
		let mut vd = self.velocity.c();
		vd.scale(delta_days);
		actions.push(AddValue::new(self.position.raw_mut(), vd));

		// check if could arrive in this frame
		let mut distance_to_target = self.position.c();
		distance_to_target.to(&self.target);
		let norm = distance_to_target.norm();

		let frame_flight_distance = self.velocity.norm() * delta_days;
		if norm < frame_flight_distance {
			log_with_time("spaceship arrived at destination");
			actions.push(SetValue::new(self.position.raw_mut(), self.target.clone()));
			actions.push(SetValue::new(self.velocity.raw_mut(), Coordinate::default()));
		}

		actions
	}
}
