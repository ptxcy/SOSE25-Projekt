use serde::{Deserialize, Serialize};

use super::{
	crafting_material::CraftingMaterial,
	gametraits::{Craftable, IsOwned},
};

/// placed somewhere to mine resources / crafting materials
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct Factory {
	owner: String,
	/// resources the factory mines per second
	pub resources: CraftingMaterial,
}

impl IsOwned for Factory {
	fn get_owner<'a>(&'a self) -> &'a String {
		&self.owner
	}

	fn get_owner_mut<'a>(&'a mut self) -> &'a mut String {
		&mut self.owner
	}
}

impl Craftable for Factory {
	fn get_cost() -> CraftingMaterial {
		CraftingMaterial { copper: 400. }
	}
}
