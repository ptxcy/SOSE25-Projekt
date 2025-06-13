use std::ops::{AddAssign, SubAssign};

use super::{
	building_region::BuildingRegion, coordinate::Coordinate,
	crafting_material::CraftingMaterial, dummy::DummyObject, factory::Factory,
	game_objects::GameObjects, gametraits::Crafter, mine::Mine,
	spaceship::Spaceship,
};

pub trait AsRaw {
	fn raw_mut(&self) -> *mut Self;
	fn raw(&self) -> *const Self;
}

impl<T> AsRaw for T {
	fn raw_mut(&self) -> *mut Self {
		self as *const Self as *mut Self
	}

	fn raw(&self) -> *const Self {
		self as *const Self
	}
}

pub trait Action: Send {
	fn execute(self: Box<Self>, game_objects: *mut GameObjects);
}

pub type ActionBox = Box<dyn Action>;

pub struct AddValue<T: AddAssign>(*mut T, T);
impl<T: AddAssign + 'static> AddValue<T> {
	pub fn new(target: *mut T, value: T) -> ActionBox {
		Box::new(AddValue(target, value))
	}
}
unsafe impl<T: AddAssign> Send for AddValue<T> {}
impl<T: AddAssign> Action for AddValue<T> {
    fn execute(self: Box<Self>, game_objects: *mut GameObjects) {
    	unsafe {*(self.0) += self.1};
    }
}

pub struct SubValue<T: SubAssign>(*mut T, T);
impl<T: SubAssign + 'static> SubValue<T> {
	pub fn new(target: *mut T, value: T) -> ActionBox {
		Box::new(SubValue(target, value))
	}
}
unsafe impl<T: SubAssign> Send for SubValue<T> {}
impl<T: SubAssign> Action for SubValue<T> {
    fn execute(self: Box<Self>, game_objects: *mut GameObjects) {
    	unsafe {*(self.0) -= self.1};
    }
}

pub struct SetValue<T>(*mut T, T);
impl<T: 'static> SetValue<T> {
	pub fn new(target: *mut T, value: T) -> ActionBox {
		Box::new(SetValue(target, value))
	}
}
unsafe impl<T> Send for SetValue<T> {}
impl<T> Action for SetValue<T> {
    fn execute(self: Box<Self>, game_objects: *mut GameObjects) {
    	unsafe {*(self.0) = self.1};
    }
}

/// dont move memory so safe to use when known that memory exists
pub enum SafeAction {
	// AddCoordinate {
	// 	coordinate: *mut Coordinate,
	// 	other: Coordinate,
	// 	multiplier: f64,
	// },
	// AddUsize(*mut usize, usize),
	// SubUsize(*mut usize, usize),
	// ReduceCraftingMaterial {
	// 	crafter: *mut dyn Crafter,
	// 	cost: CraftingMaterial,
	// },
	// SetCoordinate {
	// 	coordinate: *mut Coordinate,
	// 	other: Coordinate,
	// },
	// SetDockingAt(*mut Option<usize>, Option<usize>),
	// SetF64(*mut f64, f64),
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

impl Action for SafeAction {
	fn execute(self: Box<SafeAction>, game_objects: *mut GameObjects) {
		let go = unsafe { &mut *game_objects };
		match *self {
			// SafeAction::AddCoordinate {
			// 	coordinate,
			// 	other,
			// 	multiplier,
			// } => unsafe {
			// 	(*coordinate).addd(&other, multiplier);
			// },
			// SafeAction::SetCoordinate { coordinate, other } => unsafe {
			// 	(*coordinate).set(&other);
			// },
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
			// SafeAction::SetF64(target, value) => {
			// 	unsafe { *target = value };
			// }
			// SafeAction::ReduceCraftingMaterial { crafter, cost } => {
			// 	let crafter = unsafe { &mut (*crafter) };
			// 	crafter.get_crafting_material_mut().sub(&cost);
			// }
			// SafeAction::AddUsize(target, value) => unsafe {
			// 	*target += value;
			// },
			// SafeAction::SubUsize(target, value) => unsafe {
			// 	*target -= value;
			// },
			// SafeAction::SetDockingAt(target, value) => unsafe {
			// 	*target = value;
			// },
		}
	}
}

/// move memory not safe to use at same time as safeaction
pub enum UnsafeAction {
	DeleteSpaceship(usize),
}

impl Action for UnsafeAction {
	fn execute(self: Box<UnsafeAction>, game_objects: *mut GameObjects) {
		let go = unsafe { &mut *game_objects };
		match *self {
			UnsafeAction::DeleteSpaceship(index) => {
				go.spaceships.remove(&index);
			},
		}
	}
}
