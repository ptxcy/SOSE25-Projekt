use super::{coordinate::Coordinate, game_objects::GameObjects, player::CraftingMaterial};

/// anything that can buy something
pub trait Buyer {
	fn get_money_mut(&mut self) -> &mut f64;
	fn get_position(&self) -> Coordinate;
	fn get_owner(&self) -> &String;
	// sell to another buyer
	fn sell<B: Buyer, O: IsOwned>(&mut self, object: &mut O, buyer: &mut B, price: f64) {
		*self.get_money_mut() += price;
		*buyer.get_money_mut() -= price;
		*object.get_owner_mut() = buyer.get_owner().clone();
	}
}

/// anything that can craft a Craftable
pub trait Crafter: IsOwned {
	fn get_crafting_material_mut(&mut self) -> &mut CraftingMaterial;
	fn get_position(&self) -> Coordinate;
}

/// something that is owned by a user / player
pub trait IsOwned {
	fn get_owner<'a>(&'a self) -> &'a String;
	fn get_owner_mut<'a>(&'a mut self) -> &'a mut String;
}

/// buying something fresh from the system
pub trait Buyable {
	fn buy<T: Buyer>(buyer: &mut T, game_objects: &mut GameObjects);
}

/// soemthing craftable form materials
pub trait Craftable {
	fn craft<T: Crafter>(crafter: &mut T, game_objects: &mut GameObjects);
}
