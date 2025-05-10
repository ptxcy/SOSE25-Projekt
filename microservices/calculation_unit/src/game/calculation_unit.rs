use crate::messages::{client_message::ClientMessage, server_message::ServerMessage};
use tokio::sync::mpsc::*;
use std::sync::Arc;

pub fn broadcast(senders: &Vec<Sender<Arc<ServerMessage>>>, message: &ServerMessage) {
	let shared_message = Arc::new(message.clone()); // Wrap the message in an Arc
    for sender in senders.iter() {
        let message_clone = Arc::clone(&shared_message);
        if let Err(e) = sender.try_send(message_clone) {
            eprintln!("Failed to send message: {}", e);
        }
    }
}

pub async fn start(mut sender_receiver: Receiver<Sender<Arc<ServerMessage>>>, client_message_receiver: Receiver<ClientMessage>) {
	let mut server_message_senders = Vec::<Sender<Arc<ServerMessage>>>::new();
	loop {
		while let Ok(sender) = {println!("calculation_unit: try receive a new sender"); sender_receiver.try_recv()} {
			server_message_senders.push(sender);
		}
		println!("do calulation");
	}
}
