use serde::{Deserialize, Serialize};

use crate::logger::log_with_time;

use super::{
	action::SafeAction, coordinate::Coordinate, crafting_material::CraftingMaterial, game_objects::SpaceshipMap, gametraits::{Craftable, IsOwned, Spawnable}, id_counter::IdCounter
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
	id: usize,
	owner: String,
	pub speed: f64,
	pub velocity: Coordinate,
	pub position: Coordinate,
}

impl Craftable for Spaceship {
	fn get_cost() -> super::crafting_material::CraftingMaterial {
		CraftingMaterial {
			// TODO actual values
			copper: 2000.,
		}
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
	fn into_game_objects(
		self
	) -> SafeAction {
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
}
