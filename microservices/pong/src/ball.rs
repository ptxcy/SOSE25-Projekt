use calculation_unit::game::coordinate::Coordinate;
use macroquad::prelude::*;
use serde::{Deserialize, Serialize};

use crate::{action::{ActionWrapper, AddValue, AsRaw, SetValue, SubValue}, server_message::{GameObjects, CHUNK_SIZE}};

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

        let my_chunk = self.position.chunk(CHUNK_SIZE);

        let chunks: Vec<Coordinate> = vec![
            my_chunk,
            // my_chunk + Coordinate::new(CHUNK_SIZE as f64, 0., 0.),
        ];

        for chunk in chunks {
            let balls = game_objects.balls.get(&chunk).unwrap();
            for ball in balls.iter() {
                if std::ptr::eq(self, ball) {
                    continue;
                }
                self.collide(ball, &mut actions);
            }
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

