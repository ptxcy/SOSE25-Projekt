use std::time::{SystemTime, UNIX_EPOCH};


pub mod logger;
pub mod messages {
    pub mod server_message;
    pub mod websocket_format;
    pub mod client_message;
}

pub mod game {
    pub mod network;
    pub mod coordinate;
    pub mod calculation_unit;
    pub mod dummy;
    pub mod game_objects;
}

pub fn get_time() -> u128 {
	let now = SystemTime::now();
	let millis = now.duration_since(UNIX_EPOCH).expect("time went backwards").as_millis();
	millis
}

