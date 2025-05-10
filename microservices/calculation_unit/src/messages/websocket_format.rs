use serde::{Serialize, Deserialize};


#[derive(Serialize, Deserialize, Debug, Clone, Copy)]
pub struct ClientInfo {
	sent_time: f64,
}
#[derive(Serialize, Deserialize, Debug, Clone, Copy)]
pub struct AuthproxyInfo {
	sent_time: f64,
}
#[derive(Serialize, Deserialize, Debug, Clone, Copy)]
pub struct RequestSyncInfo {
	sent_time: f64,
}
#[derive(Serialize, Deserialize, Debug, Clone, Copy)]
pub struct CalculationUnitInfo {
	pub sent_time: f64,
}

#[derive(Serialize, Deserialize, Debug, Clone, Copy)]
pub struct RequestInfo {
	client: ClientInfo,
	authproxy: AuthproxyInfo,
	request_sync: RequestSyncInfo,
	pub calculation_unit: CalculationUnitInfo,
}

