use serde::{Deserialize, Serialize};

use super::{
	coordinate::Coordinate,
	game_objects::GameObjects,
	player::{Buyable, Buyer},
};

/// dummy object for square rendering
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct DummyObject {
	pub owner: String, // username
	pub id: usize,
	pub name: String,
	pub position: Coordinate,
	pub velocity: Coordinate,
}

impl DummyObject {
	pub fn new(owner: &String, name: &String, id_counter: &mut usize) -> Self {
		let dummy = Self {
			owner: owner.clone(),
			name: name.clone(),
			id: *id_counter,
			..Default::default()
		};
		*id_counter += 1;
		dummy
	}
}

impl Buyable for DummyObject {
	fn buy<T: Buyer>(buyer: &mut T, game_objects: &mut GameObjects) {
		(*buyer.get_money_mut()) -= 10.;
		let dummy = DummyObject {
			owner: buyer.get_owner().to_owned(),
			..Default::default()
		};
		game_objects.dummies.insert(dummy.id, dummy);
	}
}
