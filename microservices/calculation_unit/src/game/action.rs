use crate::logger::log_with_time;

use super::{
	building_region::BuildingRegion,
	coordinate::Coordinate,
	crafting_material::CraftingMaterial,
	dummy::DummyObject,
	factory::Factory,
	game_objects::GameObjects,
	gametraits::Crafter,
	mine::Mine,
	planet::{OrbitInfoMap, Planet},
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

pub enum SafeAction {
	SetF64(*mut f64, f64),
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
	SetSpaceshipTarget {
		spaceship: *mut Spaceship,
		planet: *const Planet,
		julian_day: f64,
		orbit_info_map: *const OrbitInfoMap,
	},
	ReduceCraftingMaterial {
		crafter: *mut dyn Crafter,
		cost: CraftingMaterial,
	},
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
			SafeAction::SetSpaceshipTarget {
				spaceship,
				planet,
				julian_day,
				orbit_info_map,
			} => {
				let spaceship = unsafe { &mut (*spaceship) };
				let planet = unsafe { &(*planet) };
				let orbit_info_map = unsafe { &(*orbit_info_map) };
				let target = spaceship.fly_to_get_target(
					planet,
					julian_day,
					orbit_info_map,
				);

				log_with_time(format!(
					"Moving Spaceship {} to planet {}, destination is {:?}",
					spaceship.id, planet.name, target
				));

				spaceship.target = target;
			}
		}
	}
}
