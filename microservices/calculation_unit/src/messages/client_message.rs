use serde::{Serialize, Deserialize};

use crate::game::coordinate::Coordinate;

use super::websocket_format::RequestInfo;


#[derive(Serialize, Deserialize, Debug, Clone)]
pub enum ClientRequest {
	MoveTo {
		id: String,
		position: Coordinate,
	}
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ClientMessage {
    pub request_info: RequestInfo,
    pub request_data: ClientRequest,
}
