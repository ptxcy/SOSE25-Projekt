use std::time::{SystemTime, UNIX_EPOCH};

pub mod logger;
pub mod messages {
	pub mod client_message;
	pub mod server_message;
	pub mod websocket_format;
}

pub mod game {
	pub mod action;
	pub mod calculation_unit;
	pub mod coordinate;
	pub mod dummy;
	pub mod game_objects;
	pub mod network;
}

pub fn get_time() -> u128 {
	let now = SystemTime::now();
	let millis = now
		.duration_since(UNIX_EPOCH)
		.expect("time went backwards")
		.as_millis();
	millis
}
