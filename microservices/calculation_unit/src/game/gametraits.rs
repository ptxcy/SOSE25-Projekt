use super::{
	action::SafeAction, coordinate::Coordinate,
	crafting_material::CraftingMaterial,
};

/// has a position or coordinate to spawn something
pub trait Spawner {
	fn spawn_at(&self) -> Coordinate;
}

/// something that is owned by a user / player
pub trait IsOwned {
	fn get_owner<'a>(&'a self) -> &'a String;
	fn get_owner_mut<'a>(&'a mut self) -> &'a mut String;
	fn transfer_ownership<T: IsOwned>(&self, object: &mut T) {
		*object.get_owner_mut() = self.get_owner().clone();
	}
}

/// anything that can buy something
pub trait Buyer: IsOwned {
	fn get_money_mut(&mut self) -> &mut f64;
	fn buy<B: Buyer, O: IsOwned>(
		&mut self,
		object: &mut O,
		seller: &mut B,
		price: f64,
	) {
		*self.get_money_mut() -= price;
		*seller.get_money_mut() += price;
		self.transfer_ownership(object);
	}
}

/// anything that can craft a Craftable
pub trait Crafter: IsOwned + Spawner {
	fn get_crafting_material_mut(&mut self) -> &mut CraftingMaterial;
}

/// soemthing craftable form materials
pub trait Craftable: IsOwned + Spawnable {
	fn get_cost() -> CraftingMaterial;
}

pub trait Spawnable {
	fn into_game_objects(self) -> SafeAction;
}

pub trait HasPosition {
	fn get_position(&self) -> Coordinate;
}

pub trait HasRelativePosition {
    type Parent: HasPosition;
    fn get_parent(&self) -> &Self::Parent;
    fn get_relative_position(&self) -> Coordinate;
}

impl<C: HasRelativePosition> HasPosition for C {
    fn get_position(&self) -> Coordinate {
        self.get_parent().get_position()
    }
}
