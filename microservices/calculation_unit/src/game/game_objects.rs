use std::{
	collections::HashMap,
	sync::{Arc, Mutex},
};

use serde::{Deserialize, Serialize};

use super::{
	action::{AsRaw, SafeAction},
	coordinate::Coordinate,
	dummy::DummyObject,
	orbit::OrbitInfo,
	planet::calculate_planet,
	player::Player,
};

type DummyMap = HashMap<usize, DummyObject>;
type SendDummyMap<'a> = HashMap<&'a usize, &'a DummyObject>;

/// objects that are going to be rendered by client
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct GameObjects {
	pub dummies: DummyMap,
	pub planets: Vec<Planet>,
	pub players: HashMap<String, Player>,
}

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct Planet {
	name: String,
	position: Coordinate,
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

// leight weight creating and no cloning needed
#[derive(Serialize, Debug, Clone, Default)]
pub struct SendGameObjects<'a> {
	pub dummies: SendDummyMap<'a>,
	pub planets: Vec<&'a Planet>,
	pub players: HashMap<&'a String, &'a Player>,
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
			planets,
			..Default::default()
		}
	}

	/// leight weight preparation for sending the gameobjects to the client
	pub fn prepare_for(&self, user: &String) -> SendGameObjects {
		SendGameObjects {
			dummies: self
				.dummies
				.iter()
				.filter(|(id, dummy)| {
					/* TODO filter for each user */
					true
				})
				.collect::<SendDummyMap>(),
			planets: self.planets.iter().collect(),
			players: self
				.players
				.iter()
				.filter(|(id, dummy)| {
					/* TODO filter for each user */
					true
				})
				.collect(),
		}
	}

	/// updates the game objects
	pub fn update(
		dummies: Arc<DummyMap>,
		planets: Arc<Vec<Planet>>,
		delta_seconds: f64,
		ingame_time: f64,
		orbit_info_map: &HashMap<String, fn(f64) -> OrbitInfo>,
	) -> std::result::Result<(), String> {
		let actions = Arc::new(Mutex::new(Vec::<SafeAction>::new()));

		// get actions multithreaded
		let dummies_handle = GameObjects::update_dummies(
			Arc::clone(&actions),
			Arc::clone(&dummies),
			Arc::clone(&planets),
			delta_seconds,
			ingame_time,
			orbit_info_map,
		);
		let planets_handle = GameObjects::update_planets(
			Arc::clone(&actions),
			Arc::clone(&dummies),
			Arc::clone(&planets),
			delta_seconds,
			ingame_time,
			orbit_info_map,
		);

		dummies_handle.join().unwrap();
		planets_handle.join().unwrap();

		let actions = actions.lock().unwrap();

		// execute operations on data via raw pointers
		for action in actions.iter() {
			action.execute();
		}
		Ok(())
	}

	/// store the actions that are going to be executed on dummies
	pub fn update_dummies(
		actions: Arc<Mutex<Vec<SafeAction>>>,
		dummies: Arc<DummyMap>,
		planets: Arc<Vec<Planet>>,
		delta_seconds: f64,
		ingame_time: f64,
		orbit_info_map: &HashMap<String, fn(f64) -> OrbitInfo>,
	) -> std::thread::JoinHandle<()> {
		let dummies_handle = std::thread::spawn({
			let actions_clone = actions.clone();
			let delta_seconds = delta_seconds;
			let dummies = Arc::clone(&dummies);
			move || {
				for (id, dummy) in dummies.iter() {
					let mut actions = actions_clone.lock().unwrap();
					actions.push(SafeAction::AddCoordinate {
						coordinate: dummy.position.raw_mut(),
						other: dummy.velocity.clone(),
						multiplier: delta_seconds,
					});
				}
			}
		});
		dummies_handle
	}

	/// updating the planet position in their orbits
	pub fn update_planets(
		actions: Arc<Mutex<Vec<SafeAction>>>,
		dummies: Arc<DummyMap>,
		planets: Arc<Vec<Planet>>,
		delta_seconds: f64,
		ingame_time: f64,
		orbit_info_map: &HashMap<String, fn(f64) -> OrbitInfo>,
	) -> std::thread::JoinHandle<()> {
		let planets_handle = std::thread::spawn({
			let actions_clone = actions.clone();
			let planets = Arc::clone(&planets);
			let orbit_info_map = orbit_info_map.clone();
			let ingame_time = ingame_time;
			move || {
				for planet in planets.iter() {
					let mut actions = actions_clone.lock().unwrap();
					actions.push(planet.update(ingame_time, &orbit_info_map));
				}
			}
		});
		planets_handle
	}
}
