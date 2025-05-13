use super::coordinate::Coordinate;

pub trait AsRaw {
	fn raw_mut(&self) -> *mut Self;
	fn raw(&self) -> *const Self;
}

impl<T> AsRaw for T {
    fn raw_mut(&self) -> *mut Self {
    	self as *const Self as *mut Self
    }

    fn raw(&self) -> *const Self {
    	self as *const Self
    }
}

pub enum SafeAction {
	AddCoordinate {
		coordinate: *mut Coordinate,
		other: *const Coordinate,
		multiplier: f64,
	}
}

impl SafeAction {
	pub fn execute(&self) {
		match self {
		    SafeAction::AddCoordinate { coordinate, other, multiplier } => {
		    	unsafe {
			    	(**coordinate).addd(&**other, *multiplier);
		    	}
		    },
		}
	}
}
