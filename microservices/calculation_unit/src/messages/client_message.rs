use std::collections::HashMap;

use serde::{Serialize, Deserialize};

use crate::game::{coordinate::Coordinate, dummy::DummyObject};

use super::websocket_format::RequestInfo;

#[derive(Serialize, Deserialize, Debug, Clone)]
pub enum ClientRequest {
	SpawnDummy {
		id: String
	},
	DummyMoveBy {
		id: String,
		position: Coordinate,
	},
}

impl ClientRequest {
	// executes a clients input data on the game
	pub fn execute(self, dummys: &mut HashMap<String, DummyObject>, delta_seconds: f64) -> std::result::Result<(), String> {
		match self {

			// TEMP move dummy client by certain amount
		    ClientRequest::DummyMoveBy { id, mut position } => {
		    	// TODO
		    	match dummys.get_mut(&id) {
		            Some(dummy) => {
		            	dummy.position.addd(&position, delta_seconds);
		            },
		            None => {
		            	// TODO handle connection if connection tried to move non existent object
		            	return Err(format!("tried moving dummy but dummy doesnt exist by this id: {}", id))
		            },
		        };
		    },

			// TEMP spawn dummy client
		    ClientRequest::SpawnDummy { id } => {
		    	// check if dummy with id already exists
		    	match dummys.get(&id) {
		            Some(_) => {
		            	return Err(format!("error: couldnt spawn dummy, dummy already exists! id: {}", id));
		            },
		            None => { },
		        };

		        // spawn dummy
				println!("spawn dummy client");
				let dummy = DummyObject {
					id: id.clone(), 
					position: Coordinate::default(),
				};
				dummys.insert(id, dummy);
		    },
		};
		Ok(())
	}
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ClientMessage {
    pub request_info: RequestInfo,
    pub request_data: ClientRequest,
}
