use serde::{Deserialize, Serialize};

use super::factory::Factory;


/// region to craft / placing buildings such as factories on
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct BuildingRegion {
	factories: Vec<Factory>,
}
