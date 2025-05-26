use std::f64::consts::PI;

use super::orbit::OrbitInfo;

// basierend auf https://github.com/callidaria/modsim_planetary/blob/master/astro.ipynb

/// Returns the sine of an angle in degrees.
///
/// # Parameters
/// - `a`: Angle in degrees.
///
/// # Returns
/// - Sine of the angle.
pub fn sin_deg(a: f64) -> f64 {
	(a * (PI / 180.0)).sin()
}

/// Returns the cosine of an angle in degrees.
///
/// # Parameters
/// - `a`: Angle in degrees.
///
/// # Returns
/// - Cosine of the angle.
pub fn cos_deg(a: f64) -> f64 {
	(a * (PI / 180.0)).cos()
}

/// Calculates the Julian day for a given date.
///
/// # Parameters
/// - `yy`: Year (i32)
/// - `mm`: Month (i32)
/// - `dd`: Day (i32)
///
/// # Returns
/// - Julian day as f64.
pub fn julian_day(mut yy: i32, mut mm: i32, dd: i32) -> f64 {
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

/// Evaluates a cubic polynomial curve at t with coefficients a0, a1, a2, a3.
///
/// # Parameters
/// - `t`: The input value.
/// - `a0`: Constant coefficient.
/// - `a1`: Linear coefficient.
/// - `a2`: Quadratic coefficient.
/// - `a3`: Cubic coefficient.
///
/// # Returns
/// - Value of the cubic polynomial at t.
pub fn get_curve(t: f64, a0: f64, a1: f64, a2: f64, a3: f64) -> f64 {
	return (a3 * t.powi(3)) + (a2 * t.powi(2)) + (a1 * t) + a0;
}

/// Solves Kepler's equation for the eccentric anomaly.
///
/// # Parameters
/// - `m`: Mean anomaly in degrees.
/// - `e`: Eccentricity.
///
/// # Returns
/// - Eccentric anomaly in degrees.
pub fn anomaly(m: f64, e: f64) -> f64 {
	let mut last = 0.0;
	let mut out = m + e * sin_deg(m) * (1.0 + e * cos_deg(m)) * (180.0 / PI);

	while (out - last).abs() > 0.00001 {
		last = out;
		out = last - (last - e * (180.0 / PI) * sin_deg(last) - m) / (1.0 - e * cos_deg(last));
	}
	out
}

/// Reduces an angle to the range [0, 360) degrees.
///
/// # Parameters
/// - `n`: Angle in degrees.
///
/// # Returns
/// - Angle in [0, 360) degrees.
pub fn trim_down(n: f64) -> f64 {
	n.rem_euclid(360.0) // rem_euclid handles negative numbers correctly for modulo
}

/// Calculates the time factor in Julian centuries since J2000.0.
///
/// # Parameters
/// - `d`: Julian day.
///
/// # Returns
/// - Time factor (Julian centuries since J2000.0).
pub fn get_timefactor(d: f64) -> f64 {
	(d - 2451545.0) / 36525.0
}

/// Computes the position vector (x, y) in the orbital plane.
///
/// # Parameters
/// - `a`: Semi-major axis.
/// - `e_anomaly`: Eccentric anomaly in degrees.
/// - `e`: Eccentricity.
///
/// # Returns
/// - Tuple (x, y) representing the position in the orbital plane.
pub fn distance_vector(a: f64, e_anomaly: f64, e: f64) -> (f64, f64) {
	let x = a * (cos_deg(e_anomaly) - e);
	let y = a * (1.0 - e * e).sqrt() * sin_deg(e_anomaly);
	(x, y)
}

/// Returns the distance (magnitude) of a 2D vector.
///
/// # Parameters
/// - `v`: Reference to a tuple (x, y).
///
/// # Returns
/// - Magnitude (distance) of the vector.
pub fn get_distance(v: &(f64, f64)) -> f64 {
	(v.0.powi(2) + v.1.powi(2)).sqrt()
}

/// Calculates the true anomaly from eccentricity and eccentric anomaly.
///
/// # Parameters
/// - `e`: Eccentricity.
/// - `E_deg`: Eccentric anomaly in degrees.
///
/// # Returns
/// - True anomaly in degrees.
pub fn true_anomaly(e: f64, E_deg: f64) -> f64 {
	let E_rad = E_deg.to_radians();
	let sqrt = ((1.0 + e) / (1.0 - e)).sqrt();
	let tan_v2 = sqrt * (E_rad / 2.0).tan();
	let v_rad = 2.0 * tan_v2.atan();
	v_rad.to_degrees()
}

/// Converts orbital elements to 3D cartesian coordinates.
///
/// # Parameters
/// - `r`: Distance from the focus.
/// - `N`: Longitude of ascending node in degrees.
/// - `v`: True anomaly in degrees.
/// - `w`: Argument of periapsis in degrees.
/// - `i`: Inclination in degrees.
///
/// # Returns
/// - Array [x, y, z] representing the 3D cartesian coordinates.
pub fn cartesian_coordinates(r: f64, N: f64, v: f64, w: f64, i: f64) -> [f64; 3] {
	let vw = v + w;
	[
		r * (cos_deg(N) * cos_deg(vw) - sin_deg(N) * sin_deg(vw) * cos_deg(i)),
		r * (sin_deg(N) * cos_deg(vw) + cos_deg(N) * sin_deg(vw) * cos_deg(i)),
		r * (sin_deg(vw) * sin_deg(i)),
	]
}

/// Calculates the 3D cartesian coordinates of a planet based on its orbital elements.
///
/// # Parameters
/// - `info`: Reference to an `OrbitInfo` struct containing the planet's orbital elements.
///
/// # Returns
/// - Array `[x, y, z]` representing the planet's position in 3D space.
pub fn calculate_planet(info: &OrbitInfo) -> [f64; 3] {
	let E = anomaly(info.M, info.e);
	let vxy = distance_vector(info.a, E, info.e);
	let r = get_distance(&vxy);
	let v = true_anomaly(info.e, E);
	cartesian_coordinates(r, info.N, v, info.w, info.i)
}
