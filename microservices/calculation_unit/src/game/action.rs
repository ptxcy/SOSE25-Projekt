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

/// dont move memory so safe to use when known that memory exists
pub enum SafeAction {
	AddCoordinate {
		coordinate: *mut Coordinate,
		other: Coordinate,
		multiplier: f64,
	},
	AddUsize(*mut usize, usize),
	SubUsize(*mut usize, usize),
	ReduceCraftingMaterial {
		crafter: *mut dyn Crafter,
		cost: CraftingMaterial,
	},
	SetCoordinate {
		coordinate: *mut Coordinate,
		other: Coordinate,
	},
	SetDockingAt(*mut Option<usize>, Option<usize>),
	SetF64(*mut f64, f64),
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
	pub fn execute(self, game_objects: *mut GameObjects) {
		let go = unsafe { &mut *game_objects };
		match self {
			SafeAction::AddCoordinate {
				coordinate,
				other,
				multiplier,
			} => unsafe {
				(*coordinate).addd(&other, multiplier);
			},
			SafeAction::SetCoordinate { coordinate, other } => unsafe {
				(*coordinate).set(&other);
			},
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
			SafeAction::SetF64(target, value) => {
				unsafe { *target = value };
			}
			SafeAction::ReduceCraftingMaterial { crafter, cost } => {
				let crafter = unsafe { &mut (*crafter) };
				crafter.get_crafting_material_mut().sub(&cost);
			}
			SafeAction::AddUsize(target, value) => unsafe {
				*target += value;
			},
			SafeAction::SubUsize(target, value) => unsafe {
				*target -= value;
			},
			SafeAction::SetDockingAt(target, value) => unsafe {
				*target = value;
			},
		}
	}
}
