use std::ops::{AddAssign, SubAssign};

use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Clone, PartialEq, Copy)]
pub struct Coordinate {
	pub x: f64,
	pub y: f64,
	pub z: f64,
}

impl std::ops::Neg for Coordinate {
    type Output = Self;

    fn neg(self) -> Self::Output {
    	Self {
	        x: -self.x,
	        y: -self.y,
	        z: -self.z,
	    }
    }
}

impl std::ops::Mul<f64> for Coordinate {
    type Output = Coordinate;

    fn mul(self, rhs: f64) -> Self::Output {
    	Self::Output {
	        x: self.x * rhs,
	        y: self.y * rhs,
	        z: self.z * rhs,
	    }
    }
}

impl std::ops::Div<f64> for Coordinate {
    type Output = Coordinate;

    fn div(self, rhs: f64) -> Self::Output {
    	Self::Output {
	        x: self.x / rhs,
	        y: self.y / rhs,
	        z: self.z / rhs,
	    }
    }
}

impl std::ops::Add for Coordinate {
    type Output = Coordinate;

    fn add(self, rhs: Self) -> Self::Output {
    	Self::Output {
	        x: self.x + rhs.x,
	        y: self.y + rhs.y,
	        z: self.z + rhs.z,
	    }
    }
}

impl std::ops::Sub for Coordinate {
    type Output = Coordinate;

    fn sub(self, rhs: Self) -> Self::Output {
    	Self::Output {
	        x: self.x - rhs.x,
	        y: self.y - rhs.y,
	        z: self.z - rhs.z,
	    }
    }
}

impl std::hash::Hash for Coordinate {
    fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
        (self.x as i32).hash(state);
        (self.y as i32).hash(state);
        (self.z as i32).hash(state);
    }
}

impl Eq for Coordinate { }

impl Default for Coordinate {
	fn default() -> Self {
		Coordinate {
			x: 0.0,
			y: 0.0,
			z: 0.0,
		}
	}
}

impl AddAssign for Coordinate {
	fn add_assign(&mut self, rhs: Self) {
		self.add(&rhs);
	}
}

impl SubAssign for Coordinate {
	fn sub_assign(&mut self, rhs: Self) {
		self.sub(&rhs);
	}
}

impl Coordinate {
	pub fn new(x: f64, y: f64, z: f64) -> Self {
		Self { x, y, z }
	}
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

	/// distance from self to other
	pub fn to(&mut self, other: &Self) -> &mut Self {
		self.x = other.x - self.x;
		self.y = other.y - self.y;
		self.z = other.z - self.z;
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

	/// mutates itself by multiplying each component with other
	pub fn mul(&mut self, other: &Self) -> &mut Self {
		self.x *= other.x;
		self.y *= other.y;
		self.z *= other.z;
		self
	}

	pub fn inner_sum(&self) -> f64 {
		self.x + self.y + self.z
	}

	pub fn dot(&self, other: &Self) -> f64 {
        self.x * other.x + self.y * other.y + self.z * other.z
    }
	pub fn perp_dot(&self, other: &Self) -> f64 {
        self.x * other.y - self.y * other.x
    }

	pub fn norm(&self) -> f64 {
		(self.x.powi(2) + self.y.powi(2) + self.z.powi(2)).sqrt()
	}
	pub fn norm_squared(&self) -> f64 {
		(self.x.powi(2) + self.y.powi(2) + self.z.powi(2))
	}

	pub fn normalize(&mut self, value: f64) -> &mut Self {
		if self.norm() == 0. {
			return self;
		}
		let div = 1. / self.norm() * value;
		self.scale(div)
	}
	pub fn cap(&self, value: f64) -> Self {
		let mut n = self.c();
		if self.norm() >= value {
			n.normalize(value);
			return n;
		}
		n
	}

	pub fn chunk(&self, chunk_size: u64) -> Self {
		Self {
		    x: ((self.x as u64 / chunk_size) * chunk_size) as f64,
		    y: ((self.y as u64 / chunk_size) * chunk_size) as f64,
		    z: ((self.z as u64 / chunk_size) * chunk_size) as f64,
		}
	}
}
