pub struct IdCounter {
	next: usize,
}
impl IdCounter {
	pub fn new() -> Self {
		Self { next: 0 }
	}
	pub fn assign(&mut self) -> usize {
		let id = self.next;
		self.next += 1;
		id
	}
}
