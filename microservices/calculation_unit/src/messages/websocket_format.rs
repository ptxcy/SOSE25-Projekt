use serde::{Serialize, Deserialize};


#[derive(Serialize, Deserialize, Debug)]
pub struct ClientInfo {
	sent_time: f64,
}
#[derive(Serialize, Deserialize, Debug)]
pub struct AuthproxyInfo {
	sent_time: f64,
}
#[derive(Serialize, Deserialize, Debug)]
pub struct RequestSyncInfo {
	sent_time: f64,
}
#[derive(Serialize, Deserialize, Debug)]
pub struct CalculationUnitInfo {
	sent_time: f64,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct RequestInfo {
	client: ClientInfo,
	authproxy: AuthproxyInfo,
	request_sync: RequestSyncInfo,
	calculation_unit: CalculationUnitInfo,
}

