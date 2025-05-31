use std::collections::HashMap;

use serde::{Deserialize, Serialize};

use crate::logger::log_with_time;

use super::{action::{AsRaw, SafeAction}, coordinate::Coordinate, dummy::DummyObject, orbit::OrbitInfo, planet::calculate_planet};

/// objects that are going to be rendered by client
#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct GameObjects {
	pub dummies: HashMap<String, DummyObject>,
	pub planets: Vec<Planet>
}

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct Planet {
	name: String,
	position: Coordinate,
}

impl Planet {
	pub fn update(&mut self, time: f64, orbit_info_map: &HashMap<String, fn(f64) -> OrbitInfo>) {
		let get_planet = orbit_info_map.get(&self.name).unwrap();
		let orbit_info = &(*get_planet)(time);
		self.position = calculate_planet(&orbit_info);
	}
	pub fn new(name: &str) -> Self {
		Self {
			name: name.to_string(),
			..Default::default()
		}
	}
}

// leight weight creating and no cloning needed
#[derive(Serialize, Debug, Clone)]
pub struct SendGameObjects<'a> {
	pub dummies: HashMap<&'a String, &'a DummyObject>,
	pub planets: Vec<&'a Planet>
}

impl GameObjects {
	pub fn new() -> Self {
		let planets = vec![
			Planet::new("mercury"),
			Planet::new("venus"),
			Planet::new("mars"),
			Planet::new("jupiter"),
			Planet::new("saturn"),
			Planet::new("uranus"),
			Planet::new("neptune"),
			Planet::new("earth"),
		];
		Self {
			dummies: HashMap::<String, DummyObject>::new(),
			planets,
		}
	}

	/// leight weight preparation for sending the gameobjects to the client
	pub fn prepare_for(&self, user: &String) -> SendGameObjects {
		SendGameObjects {
			dummies: self
				.dummies
				.iter()
				.filter(|(id, dummy)| {
					// TODO filter for each user
					true
				})
				.collect::<HashMap<&String, &DummyObject>>(),
			planets: self.planets.iter().collect(),
		}
	}

	/// updates the game objects
	pub fn update(
		&mut self,
		delta_seconds: f64,
		ingame_time: f64, orbit_info_map: &HashMap<String, fn(f64) -> OrbitInfo>
	) -> std::result::Result<(), String> {
		let mut actions = Vec::<SafeAction>::new();

		// TODO multithreaded updates
		// get actions
		self.update_dummies(&mut actions, delta_seconds)?;
		self.update_planets(ingame_time, orbit_info_map)?;

		// execute operations on data via raw pointers
		for action in actions {
			action.execute();
		}
		Ok(())
	}

	/// store the actions that are going to be executed on dummies
	pub fn update_dummies(
		&self,
		actions: &mut Vec<SafeAction>,
		delta_seconds: f64,
	) -> std::result::Result<(), String> {
		for (id, dummy) in self.dummies.iter() {
			// dummy.position.addd(&dummy.velocity, delta_seconds);
			actions.push(SafeAction::AddCoordinate {
				coordinate: dummy.position.raw_mut(),
				other: dummy.velocity.raw(),
				multiplier: delta_seconds,
			});
		}
		Ok(())
	}

	pub fn update_planets(&mut self, ingame_time: f64, orbit_info_map: &HashMap<String, fn(f64) -> OrbitInfo> ) -> std::result::Result<(), String> {
		// log_with_time("update planets");
		for planet in self.planets.iter_mut() {
			planet.update(ingame_time, orbit_info_map);
		}
		Ok(())
	}
}
