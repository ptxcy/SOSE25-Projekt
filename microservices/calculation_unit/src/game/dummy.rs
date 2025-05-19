use serde::{Deserialize, Serialize};

use super::coordinate::Coordinate;

// dummy object for square rendering
#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct DummyObject {
	pub id: String,
	pub position: Coordinate,
	pub velocity: Coordinate,
}
