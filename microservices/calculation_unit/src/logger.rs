use std::time::{SystemTime, UNIX_EPOCH};


pub trait Loggable {
	fn log(self) -> Self;
}

impl<T, E: ToString> Loggable for Result<T, E> {
	fn log(self) -> Self {
		match &self {
			Ok(_) => {},
			Err(e) => {
				let now = SystemTime::now().duration_since(UNIX_EPOCH).expect("Time went backwards.");
				let total_seconds = now.as_secs();
				let hours = (total_seconds / 3600) % 24;
				let minutes = (total_seconds / 60) % 60;
				let seconds = total_seconds % 60;
				println!("{{{}:{}:{}}} Error: {}", hours, minutes, seconds, e.to_string());
			},
		}
		self
	}
}
