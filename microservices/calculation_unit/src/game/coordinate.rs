use serde::{Deserialize, Serialize};


#[derive(Serialize, Deserialize, Debug)]
pub struct Coordinate {
	x: f64,
	y: f64,
	z: f64,
}
