use std::thread::JoinHandle;

use crate::ball::Ball;


pub struct Threadpool {
	pub threads: Vec<JoinHandle<()>>,
}

pub enum UpdateData {
	Balls(Vec<*const Ball>)
}

impl Threadpool {
	pub fn new(count: usize) -> Self {
		let mut threads = Vec::with_capacity(count);
		for _ in 0..count {
			threads.push(std::thread::spawn(move || {
				loop {
					
				}
			}));
		}
		Self { threads }
	}
	pub fn execute(&self, update_data: Vec<UpdateData>) {
		let batchsize = update_data.len() as f64 / self.threads.len() as f64;
		let mut i = 0;
		while batchsize as usize * i < update_data.len() {
			
			i += 1;
		}
	}
}
