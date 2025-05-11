use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize, Debug, Clone, Copy, Default)]
pub struct ClientInfo {
	sent_time: f64,
}

#[derive(Serialize, Deserialize, Debug, Clone, Copy, Default)]
pub struct AuthproxyInfo {
	sent_time: f64,
}

#[derive(Serialize, Deserialize, Debug, Clone, Copy, Default)]
pub struct RequestSyncInfo {
	sent_time: f64,
}

#[derive(Serialize, Deserialize, Debug, Clone, Copy, Default)]
pub struct CalculationUnitInfo {
	pub sent_time: f64,
}

#[derive(Serialize, Deserialize, Debug, Clone, Copy, Default)]
pub struct RequestInfo {
	client: ClientInfo,
	authproxy: AuthproxyInfo,
	request_sync: RequestSyncInfo,
	pub calculation_unit: CalculationUnitInfo,
}

impl RequestInfo {
    pub fn new(sent_time: f64) -> Self {
    	Self {
	        calculation_unit: CalculationUnitInfo { sent_time },
	        ..Default::default()
	    }
    }
}

