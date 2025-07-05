use std::collections::HashMap;

use calculation_unit::game::coordinate::Coordinate;
use macroquad::prelude::*;
use serde::{Deserialize, Serialize};
use tokio::sync::mpsc::Sender;

use crate::{action::AsRaw, ball::Ball, player::Player};

pub const CHUNK_SIZE: u64 = 10;

#[derive(Debug, Clone, Default)]
pub struct Chunks {
    size: (usize, usize),
    pub array: Vec<Vec<*const Ball>>,
}

impl Chunks {
    pub fn new(x: usize, y: usize) -> Self {
        let mut array = Vec::with_capacity(x * y);
        for _ in 0..(x*y) {
            array.push(Vec::with_capacity(50));
        }
        Self {
            size: (x, y),
            array,
        }
    }
    pub fn get(&self, x: i64, y: i64) -> Option<&Vec<*const Ball>> {
        if x < 0 || x as usize >= self.size.0 {
            return None;
        }
        self.array.get((x as usize) + (y as usize) * self.size.1)
    }
    pub fn get_chunk(&self, index: usize) -> (usize, usize) {
        let x = index % self.size.0;
        let y = index / self.size.1;
        (x, y)
    }
    pub fn estimate_chunk(&self, x: f64, y: f64) -> usize {
        let xi = (x / CHUNK_SIZE as f64 + self.size.0 as f64 / 2.) as usize;
        let yi = (y / CHUNK_SIZE as f64 + self.size.1 as f64 / 2.) as usize;
        let index = xi + yi * self.size.0;
        index
    }
    pub fn update(&mut self, balls: &Vec<Ball>) {
        for balls_chunk in &mut self.array {
            balls_chunk.clear();
        }
        for ball in balls.iter() {
            let chunk = self.estimate_chunk(ball.position.x, ball.position.y);
            self.array[chunk].push(ball.raw_mut());
        }
    }
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct GameObjects {
    pub balls: Vec<Ball>,
    pub lines: Vec<(Coordinate, Coordinate)>,
    pub players: HashMap<String, Player>,
    pub score: (u16, u16),

    #[serde(skip)]
    pub chunks: Chunks,
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
                    .random_velocity(20.)
                ;
                balls.push(ball);
            }
        }
        let mut chunks = Chunks::new(2000 / CHUNK_SIZE as usize, 1200 / CHUNK_SIZE as usize);
        chunks.update(&balls);

        let lines = vec![
            (Coordinate::new(-700., -100., 0.), Coordinate::new(-500., 300., 0.))
        ];

        Self {
            balls,
            chunks,
            lines,
            players: HashMap::with_capacity(2),
            score: (0, 0),
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
