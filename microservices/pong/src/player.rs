use calculation_unit::{game::coordinate::Coordinate, logger::log_with_time};
use serde::{Deserialize, Serialize};

use crate::{action::{ActionWrapper, AddValue, AsRaw}, server_message::GameObjects};


#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Player {
	pub speed: f64,
	pub team: bool,
	pub velocity: Coordinate,
	pub position: Coordinate,
	pub relative_lines: Vec<(Coordinate, Coordinate)>,
}

impl Player {
	pub fn new(team: bool) -> Self {
		let rl = if team {
			vec![
				(Coordinate::new(0., 100., 0.), Coordinate::new(0., -100., 0.)),
				(Coordinate::new(0., 100., 0.), Coordinate::new(50., 0., 0.)),
				(Coordinate::new(0., -100., 0.), Coordinate::new(50., 0., 0.)),
			]
		}
		else {
			vec![
				(Coordinate::new(0., 100., 0.), Coordinate::new(0., -100., 0.)),
				(Coordinate::new(0., 100., 0.), Coordinate::new(-50., 0., 0.)),
				(Coordinate::new(0., -100., 0.), Coordinate::new(-50., 0., 0.)),
			]
		};
		Self {
			team,
		    velocity: Default::default(),
		    position: Coordinate::new(if team {-700.} else {700.}, 0., 0.),
		    relative_lines: rl,
			speed: 200.,
		}
	}
	pub fn update(&self, _go: &GameObjects, delta_seconds: f64, actions: &mut Vec<ActionWrapper>) {
        let mut add = self.velocity.c();
        add.scale(delta_seconds);
        actions.push(AddValue::new(self.position.raw_mut(), add));
	}
}
