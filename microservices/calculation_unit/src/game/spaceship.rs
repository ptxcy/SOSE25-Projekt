use serde::{Deserialize, Serialize};

use super::{coordinate::Coordinate, gametraits::{Craftable, IsOwned, Spawnable}};

#[derive(Serialize, Debug, Clone, Default)]
pub struct Spacestation {
	spaceships: Vec<Spaceship>,
}

impl Spacestation {
	// pub fn depart(&mut self)
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
        todo!()
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
    fn into_game_objects(self, game_objects: &mut super::game_objects::GameObjects) {
    	game_objects.spaceships.push(self);
    }
}

impl Spaceship {
	pub fn arrive(self, spacestation: &mut Spacestation) {
		spacestation.spaceships.push(self);
	}
	pub fn new(owner: &String, speed: f64, id_counter: &mut usize) -> Self {
		let id = *id_counter;
		let ship = Self {
		    id,
		    owner: owner.clone(),
		    speed,
		    ..Default::default()
		};
		*id_counter += 1;
		ship
	}
}
