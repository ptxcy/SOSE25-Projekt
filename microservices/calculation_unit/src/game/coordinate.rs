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
	/// clones itself and returns it
	pub fn c(&self) -> Self {
		self.clone()
	}

	/// mutates itself by adding another
	pub fn add(&mut self, other: &Self) -> &mut Self {
		self.x += other.x;
		self.y += other.y;
		self.z += other.z;
		self
	}

	/// mutates itself by setting each value to another
	pub fn set(&mut self, other: &Self) -> &mut Self {
		self.x = other.x;
		self.y = other.y;
		self.z = other.z;
		self
	}

	/// mutates itself by subtracting another
	pub fn sub(&mut self, other: &Self) -> &mut Self {
		self.x -= other.x;
		self.y -= other.y;
		self.z -= other.z;
		self
	}

	/// mutates itself by adding another with delta time scaled
	pub fn addd(&mut self, other: &Self, delta_seconds: f64) -> &mut Self {
		self.x += other.x * delta_seconds;
		self.y += other.y * delta_seconds;
		self.z += other.z * delta_seconds;
		self
	}

	/// mutates itself by subtracting another with delta time scaled
	pub fn subd(&mut self, other: &Self, delta_seconds: f64) -> &mut Self {
		self.x -= other.x * delta_seconds;
		self.y -= other.y * delta_seconds;
		self.z -= other.z * delta_seconds;
		self
	}

	/// mutates itself by scaling all values by a value
	pub fn scale(&mut self, value: f64) -> &mut Self {
		self.x *= value;
		self.y *= value;
		self.z *= value;
		self
	}
}
