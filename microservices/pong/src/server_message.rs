use std::collections::HashMap;

use calculation_unit::game::coordinate::Coordinate;
use macroquad::prelude::*;
use serde::{Deserialize, Serialize};
use tokio::sync::mpsc::Sender;

use crate::{action::AsRaw, ball::Ball};

pub const CHUNK_SIZE: u64 = 4;

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct GameObjects {
    pub balls: Vec<Ball>,
    pub lines: Vec<(Coordinate, Coordinate)>,

    #[serde(skip)]
    pub chunks: HashMap<Coordinate, Vec<*const Ball>>,
}

unsafe impl Sync for GameObjects {}
unsafe impl Send for GameObjects {}

impl GameObjects {
    pub fn new(count: usize) -> Self {

        let dist = 8.;
        let offset = (count as f64 * dist) / 2.;

        let mut balls = Vec::<Ball>::new();
        for x in 0..count {
            for y in 0..count {
                let ball = Ball::default()
                    .set_position(
                        x as f64 * dist - offset,
                        y as f64 * dist - offset
                    )
                    .random_velocity(10.)
                ;
                balls.push(ball);
            }
        }
        let mut chunks: HashMap<Coordinate, Vec<*const Ball>> = HashMap::new();
        Self::chunky(&balls, &mut chunks);

        let lines = vec![
            (Coordinate::new(-700., -100., 0.), Coordinate::new(-500., 300., 0.))
        ];

        Self {
            balls,
            chunks,
            lines,
        }
    }

    pub fn chunky(balls: &Vec<Ball>, chunks: &mut HashMap<Coordinate, Vec<*const Ball>>) {
        for vec in chunks.values_mut() {
            vec.clear();
        }

        for ball in balls.iter() {
            let chunk = ball.position.chunk(CHUNK_SIZE);
            match chunks.get_mut(&chunk) {
                Some(chunk) => chunk.push(ball.raw()),
                None => {
                    chunks.insert(chunk, vec![ball.raw()]);
                },
            };
        }
    }
}

pub struct ServerMessageSenderChannel {
    pub user_id: String,
    pub sender: Sender<Vec<u8>>,
    // FPS
    pub update_threshold: f64,
    pub tick_counter: f64,
}

impl ServerMessageSenderChannel {
    pub fn new(user_id: String, sender: Sender<Vec<u8>>) -> Self {
        Self {
            user_id,
            sender,
            // default 60 fps value till updated
            update_threshold: 1. / 60.,
            tick_counter: 0.,
        }
    }
}


/// data that is going to be rendered
#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ObjectData {
	pub target_user_id: String,
	pub game_objects: Vec<u8>, // serialized game objects
}

impl ObjectData {
    pub fn prepare_for(username: &String, od: &GameObjects) -> Self {
        Self {
            target_user_id: username.clone(),
            game_objects: rmp_serde::to_vec(od).unwrap(),
        }
    }
}

/// message that is being sent from server to clients via auth proxy
#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ServerMessage {
	pub request_data: ObjectData,
}
