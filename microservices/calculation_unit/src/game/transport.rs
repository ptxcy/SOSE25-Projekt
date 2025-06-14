use std::sync::{Arc, Mutex};

use super::{action::{ActionWrapper, AddValue, SetValue}, crafting_material::CraftingMaterial};

#[derive(Debug, Clone)]
pub struct ValidationTuple<T>(T, Arc<Mutex<bool>>);

impl<T> ValidationTuple<T> {
	pub fn is_valid(&self) -> bool {
		self.1.lock().unwrap().clone()
	}
}

#[derive(Debug, Clone)]
pub struct CraftingMaterialTransport {
	source: ValidationTuple<*mut CraftingMaterial>,
	destinatons: Vec<ValidationTuple<*mut CraftingMaterial>>,
}

impl CraftingMaterialTransport {
	pub fn is_valid(&self) -> bool {
		self.source.is_valid()
	}

	// make sure to check if valid and retain before use
	pub fn transport(&mut self) -> Vec<ActionWrapper> {
		let mut actions = Vec::<ActionWrapper>::new();
		if self.destinatons.len() == 0 {
			return actions;
		}
		let mut portion = unsafe {(*self.source.0).clone()};

		let factor = 1. / self.destinatons.len() as f64;
		portion.scale(factor);

		self.destinatons.retain(|destination| destination.is_valid());
		for destination in self.destinatons.iter() {
			actions.push(AddValue::new(destination.0, portion.clone()));
		}
		actions.push(SetValue::new(self.source.0, CraftingMaterial::default()));
		actions
	}
}
