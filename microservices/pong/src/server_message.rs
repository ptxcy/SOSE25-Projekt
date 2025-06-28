use calculation_unit::{game::coordinate::Coordinate, logger::log_with_time};
use macroquad::prelude::*;
use serde::{Deserialize, Serialize};
use tokio::sync::mpsc::Sender;

use crate::action::{ActionWrapper, AddValue, AsRaw, SetValue, SubValue};

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Ball {
    pub position: Coordinate,
    pub velocity: Coordinate,
    pub radius: f64,
    pub bounciness: f64,
}

impl Default for Ball {
    fn default() -> Self {
        Self {
            position: Default::default(),
            velocity: Default::default(),
            radius: 2.,
            bounciness: 1.,
        }
    }
}

impl Ball {
    pub fn set_position(mut self, x: f64, y: f64) -> Self {
        self.position.x = x;
        self.position.y = y;
        self
    }
    pub fn set_radius(mut self, radius: f64) -> Self {
        self.radius = radius;
        self
    }
    pub fn random_velocity(mut self, speed: f64) -> Self {
        let x: f64 = rand::gen_range(-1., 1.);
        let y: f64 = rand::gen_range(-1., 1.);
        // let z: f64 = rand::gen_range(-1., 1.);
        let mut c = Coordinate::new(x, y, 0.);
        c.normalize(speed);
        self.velocity = c;
        self
    }
    pub fn update(&self, game_objects: &GameObjects, delta_seconds: f64) -> Vec<ActionWrapper> {
        let mut actions: Vec<ActionWrapper> = vec![];
        // self.position.addd(&self.velocity, delta_seconds);
        let mut add = self.velocity.c();
        add.scale(delta_seconds);
        actions.push(AddValue::new(self.position.raw_mut(), add));

        for ball in game_objects.balls.iter() {
            if std::ptr::eq(self, ball) {
                continue;
            }
            self.collide(ball, &mut actions);
        }
        self.handle_wall_collision(&mut actions);
        
        actions
    }
    pub fn collide(&self, other: &Self, actions: &mut Vec<ActionWrapper>) {
        let mut dist = self.position.c();
        dist.to(&other.position);
        let norm = dist.norm();
        if norm < self.radius + other.radius {
            let mut collision_normal = dist.c();
            collision_normal.normalize(1.);

            let overlap = self.radius + other.radius - norm;
            let mut seperation = collision_normal.c();
            seperation.scale(overlap * 0.5);

            actions.push(SubValue::new(self.position.raw_mut(), seperation));
            // actions.push(AddValue::new(other.position.raw_mut(), seperation));

            let mut relative_velocity = self.velocity.c();
            relative_velocity.to(&other.velocity);

            let mut prod = collision_normal.c();
            prod.mul(&relative_velocity);
            let dvn = prod.inner_sum();

            if dvn > 0. {return;}

            let combined_bounciness = self.bounciness.min(other.bounciness);
            let impulse = dvn * combined_bounciness;

            let mut velocity_change = collision_normal.c();
            velocity_change.scale(impulse);

            actions.push(AddValue::new(self.velocity.raw_mut(), velocity_change));
            // actions.push(SubValue::new(other.velocity.raw_mut(), velocity_change));
        }
    }
    fn handle_wall_collision(&self, actions: &mut Vec<ActionWrapper>) {
        let wall_bound = 500.0;
        let wall_bounciness = self.bounciness;
        
        // Left wall (x = -100)
        if self.position.x - self.radius < -wall_bound {
            let mut new_velocity = self.velocity.c();
            new_velocity.x = -new_velocity.x * wall_bounciness;
            actions.push(SetValue::new(self.velocity.raw_mut(), new_velocity));
            
            // Keep ball in bounds
            let mut new_pos = self.position.c();
            new_pos.x = -wall_bound + self.radius;
            actions.push(SetValue::new(self.position.raw_mut(), new_pos));
        }
        
        // Right wall (x = +100)
        if self.position.x + self.radius > wall_bound {
            let mut new_velocity = self.velocity.c();
            new_velocity.x = -new_velocity.x * wall_bounciness;
            actions.push(SetValue::new(self.velocity.raw_mut(), new_velocity));
            
            let mut new_pos = self.position.c();
            new_pos.x = wall_bound - self.radius;
            actions.push(SetValue::new(self.position.raw_mut(), new_pos));
        }
        
        // Bottom wall (y = -100)
        if self.position.y - self.radius < -wall_bound {
            let mut new_velocity = self.velocity.c();
            new_velocity.y = -new_velocity.y * wall_bounciness;
            actions.push(SetValue::new(self.velocity.raw_mut(), new_velocity));
            
            let mut new_pos = self.position.c();
            new_pos.y = -wall_bound + self.radius;
            actions.push(SetValue::new(self.position.raw_mut(), new_pos));
        }
        
        // Top wall (y = +100)
        if self.position.y + self.radius > wall_bound {
            let mut new_velocity = self.velocity.c();
            new_velocity.y = -new_velocity.y * wall_bounciness;
            actions.push(SetValue::new(self.velocity.raw_mut(), new_velocity));
            
            let mut new_pos = self.position.c();
            new_pos.y = wall_bound - self.radius;
            actions.push(SetValue::new(self.position.raw_mut(), new_pos));
        }
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
                    .random_velocity(30.)
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
