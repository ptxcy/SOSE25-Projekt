use std::collections::HashMap;

use serde::{Deserialize, Serialize};

use super::{planet::OrbitInfoMap, planet_util::{get_curve, trim_down}};

/// N = Länge des aufsteigenden Knotens (oft auch als großes Omega geschrieben)
/// Diese definiert die Drehung der Schnittgraden von der Ebene der Erdbahnellipse mit der Ebene der jeweiligen Planetenbahnellipse um die z-Achse.
///
/// i = Bahnneigung in Relation zur Ebene der Erdbahn
///
/// w = Periapsisabstand (oft auch als kleines Omega geschrieben)
/// Definiert die Rotation der Periapsis auf der jeweiligen Planetenbahnellipse
///
/// a = Länge der großen Halbachse oder auch Hauptachse in AE
///
/// e = Exzentrizität der Ellipse (für )
/// Diese Variable definiert die Form der Ellipse, wobei 0 kreisförmig ist und bei steigendem Wert die beiden Foci der Ellipse weiter auseinandergehen.
///
/// M = Mittlere Anomalie
#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct OrbitInfo {
	pub N: f64,
	pub i: f64,
	pub w: f64,
	pub a: f64,
	pub e: f64,
	pub M: f64,
}

impl OrbitInfo {
	pub fn new(N: f64, i: f64, w: f64, a: f64, e: f64, M: f64) -> Self {
		Self { N, i, w, a, e, M }
	}
}

pub fn initialize_orbit_info_map() -> OrbitInfoMap {
	let mut map = HashMap::new();
	map.insert("mercury".to_string(), get_mercury as fn(f64) -> OrbitInfo);
	map.insert("venus".to_string(), get_venus as fn(f64) -> OrbitInfo);
	map.insert("mars".to_string(), get_mars as fn(f64) -> OrbitInfo);
	map.insert("jupiter".to_string(), get_jupiter as fn(f64) -> OrbitInfo);
	map.insert("saturn".to_string(), get_saturn as fn(f64) -> OrbitInfo);
	map.insert("uranus".to_string(), get_uranus as fn(f64) -> OrbitInfo);
	map.insert("neptune".to_string(), get_neptune as fn(f64) -> OrbitInfo);
	map.insert("earth".to_string(), get_earth as fn(f64) -> OrbitInfo);
	map
}

pub fn get_mercury(t: f64) -> OrbitInfo {
	let n = get_curve(t, 48.330893, 1.1861883, 0.00017542, 0.000000215);
	let r = get_curve(t, 77.456119, 1.5564776, 0.00029544, 0.000000009);
	let l = get_curve(t, 252.250906, 149474.072249, 0.0003035, 0.000000018);

	OrbitInfo::new(
		n,
		get_curve(t, 7.004986, 0.0018215, -0.0000181, 0.000000056),
		r - n,       // w = r - N
		0.387098310, // a is a constant for Mercury in this model
		get_curve(t, 0.20563175, 0.000020407, -0.0000000283, -0.00000000018),
		trim_down(l - r), // M = L - r, trimmed
	)
}

pub fn get_venus(t: f64) -> OrbitInfo {
	let n = get_curve(t, 76.67992, 0.9011206, 0.00040618, -0.000000093);
	let r = get_curve(t, 131.563703, 1.4022288, -0.000107618, -0.000005678);
	let l = get_curve(t, 181.979801, 58519.2130302, 0.00031014, 0.000000015);

	OrbitInfo::new(
		n,
		get_curve(t, 3.394662, 0.0010037, -0.00000088, -0.000000007),
		r - n,
		0.72332982,
		get_curve(t, 0.00677192, -0.000047765, 0.0000000981, 0.00000000046),
		trim_down(l - r),
	)
}

pub fn get_mars(t: f64) -> OrbitInfo {
	let n = get_curve(t, 49.558093, 0.7720959, 0.00001557, 0.000002267);
	let r = get_curve(t, 336.060234, 1.8410449, 0.00013477, 0.000000536);
	let l = get_curve(t, 355.433, 19141.6964471, 0.00031052, 0.000000016);

	OrbitInfo::new(
		n,
		get_curve(t, 1.849726, -0.0006011, 0.00001276, -0.000000007),
		r - n,
		1.523679342,
		get_curve(t, 0.09340065, 0.000090484, -0.0000000806, -0.00000000025),
		trim_down(l - r),
	)
}

pub fn get_jupiter(t: f64) -> OrbitInfo {
	let n = get_curve(t, 100.464407, 1.0209774, 0.00040315, 0.0);
	let r = get_curve(t, 14.331207, 1.6126352, 0.00103042, -0.000004464);
	let l = get_curve(t, 34.351519, 3036.3027748, 0.0002233, 0.000000037);
	let a = t * 0.0000001913 + 5.202603209; // 'a' is a linear function of T for Jupiter

	OrbitInfo::new(
		n,
		get_curve(t, 1.303267, -0.0054965, 0.00000466, -0.000000002),
		r - n,
		a,
		get_curve(t, 0.04849793, 0.000163225, -0.0000004714, -0.00000000201),
		trim_down(l - r),
	)
}

pub fn get_saturn(t: f64) -> OrbitInfo {
	let n = get_curve(t, 113.665503, 0.877088, -0.00012176, -0.000002249);
	let r = get_curve(t, 93.057237, 1.9637613, 0.00083753, 0.000004928);
	let l = get_curve(t, 50.077444, 1223.5110686, 0.00051908, -0.00000003);
	let a = t.powi(2) * 0.000000004 + t * -0.000002139 + 9.554909192; // 'a' is a quadratic function of T for Saturn

	OrbitInfo::new(
		n,
		get_curve(t, 2.488879, -0.0037362, -0.00001519, 0.000000087),
		r - n,
		a,
		get_curve(t, 0.05554814, -0.0003466641, -0.0000006436, -0.0000000034),
		trim_down(l - r),
	)
}

pub fn get_uranus(t: f64) -> OrbitInfo {
	let n = get_curve(t, 74.005957, 0.5211278, 0.00133947, 0.000018484);
	let r = get_curve(t, 173.005291, 1.486379, 0.000214, 0.000000434);
	let l = get_curve(t, 314.055005, 429.8640561, 0.0003039, 0.000000026);
	let a = t.powi(2) * 0.00000000098 + t * -0.0000000372 + 19.218446062; // 'a' is a quadratic function of T for Uranus

	OrbitInfo::new(
		n,
		get_curve(t, 0.773197, 0.0007744, 0.00003749, -0.000000092),
		r - n,
		a,
		get_curve(t, 0.04638122, -0.000027293, 0.0000000789, 0.00000000024),
		trim_down(l - r),
	)
}

pub fn get_neptune(t: f64) -> OrbitInfo {
	let n = get_curve(t, 131.784057, 1.1022039, 0.00025952, -0.000000637);
	let r = get_curve(t, 48.120276, 1.4262957, 0.00038434, 0.00000002);
	let l = get_curve(t, 304.348665, 219.8833092, 0.00030882, 0.000000018);
	let a = t.powi(2) * 0.00000000069 + t * -0.0000001663 + 30.110386869; // 'a' is a quadratic function of T for Neptune

	OrbitInfo::new(
		n,
		get_curve(t, 1.769953, -0.0093082, -0.00000708, 0.000000027),
		r - n,
		a,
		get_curve(t, 0.00945575, 0.000006033, 0.0, -0.00000000005),
		trim_down(l - r),
	)
}

pub fn get_earth(t: f64) -> OrbitInfo {
	let r = get_curve(t, 102.937348, 1.7195366, 0.00045688, -0.000000018);
	let l = get_curve(t, 100.466457, 36000.7698278, 0.00030322, 0.00000002);
	let a = 1.000001018;

	OrbitInfo::new(
		0.0,
		0.0,
		r,
		a,
		get_curve(t, 0.01670863, -0.000042037, -0.0000001267, 0.00000000014), // e (Exzentrizität)
		trim_down(l - r), // M (mittlere Anomalie)
	)
}
