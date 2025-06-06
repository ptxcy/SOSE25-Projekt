use super::{
	building_region::BuildingRegion, coordinate::Coordinate,
	dummy::DummyObject, factory::Factory, mine::Mine, spaceship::Spaceship,
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

pub enum SafeAction {
	AddCoordinate {
		coordinate: *mut Coordinate,
		other: Coordinate,
		multiplier: f64,
	},
	SetCoordinate {
		coordinate: *mut Coordinate,
		other: Coordinate,
	},
	SpawnFactory {
		region: *mut BuildingRegion,
		factory: Factory,
	},
	SpawnMine {
		region: *mut BuildingRegion,
		mine: Mine,
	},
	SpawnDummy(DummyObject),
	SpawnSpaceship(Spaceship),
}

unsafe impl Send for SafeAction {}

impl SafeAction {
	pub fn execute(self) {
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
			SafeAction::SpawnDummy(dummy_object) => todo!(),
			SafeAction::SpawnSpaceship(spaceship) => todo!(),
		}
	}
}
