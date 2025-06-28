use calculation_unit::{game::coordinate::Coordinate, logger::log_with_time};
use macroquad::prelude::*;
use serde::{Deserialize, Serialize};
use tokio::sync::mpsc::Sender;

use crate::action::{ActionWrapper, AddValue, AsRaw};

#[derive(Serialize, Deserialize, Debug, Clone, Default)]
pub struct Ball {
    pub position: Coordinate,
    pub velocity: Coordinate,
    pub radius: f64,
}

impl Ball {
    pub fn set_position(mut self, x: f64, y: f64) -> Self {
        self.position.x = x;
        self.position.y = y;
        self
    }
    pub fn random_velocity(mut self) -> Self {
        let x: f64 = rand::gen_range(-1., 1.);
        let y: f64 = rand::gen_range(-1., 1.);
        // let z: f64 = rand::gen_range(-1., 1.);
        let mut c = Coordinate::new(x, y, 0.);
        c.normalize(10.);
        self.velocity = c;
        self
    }
    pub fn update(&self, delta_seconds: f64) -> Vec<ActionWrapper> {
        let mut actions: Vec<ActionWrapper> = vec![];
        // self.position.addd(&self.velocity, delta_seconds);
        let mut add = self.velocity.c();
        add.scale(delta_seconds);
        actions.push(AddValue::new(self.position.raw_mut(), add));
        actions
    }
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct GameObjects {
    pub balls: Vec<Ball>,
}

impl GameObjects {
    pub fn new(count: usize) -> Self {

        let dist = 5.;
        let offset = (count as f64 * dist) / 2.;

        let mut balls = Vec::<Ball>::new();
        for x in 0..count {
            for y in 0..count {
                let ball = Ball::default()
                    .set_position(
                        x as f64 * dist - offset,
                        y as f64 * dist - offset
                    )
                    .random_velocity()
                ;
                balls.push(ball);
            }
        }
        Self {
            balls,
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
