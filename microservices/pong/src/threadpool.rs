use std::thread::JoinHandle;
use crate::{action::{self, ActionWrapper, AsRaw}, ball::Ball, server_message::GameObjects};

pub struct Threadpool {
    pub threads: Vec<(std::sync::mpsc::Sender<UpdateData>, JoinHandle<()>)>,
}

pub enum UpdateData {
    Balls {
        balls: *const Vec<*const Ball>,
        chunk: (usize, usize),
    },
    GameObjects(*const GameObjects),
    DeltaTime(f64),
    ActionSender(std::sync::mpsc::Sender<Vec<ActionWrapper>>),
}

unsafe impl Send for UpdateData {}
unsafe impl Sync for UpdateData {}

impl Threadpool {
    pub fn new(count: usize) -> Self {
        let mut threads = Vec::with_capacity(count);
        for _ in 0..count {
            let (sender, receiver) = std::sync::mpsc::channel::<UpdateData>();
            threads.push((
                sender,
                std::thread::spawn(move || {
                    let mut go: Option<*const GameObjects> = None;
                    let mut dt: Option<f64> = None;
                    let mut action_sender: Option<std::sync::mpsc::Sender<Vec<ActionWrapper>>> =
                        None;
                    while let Ok(message) = receiver.recv() {
                        let mut actions = Vec::<ActionWrapper>::with_capacity(100);
                        match message {
                            UpdateData::Balls { balls, chunk } => match (go, dt) {
                                (Some(go), Some(dt)) => {
                                    let game_objects = unsafe { &*go };
                                    let balls = unsafe { &*balls };
                                    for ball in balls.iter() {
                                        let ball = unsafe { &**ball };
                                        ball.update(chunk, game_objects, dt, &mut actions);
                                    }
                                }
                                _ => {}
                            },
                            UpdateData::GameObjects(new_go) => {
                                go = Some(new_go);
                            },
                            UpdateData::DeltaTime(new_dt) => {
                                dt = Some(new_dt);
                            },
                            UpdateData::ActionSender(sender) => {
                                action_sender = Some(sender);
                            },
                        }
                        if let Some(sender) = &mut action_sender {
                            match sender.send(actions) {
                                Ok(_) => {},
                                Err(_) => {
                                    action_sender = None;
                                },
                            };
                        }
                    }
                }),
            ));
        }
        Self { threads }
    }
    pub fn execute(
        &self,
        update_data: Vec<UpdateData>,
        game_objects: &GameObjects,
        delta_seconds: f64,
    ) -> Vec<action::ActionWrapper> {
        let (action_sender, action_receiver) = std::sync::mpsc::channel::<Vec<ActionWrapper>>();
        let expected_packages = update_data.len();

        // provide info
        for thread in self.threads.iter() {
            thread.0.send(UpdateData::DeltaTime(delta_seconds)).unwrap();
            thread.0.send(UpdateData::GameObjects(game_objects.raw())).unwrap();
            thread.0.send(UpdateData::ActionSender(action_sender.clone())).unwrap();
        }

        // provide data
        let mut thread_i = 0;
        for ud in update_data {
            self.threads[thread_i].0.send(ud).unwrap();
            thread_i += 1;
            if thread_i >= self.threads.len() {
                thread_i = 0;
            }
        }

        // get actions
        let mut actions: Vec<ActionWrapper> = Vec::with_capacity(game_objects.balls.len() * 2);
        let mut packages_received = 0;
        while let Ok(p) = action_receiver.recv() {
            packages_received += 1;
            for action in p {
                actions.push(action);
            }
            if packages_received >= expected_packages {
                // done
                break;
            }
        }

        actions
    }
}
