use tokio::sync::mpsc::Sender;

use crate::messages::server_message::ServerMessage;


pub fn start(server_message_tx: Sender<ServerMessage>) {
	loop {
		
	}
}
