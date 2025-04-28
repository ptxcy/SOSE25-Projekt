
pub trait Loggable {
	fn log(&self) -> &Self;
}

impl<T, E: ToString> Loggable for Result<T, E> {
    fn log(&self) -> &Self {
    	match self {
	        Ok(_) => todo!(),
	        Err(_) => todo!(),
	    }
    }
}
