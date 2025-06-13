use std::ops::{AddAssign, SubAssign};

use crate::logger::log_with_time;

use super::{
	building_region::BuildingRegion, dummy::DummyObject, factory::Factory,
	game_objects::GameObjects, mine::Mine,
	spaceship::Spaceship,
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
fn malloc<T>(data: T) -> *mut () {
	// log_with_time("allocating memory");
	Box::into_raw(Box::new(data)) as *mut ()
}

type ActionFn = fn(*mut (), *mut GameObjects);

pub struct ActionWrapper {
    data: *mut (),
    func: ActionFn,
}

unsafe impl Send for ActionWrapper {}

impl ActionWrapper {
    pub fn execute(self, go: *mut GameObjects) {
        (self.func)(self.data, go);
        unsafe {
        	// free memory
			// log_with_time("freeing memory");
        	let _ = Box::from_raw(self.data);
        }
    }
}

pub struct AddValue<T: AddAssign + Copy>(*mut T, T);
impl<T: AddAssign + Copy> AddValue<T> {
	fn get_execute() -> ActionFn {
        fn execute<T: AddAssign + Copy>(data: *mut (), _game_objects: *mut GameObjects) {
            let data = data as *mut AddValue<T>;
            unsafe {
                *(*data).0 += (*data).1;
            }
        }
        execute::<T>
    }
    pub fn new(target: *mut T, value: T) -> ActionWrapper {
		ActionWrapper {
			data: malloc(AddValue(target, value)),
			func: Self::get_execute()
		}
	}
}

pub struct SubValue<T: SubAssign + Copy>(*mut T, T);
impl<T: SubAssign + Copy> SubValue<T> {
	fn get_execute() -> ActionFn {
        fn execute<T: SubAssign + Copy>(data: *mut (), _game_objects: *mut GameObjects) {
            let data = data as *mut SubValue<T>;
            unsafe {
                *(*data).0 -= (*data).1;
            }
        }
        execute::<T>
    }
    pub fn new(target: *mut T, value: T) -> ActionWrapper {
		ActionWrapper {
			data: malloc(SubValue(target, value)),
			func: Self::get_execute()
		}
	}
}

pub struct SetValue<T: Copy>(*mut T, T);
impl<T: Copy> SetValue<T> {
	fn get_execute() -> ActionFn {
        fn execute<T: Copy>(data: *mut (), _game_objects: *mut GameObjects) {
            let data = data as *mut SetValue<T>;
            unsafe {
                *(*data).0 = (*data).1;
            }
        }
        execute::<T>
    }
    pub fn new(target: *mut T, value: T) -> ActionWrapper {
		ActionWrapper {
			data: malloc(SetValue(target, value)),
			func: Self::get_execute()
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
	fn execute(self, game_objects: *mut GameObjects) {
		let go = unsafe { &mut *game_objects };
		match self {
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
	fn get_execute() -> ActionFn {
		fn execute(data: *mut (), game_objects: *mut GameObjects) {
			let v = data as *mut SafeAction;
			let sa = unsafe {(*v).clone()};
			sa.execute(game_objects);
		}
		execute
	}
    pub fn new(sa: SafeAction) -> ActionWrapper {
		ActionWrapper {
			data: malloc(sa),
			func: Self::get_execute()
		}
	}
}

/// move memory not safe to use at same time as safeaction
#[derive(Clone)]
pub enum UnsafeAction {
	DeleteSpaceship(usize),
}

impl UnsafeAction {
	fn execute(self, game_objects: *mut GameObjects) {
		let go = unsafe { &mut *game_objects };
		match self {
			UnsafeAction::DeleteSpaceship(index) => {
				go.spaceships.remove(&index);
			},
		}
	}
	fn get_execute() -> ActionFn {
		fn execute(data: *mut (), game_objects: *mut GameObjects) {
			let v = data as *mut UnsafeAction;
			let usa = unsafe {(*v).clone()};
			usa.execute(game_objects);
		}
		execute
	}
    pub fn new(usa: UnsafeAction) -> ActionWrapper {
		ActionWrapper {
			data: malloc(usa),
			func: Self::get_execute()
		}
	}
}
