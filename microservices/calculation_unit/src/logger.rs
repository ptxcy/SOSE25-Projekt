use std::{
	fmt,
	time::{SystemTime, UNIX_EPOCH},
};

fn message_with_time<E: fmt::Debug>(message: E) -> String {
	let now = SystemTime::now()
		.duration_since(UNIX_EPOCH)
		.expect("Time went backwards.");
	let total_seconds = now.as_secs();
	let hours = (total_seconds / 3600 + 2) % 24;
	let minutes = (total_seconds / 60) % 60;
	let seconds = total_seconds % 60;
	format!("{{{}:{}:{}}} - {:?}", hours, minutes, seconds, message)
}

pub fn log_with_time<E: fmt::Debug>(message: E) {
	println!("{}\n", message_with_time(message));
}

pub trait Loggable {
	fn log(self) -> Self;
	fn logm<M: fmt::Debug>(self, message: M) -> Self;
}

impl<T, E: fmt::Debug> Loggable for Result<T, E> {
	fn log(self) -> Self {
		match &self {
			Ok(_) => {}
			Err(e) => {
				println!("{}", message_with_time(e));
			}
		}
		self
	}

	fn logm<M: fmt::Debug>(self, message: M) -> Self {
		match &self {
			Ok(_) => {}
			Err(e) => {
				println!("{}: {:?}", message_with_time(e), message);
			}
		}
		self
	}
}
