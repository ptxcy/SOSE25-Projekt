use std::{
	cell::RefCell,
	fmt,
	sync::Mutex,
	time::{SystemTime, UNIX_EPOCH},
};

static last_log: Mutex<RefCell<Option<String>>> = Mutex::new(RefCell::new(None));
static repeat_log_count: Mutex<RefCell<usize>> = Mutex::new(RefCell::new(0));

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
	let message = message_with_time(message);
	let last_log_message = last_log.lock().unwrap();
	let mut last_log_message_inner = last_log_message.borrow_mut();
	match &*last_log_message_inner {
		Some(m) => {
			let repeat_lock = repeat_log_count.lock().unwrap();
			let mut repeat_count_mut = repeat_lock.borrow_mut();
			if message == *m {
				*repeat_count_mut += 1;
				if (*repeat_count_mut) % 1000 >= 999 {
					print!("+");
				}
				return;
			} else {
				println!("({})", *repeat_count_mut);
				*repeat_count_mut = 0;
			}
		}
		None => {}
	};
	*last_log_message_inner = Some(message.clone());
	println!("\n{}", message);
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
				log_with_time(e)
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
