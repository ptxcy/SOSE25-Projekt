use serde::{Deserialize, Serialize};


#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Coordinate {
	pub x: f64,
	pub y: f64,
	pub z: f64,
}

impl Default for Coordinate {
    fn default() -> Self {
    	Coordinate {
	        x: 0.0,
	        y: 0.0,
	        z: 0.0,
	    }
    }
}

impl Coordinate {

	pub fn c(&self) -> Self {
		self.clone()
	}

	pub fn add(&mut self, other: &Self) -> &mut Self {
		self.x += other.x;
		self.y += other.y;
		self.z += other.z;
		self
	}

	pub fn scale(&mut self, value: f64) -> &mut Self {
		self.x *= value;
		self.y *= value;
		self.z *= value;
		self
	}
}
