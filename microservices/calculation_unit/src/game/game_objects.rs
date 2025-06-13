use std::{
	collections::HashMap,
	sync::{mpsc::*, *},
	thread::JoinHandle,
};

use serde::{Deserialize, Serialize};

use super::{
	action::{Action, ActionBox, AddValue, AsRaw, SafeAction},
	dummy::DummyObject,
	planet::{OrbitInfoMap, Planet, PlanetReceive},
	player::Player,
	spaceship::Spaceship,
};

pub type PlayerMap = HashMap<String, Player>;
pub type SpaceshipMap = HashMap<usize, Spaceship>;
pub type DummyMap = HashMap<usize, DummyObject>;

pub type SendPlayerMap<'a> = HashMap<&'a String, &'a Player>;
pub type SendSpaceshipMap<'a> = HashMap<&'a usize, &'a Spaceship>;
pub type SendDummyMap<'a> = HashMap<&'a usize, &'a DummyObject>;

/// objects that are going to be rendered by client
#[derive(Serialize, Debug, Clone, Default)]
pub struct GameObjects {
	pub dummies: DummyMap,
	pub planets: Vec<Planet>,
	pub players: HashMap<String, Player>,
	pub spaceships: SpaceshipMap,
}

/// for dummy client in rust
#[derive(Deserialize, Debug, Clone, Default)]
pub struct GameObjectsReceive {
	pub dummies: DummyMap,
	pub planets: Vec<PlanetReceive>,
	pub players: HashMap<String, Player>,
	pub spaceships: SpaceshipMap,
}

/// leight weight creating and no cloning needed
#[derive(Serialize, Debug, Clone, Default)]
pub struct SendGameObjects<'a> {
	pub dummies: SendDummyMap<'a>,
	pub planets: Vec<&'a Planet>,
	pub players: SendPlayerMap<'a>,
	pub spaceships: SendSpaceshipMap<'a>,
}

impl GameObjects {
	pub fn new() -> Self {
		let planets = Planet::solar_system();
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
			spaceships: self
				.spaceships
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
		game_objects: &mut GameObjects,
		delta_ingame_days: f64,
		timefactor: f64,
		orbit_info_map: &OrbitInfoMap,
	) -> std::result::Result<(), String> {
		let (action_sender, action_receiver) =
			std::sync::mpsc::channel::<Box<dyn Action>>();

		// get actions multithreaded
		let mut threads = Vec::<JoinHandle<()>>::new();
		threads.push(GameObjects::update_dummies(
			action_sender.clone(),
			game_objects as *const GameObjects,
			delta_ingame_days,
		));
		threads.push(GameObjects::update_planets(
			action_sender.clone(),
			game_objects as *const GameObjects,
			timefactor,
			orbit_info_map,
		));
		threads.push(GameObjects::update_spaceships(
			action_sender.clone(),
			game_objects as *const GameObjects,
			delta_ingame_days,
		));
		threads.push(GameObjects::update_spaceship_planet_relations(
			action_sender.clone(),
			game_objects as *const GameObjects,
			delta_ingame_days
		));

		// wait for all threads to finish collecting their actions
		for thread in threads {
			thread.join().unwrap();
		}

		// execute operations on data via raw pointers
		while let Ok(action) = action_receiver.try_recv() {
			// WARNING super unsafe so far not well tested
			action.execute(game_objects as *mut GameObjects);
		}
		Ok(())
	}

	/// store the actions that are going to be executed on dummies
	pub fn update_dummies(
		action_sender: Sender<ActionBox>,
		game_objects: *const GameObjects,
		delta_ingame_days: f64,
	) -> std::thread::JoinHandle<()> {
		let dummies_handle = std::thread::spawn({
			let dummies = unsafe { &(*game_objects).dummies };
			move || {
				for (id, dummy) in dummies.iter() {
					let mut dv = dummy.velocity.clone();
					dv.scale(delta_ingame_days);

					action_sender
						// .send(Box::new(SafeAction::AddCoordinate {
						// 	coordinate: dummy.position.raw_mut(),
						// 	other: dummy.velocity.clone(),
						// 	multiplier: delta_ingame_days,
						// }))
						.send(AddValue::new(dummy.position.raw_mut(), dv))
						.unwrap();
				}
			}
		});
		dummies_handle
	}

	/// updating the planet position in their orbits
	pub fn update_planets(
		action_sender: Sender<ActionBox>,
		game_objects: *const GameObjects,
		timefactor: f64,
		orbit_info_map: &OrbitInfoMap,
	) -> std::thread::JoinHandle<()> {
		let planets_handle = std::thread::spawn({
			let planets = unsafe { &(*game_objects).planets };
			let orbit_info_map = orbit_info_map.clone();
			move || {
				for planet in planets.iter() {
					action_sender
						.send(Box::new(planet.update(timefactor, &orbit_info_map)))
						.unwrap();
				}
			}
		});
		planets_handle
	}

	pub fn update_spaceships(
		action_sender: Sender<ActionBox>,
		game_objects: *const GameObjects,
		delta_days: f64,
	) -> std::thread::JoinHandle<()> {
		let planets_handle = std::thread::spawn({
			let spaceships = unsafe { &(*game_objects).spaceships };
			move || {
				for (id, spaceship) in spaceships.iter() {
					for action in spaceship.update(delta_days) {
						action_sender.send(Box::new(action)).unwrap();
					}
				}
			}
		});
		planets_handle
	}

	pub fn update_spaceship_planet_relations(
		action_sender: Sender<ActionBox>,
		game_objects: *const GameObjects,
		delta_days: f64
	) -> std::thread::JoinHandle<()> {
		let planets_handle = std::thread::spawn({
			let spaceships = unsafe { &(*game_objects).spaceships };
			let planets = unsafe { &(*game_objects).planets };
			move || {
				for (id, spaceship) in spaceships.iter() {
					for (i, planet) in planets.iter().enumerate() {
						// spaceship docking checks
						let mut distance = planet.position.c();
						distance.sub(&spaceship.position);
						let norm = distance.norm();
						if norm < spaceship.speed * delta_days && spaceship.docking_mode {
							// spaceship can and wants to dock
							let actions = spaceship.arrive(planet, i).unwrap();
							for action in actions {
								action_sender.send(Box::new(action)).unwrap();
							}
						}
					}
				}
			}
		});
		planets_handle
	}
}
