use std::collections::HashMap;

use serde::{Serialize, Deserialize};

use crate::game::{coordinate::Coordinate, dummy::DummyObject, game_objects::GameObjects};

use super::websocket_format::RequestInfo;

#[derive(Serialize, Deserialize, Debug, Clone)]
pub enum ClientRequest {
	SetClientFPS(f64),
	SpawnDummy {
		id: String
	},
	DummySetVelocity {
		id: String,
		position: Coordinate,
	},
}

impl Default for ClientRequest {
    fn default() -> Self {
    	Self::SetClientFPS(60.)
    }
}

impl ClientRequest {
	// executes a clients input data on the game
	pub fn execute(self, game_objects: &mut GameObjects, delta_seconds: f64) -> std::result::Result<(), String> {
		let dummies = &mut game_objects.dummies;
		match self {

			// TEMP move dummy client by certain amount
		    ClientRequest::DummySetVelocity { id, position } => {
		    	// TODO
		    	match dummies.get_mut(&id) {
		            Some(dummy) => {
		            	dummy.velocity = position;
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
		    	match dummies.get(&id) {
		            Some(_) => {
		            	return Err(format!("error: couldnt spawn dummy, dummy already exists! id: {}", id));
		            },
		            None => { },
		        };

		        // spawn dummy
				println!("spawn dummy client");
				let dummy = DummyObject {
					id: id.clone(), 
					..Default::default()
				};
				dummies.insert(id, dummy);
		    },
		    ClientRequest::SetClientFPS(fps) => {
		    	// TODO update the conneciton
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

impl Default for ClientMessage {
    fn default() -> Self {
    	ClientMessage {
    		request_info: Default::default(),
    		request_data: Default::default(),
    	}
    }
}
