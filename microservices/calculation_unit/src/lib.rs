use std::time::{SystemTime, UNIX_EPOCH};

pub mod logger;
pub mod messages {
	pub mod client_message;
	pub mod client_request_execution;
	pub mod server_message;
	pub mod websocket_format;
}

pub mod game {
	pub mod action;
	pub mod building_region;
	pub mod calculation_unit;
	pub mod coordinate;
	pub mod crafting_material;
	pub mod dummy;
	pub mod factory;
	pub mod game_objects;
	pub mod gametraits;
	pub mod network;
	pub mod orbit;
	pub mod planet;
	pub mod planet_util;
	pub mod player;
}

pub fn get_time() -> u128 {
	let now = SystemTime::now();
	let millis = now
		.duration_since(UNIX_EPOCH)
		.expect("time went backwards")
		.as_millis();
	millis
}
