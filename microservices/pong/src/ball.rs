use calculation_unit::game::{coordinate::Coordinate};
use macroquad::prelude::*;
use serde::{Deserialize, Serialize};

use crate::{action::{ActionWrapper, AddValue, AsRaw, SetValue, SubValue}, line::Line, server_message::{GameObjects}};

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
    pub fn fix_velocity(&mut self) {
        self.velocity = self.velocity.cap(150.);
    }
    pub fn update(&self, my_chunk: (usize, usize), game_objects: &GameObjects, delta_seconds: f64, actions: &mut Vec<ActionWrapper>) {
        let mut add = self.velocity.c();
        add.scale(delta_seconds);
        actions.push(AddValue::new(self.position.raw_mut(), add));

        let chunks: Vec<(i64, i64)> = vec![
            (0, -1),
            (0, 0),
            (0, 1),
            (-1, -1),
            (-1, 0),
            (-1, 1),
            (1, -1),
            (1, 0),
            (1, 1),
        ];

        for chunk in chunks.iter() {
            let balls = game_objects.chunks.get((my_chunk.0 as i64) + chunk.0, (my_chunk.1 as i64) + chunk.1);
            if let Some(balls) = balls {
                for ball in balls.iter() {
                    if self as *const Ball >= ball.0 {
                        continue;
                    }
                    self.collide_ball(unsafe {&(*ball.0)}, actions);
                }
            }
        }

        for line in game_objects.lines.iter() {
            self.collide_line(line.clone(), Coordinate::default(), actions);
        }
        for player in game_objects.players.iter() {
            for line in player.relative_lines.iter() {
                self.collide_line(Line::new(line.0 + player.position, line.1 + player.position), player.velocity, actions);
            }
        }

        self.handle_wall_collision(game_objects, actions);
    }
    pub fn closest_point_on_segment(&self, a: Coordinate, b: Coordinate) -> Coordinate {
        let ab = b - a;
        let t = ((self.position - a).dot(&ab) / ab.norm_squared()).clamp(0., 1.);
        let mut abc = ab.c();
        abc.scale(t);
        a + abc
    }
    pub fn collide_line(&self, line: Line, v: Coordinate, actions: &mut Vec<ActionWrapper>) {
        let relative_velocity = self.velocity - v;
        let closest = self.closest_point_on_segment(line.a, line.b);
        let to_closest = self.position - closest;
        let dist_sq = to_closest.norm_squared();

        if dist_sq >= self.radius.powi(2) {
            return;
        }

        let dist = dist_sq.sqrt();
        let mut normal = to_closest.c();
        normal.normalize(1.);

        // Only collide if moving towards the line
        if relative_velocity.dot(&normal) >= 0. {
            return;
        }
    
        // Position correction - move ball out of line
        let penetration = self.radius - dist;
        let corrected_pos = self.position + normal * penetration;
        actions.push(SetValue::new(self.position.raw_mut(), corrected_pos));
    
        // Velocity reflection
        let reflected = relative_velocity - normal * 2. * relative_velocity.dot(&normal);
        actions.push(SetValue::new(self.velocity.raw_mut(), reflected + v));
    }
    pub fn collide_ball(&self, other: &Self, actions: &mut Vec<ActionWrapper>) {
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
            actions.push(AddValue::new(other.position.raw_mut(), seperation));

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
            actions.push(SubValue::new(other.velocity.raw_mut(), velocity_change));
        }
    }
    fn handle_wall_collision(&self, game_objects: &GameObjects, actions: &mut Vec<ActionWrapper>) {
        let wall_bound_y = 1080. / 2.2;
        let wall_bound_x = 1920. / 2.2;
        let wall_bounciness = self.bounciness;
        
        // Left wall (x = -100)
        if self.position.x - self.radius < -wall_bound_x {
            let mut new_velocity = self.velocity.c();
            new_velocity.x = -new_velocity.x * wall_bounciness;
            actions.push(SetValue::new(self.velocity.raw_mut(), new_velocity));
            
            // Keep ball in bounds
            let mut new_pos = self.position.c();
            new_pos.x = -wall_bound_x + self.radius;
            actions.push(SetValue::new(self.position.raw_mut(), new_pos));

            actions.push(AddValue::new(game_objects.score.player1.raw_mut(), 1));
        }
        
        // Right wall (x = +100)
        if self.position.x + self.radius > wall_bound_x {
            let mut new_velocity = self.velocity.c();
            new_velocity.x = -new_velocity.x * wall_bounciness;
            actions.push(SetValue::new(self.velocity.raw_mut(), new_velocity));
            
            let mut new_pos = self.position.c();
            new_pos.x = wall_bound_x - self.radius;
            actions.push(SetValue::new(self.position.raw_mut(), new_pos));

            actions.push(AddValue::new(game_objects.score.player2.raw_mut(), 1));
        }
        
        // Bottom wall (y = -100)
        if self.position.y - self.radius < -wall_bound_y {
            let mut new_velocity = self.velocity.c();
            new_velocity.y = -new_velocity.y * wall_bounciness;
            actions.push(SetValue::new(self.velocity.raw_mut(), new_velocity));
            
            let mut new_pos = self.position.c();
            new_pos.y = -wall_bound_y + self.radius;
            actions.push(SetValue::new(self.position.raw_mut(), new_pos));
        }
        
        // Top wall (y = +100)
        if self.position.y + self.radius > wall_bound_y {
            let mut new_velocity = self.velocity.c();
            new_velocity.y = -new_velocity.y * wall_bounciness;
            actions.push(SetValue::new(self.velocity.raw_mut(), new_velocity));
            
            let mut new_pos = self.position.c();
            new_pos.y = wall_bound_y - self.radius;
            actions.push(SetValue::new(self.position.raw_mut(), new_pos));
        }
    }
}

