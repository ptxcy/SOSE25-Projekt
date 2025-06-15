use std::ops::{AddAssign, SubAssign};

use crate::logger::log_with_time;

use super::{
	building_region::BuildingRegion, dummy::DummyObject, factory::Factory,
	game_objects::GameObjects, mine::Mine, spaceship::Spaceship,
};

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
fn allocate<T>(data: T) -> *mut () {
	log_with_time("allocating memory");
	Box::into_raw(Box::new(data)) as *mut ()
}
fn drop_data<T>(ptr: *mut ()) {
    unsafe {
        let _ = Box::from_raw(ptr as *mut T);
    }
}

pub trait Action {
	fn execute(data: *mut (), game_objects: *mut GameObjects);
}

type ExecuteFn = fn(*mut (), *mut GameObjects);
type DropFn = fn(*mut ());

pub struct ActionWrapper {
	data: *mut (),
	execute: ExecuteFn,
	drop: DropFn,
}

unsafe impl Send for ActionWrapper {}

impl ActionWrapper {
	pub fn execute(self, go: *mut GameObjects) {
		(self.execute)(self.data, go);
		log_with_time("freeing memory");
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

/// dont move memory so safe to use when known that memory exists
#[derive(Clone)]
pub enum SafeAction {
	SpawnDummy(DummyObject),
	SpawnFactory {
		region: *mut BuildingRegion,
		factory: Factory,
	},
	SpawnMine {
		region: *mut BuildingRegion,
		mine: Mine,
	},
	SpawnSpaceship(Spaceship),
}

unsafe impl Send for SafeAction {}

impl SafeAction {
	pub fn new(sa: SafeAction) -> ActionWrapper {
		ActionWrapper {
			data: allocate(sa),
			execute: Self::execute,
			drop: drop_data::<Self>,
		}
	}
}

impl Action for SafeAction {
	fn execute(data: *mut (), game_objects: *mut GameObjects) {
		let v = data as *mut SafeAction;
		let sa = unsafe { (*v).clone() };
		let go = unsafe { &mut *game_objects };
		match sa {
			SafeAction::SpawnFactory { region, factory } => {
				let region = unsafe { &mut *region };
				region.factories.push(factory);
			}
			SafeAction::SpawnMine { region, mine } => {
				let region = unsafe { &mut *region };
				region.mines.push(mine);
			}
			SafeAction::SpawnDummy(dummy_object) => {
				go.dummies.insert(dummy_object.id, dummy_object);
			}
			SafeAction::SpawnSpaceship(spaceship) => {
				go.spaceships.insert(spaceship.id, spaceship);
			}
		}
	}
}

/// move memory not safe to use at same time as safeaction
#[derive(Clone)]
pub enum UnsafeAction {
	DeleteSpaceship(usize),
}

impl UnsafeAction {
	pub fn new(usa: UnsafeAction) -> ActionWrapper {
		ActionWrapper {
			data: allocate(usa),
			execute: Self::execute,
			drop: drop_data::<Self>,
		}
	}
}

impl Action for UnsafeAction {
	fn execute(data: *mut (), game_objects: *mut GameObjects) {
		let v = data as *mut UnsafeAction;
		let usa = unsafe { (*v).clone() };
		let go = unsafe { &mut *game_objects };
		match usa {
			UnsafeAction::DeleteSpaceship(index) => {
				go.spaceships.remove(&index);
			}
		}
	}
}
