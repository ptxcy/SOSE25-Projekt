use crate::{client_message::ClientMessage, server_message::ServerMessageSenderChannel};
use tokio::sync::mpsc::*;

pub async fn start(
    server_message_sender_receiver: Receiver<ServerMessageSenderChannel>,
    client_message_receiver: Receiver<ClientMessage>,
) {
    // game loop
    loop {
        tokio::task::yield_now().await;
    }
}
