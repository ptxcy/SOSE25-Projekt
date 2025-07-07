use calculation_unit::game::coordinate::Coordinate;
use serde::{Deserialize, Serialize};


#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Line {
	pub a: Coordinate,
	pub b: Coordinate,
}

impl Line {
	pub fn new(a: Coordinate, b: Coordinate) -> Self {
		Self {
		    a,
		    b,
		}
	}
}
