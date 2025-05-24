use std::f64::consts::PI;

// basierend auf https://github.com/callidaria/modsim_planetary/blob/master/astro.ipynb

pub fn sin_deg(a: f64) -> f64 {
	(a * (PI / 180.0)).sin()
}

pub fn cos_deg(a: f64) -> f64 {
	(a * (PI / 180.0)).cos()
}

pub fn julian_day(mut yy: i32, mut mm: i32, mut dd: i32) -> f64 {
	if mm < 3 && mm > 0 {
		yy -= 1;
		mm += 12;
	}
	let a = yy / 100;
	let b = if yy > 1582 || (yy == 1582 && (mm > 10 || (mm == 10 && dd >= 15))) {
		2 - a + a / 4
	} else {
		0
	};
	let l = (365.25 * (yy as f64 + 4716.)) as i32;
	let r = (30.6001 * (mm as f64 + 1.)) as i32;
	return (l + r + dd + b) as f64 - 1524.5;
}

pub fn get_curve(t: f64, a0: f64, a1: f64, a2: f64, a3: f64) -> f64 {
	return (a3 * t.powi(3)) + (a2 * t.powi(2)) + (a1 * t) + a0;
}

pub fn anomaly(m: f64, e: f64) -> f64 {
	let mut last = 0.0;
	let mut out = m + e * sin_deg(m) * (1.0 + e * cos_deg(m)) * (180.0 / PI);

	while (out - last).abs() > 0.00001 {
		last = out;
		out = last - (last - e * (180.0 / PI) * sin_deg(last) - m) / (1.0 - e * cos_deg(last));
	}
	out
}
pub fn trim_down(n: f64) -> f64 {
	n.rem_euclid(360.0) // rem_euclid handles negative numbers correctly for modulo
}

pub fn get_timefactor(d: f64) -> f64 {
	(d - 2451545.0) / 36525.0
}

pub fn distance_vector(a: f64, e_anomaly: f64, e: f64) -> (f64, f64) {
	let x = a * (cos_deg(e_anomaly) - e);
	let y = a * (1.0 - e * e).sqrt() * sin_deg(e_anomaly);
	(x, y)
}

pub fn get_distance(v: &(f64, f64)) -> f64 {
	(v.0.powi(2) + v.1.powi(2)).sqrt()
}

pub fn true_anomaly(e: f64, E_deg: f64) -> f64 {
	let E_rad = E_deg.to_radians();
	let factor = ((1.0 + e) / (1.0 - e)).sqrt() * E_rad.tan();
	2.0 * factor.atan().to_degrees()
}

pub fn cartesian_coordinates(r: f64, N: f64, v: f64, w: f64, i: f64) -> [f64; 3] {
	let vw = v + w;
	[
		r * (cos_deg(N) * cos_deg(vw) - sin_deg(N) * sin_deg(vw) * cos_deg(i)),
		r * (sin_deg(N) * cos_deg(vw) + cos_deg(N) * sin_deg(vw) * cos_deg(i)),
		r * (sin_deg(vw) * sin_deg(i)),
	]
}
