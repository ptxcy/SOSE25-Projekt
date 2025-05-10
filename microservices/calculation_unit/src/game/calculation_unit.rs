use crate::{game::coordinate::Coordinate, logger::Loggable, messages::{client_message::{ClientMessage, ClientRequest}, server_message::ServerMessage}};
use tokio::sync::mpsc::*;
use std::{collections::HashMap, sync::Arc, time::Instant};

use super::dummy::DummyObject;

// send message to all client receivers
pub fn broadcast(senders: &Vec<Sender<Arc<ServerMessage>>>, message: &ServerMessage) {
	let shared_message = Arc::new(message.clone()); // Wrap the message in an Arc
    for sender in senders.iter() {
        let message_clone = Arc::clone(&shared_message);
        if let Err(e) = sender.try_send(message_clone) {
            eprintln!("Failed to send message: {}", e);
        }
    }
}

pub async fn start(mut sender_receiver: Receiver<Sender<Arc<ServerMessage>>>, mut client_message_receiver: Receiver<ClientMessage>) {
	// client channels
	let mut server_message_senders = Vec::<Sender<Arc<ServerMessage>>>::new();

	// map id to dummy
	let mut dummys = HashMap::<String, DummyObject>::new();

	// delta time init
	let mut last_time = Instant::now();
	
	// game loop
	loop {
		// get new client channels
		while let Ok(sender) = {println!("calculation_unit: try receive a new sender"); sender_receiver.try_recv()} {
			server_message_senders.push(sender);
		}

		// TODO delta time calculation here
		let now = Instant::now();
        let delta_time = now.duration_since(last_time);
        last_time = now;
        let delta_seconds = delta_time.as_secs_f64();

		// receive client input
		while let Ok(client_message) = client_message_receiver.try_recv() {
			let result = client_message.request_data.execute(&mut dummys, delta_seconds).log();
		}

		// TODO game logic calculation
		println!("do calulation");
	}
}
