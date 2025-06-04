use serde::{Deserialize, Serialize};

use super::{coordinate::Coordinate, dummy::DummyObject, game_objects::GameObjects};

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct Player {
	pub username: String,
	pub money: f64,
}

impl Player {
	pub fn new(username: String) -> Self {
		let player = Self {
			username,
			..Default::default()
		};
		player
	}
}

pub trait Buyer {
	fn get_money_mut(&mut self) -> &mut f64;
	fn get_position(&self) -> Coordinate;
	fn get_owner(&self) -> &String;
}

impl Buyer for Player {
	fn get_money_mut(&mut self) -> &mut f64 {
		&mut self.money
	}
	fn get_position(&self) -> Coordinate {
		Coordinate::default()
	}

	fn get_owner(&self) -> &String {
		&self.username
	}
}

pub trait Buyable {
	fn buy<T: Buyer>(buyer: &mut T, game_objects: &mut GameObjects);
}
