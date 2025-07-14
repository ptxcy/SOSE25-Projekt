use std::ops::{AddAssign, SubAssign};

use crate::server_message::GameObjects;

pub trait AsRaw {
	fn raw(&self) -> *const Self;
	fn raw_mut(&self) -> *mut Self;
	fn void(&self) -> *mut ();
}

impl<T> AsRaw for T {
	fn raw(&self) -> *const Self {
		self as *const Self
	}
	fn raw_mut(&self) -> *mut Self {
		self.raw() as *mut Self
	}
	fn void(&self) -> *mut () {
		self.raw_mut() as *mut ()
	}
}

/// free it yourself
pub fn allocate<T>(data: T) -> *mut () {
	// log_with_time("allocating memory");
	Box::into_raw(Box::new(data)) as *mut ()
}
pub fn drop_data<T>(ptr: *mut ()) {
    unsafe {
        let _ = Box::from_raw(ptr as *mut T);
    }
}

pub trait Action {
	fn execute(data: *mut (), game_objects: *mut GameObjects);
}

pub type ExecuteFn = fn(*mut (), *mut GameObjects);
pub type DropFn = fn(*mut ());

pub struct ActionWrapper {
	data: *mut (),
	execute: ExecuteFn,
	drop: DropFn,
}

unsafe impl Send for ActionWrapper {}

impl ActionWrapper {
	pub fn execute(self, go: *mut GameObjects) {
		(self.execute)(self.data, go);
		// log_with_time("freeing memory");
		(self.drop)(self.data);
	}
}

pub struct AddValue<T: AddAssign + Copy>(*mut T, T);
impl<T: AddAssign + Copy> AddValue<T> {
	pub fn new(target: *mut T, value: T) -> ActionWrapper {
		ActionWrapper {
			data: allocate(AddValue(target, value)),
			execute: Self::execute,
			drop: drop_data::<Self>,
		}
	}
}
impl<T: AddAssign + Copy> Action for AddValue<T> {
	fn execute(data: *mut (), _game_objects: *mut GameObjects) {
		let data = data as *mut Self;
		unsafe {
			*(*data).0 += (*data).1;
		}
	}
}

pub struct SubValue<T: SubAssign + Copy>(*mut T, T);
impl<T: SubAssign + Copy> SubValue<T> {
	pub fn new(target: *mut T, value: T) -> ActionWrapper {
		ActionWrapper {
			data: allocate(SubValue(target, value)),
			execute: Self::execute,
			drop: drop_data::<Self>,
		}
	}
}
impl<T: SubAssign + Copy> Action for SubValue<T> {
	fn execute(data: *mut (), _game_objects: *mut GameObjects) {
		let data = data as *mut SubValue<T>;
		unsafe {
			*(*data).0 -= (*data).1;
		}
	}
}

pub struct SetValue<T: Copy>(*mut T, T);
impl<T: Copy> SetValue<T> {
	pub fn new(target: *mut T, value: T) -> ActionWrapper {
		ActionWrapper {
			data: allocate(SetValue(target, value)),
			execute: Self::execute,
			drop: drop_data::<Self>,
		}
	}
}
impl<T: Copy> Action for SetValue<T> {
	fn execute(data: *mut (), _game_objects: *mut GameObjects) {
		let data = data as *mut SetValue<T>;
		unsafe {
			*(*data).0 = (*data).1;
		}
	}
}

