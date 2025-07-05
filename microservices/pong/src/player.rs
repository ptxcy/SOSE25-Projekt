use calculation_unit::game::coordinate::Coordinate;
use serde::{Deserialize, Serialize};


#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Player {
	pub team: bool,
	pub velocity: Coordinate,
	pub position: Coordinate,
	pub relative_lines: Vec<(Coordinate, Coordinate)>,
}

impl Player {
	pub fn new(team: bool) -> Self {
		let rl = if team {
			vec![
				(Coordinate::new(0., 100., 0.), Coordinate::new(0., -100., 0.))
			]
		}
		else {
			vec![
				(Coordinate::new(0., 100., 0.), Coordinate::new(0., -100., 0.))
			]
		};
		Self {
			team,
		    velocity: Default::default(),
		    position: Coordinate::new(if team {-700.} else {700.}, 0., 0.),
		    relative_lines: rl,
		}
	}
}
