use calculation_unit::game::orbit::*;
use calculation_unit::game::planet::get_timefactor;
use calculation_unit::game::planet::julian_day;
use plotly::Layout;
use plotly::common::Marker;
use plotly::common::Mode;
use plotly::{Plot, Scatter3D};

fn trace_planet(
	coords: &Vec<[f64; 3]>,
	name: &str,
) -> std::boxed::Box<plotly::Scatter3D<f64, f64, f64>> {
	let x: Vec<f64> = coords.iter().map(|c| c[0]).collect();
	let y: Vec<f64> = coords.iter().map(|c| c[1]).collect();
	let z: Vec<f64> = coords.iter().map(|c| c[2]).collect();

	let mut trace = Scatter3D::new(x, y, z);
	trace
		.name(name)
		.mode(Mode::Lines)
		.line(plotly::common::Line::new().width(2.0))
}

fn main() {
	let d = julian_day(2020, 3, 3);

	let mut mercury_xyz = vec![];
	let mut venus_xyz = vec![];
	let mut earth_xyz = vec![];
	let mut mars_xyz = vec![];

	let mut jupiter_xyz = vec![];
	let mut saturn_xyz = vec![];
	let mut uranus_xyz = vec![];
	let mut neptune_xyz = vec![];

	for i in 0..720 {
		let t = get_timefactor(d - i as f64);
		mercury_xyz.push(calculate_planet(&get_mercury(t)));
		venus_xyz.push(calculate_planet(&get_venus(t)));
		earth_xyz.push(calculate_planet(&get_earth(t)));
		mars_xyz.push(calculate_planet(&get_mars(t)));
	}

	for i in 0..60000 {
		let t = get_timefactor(d - i as f64);
		jupiter_xyz.push(calculate_planet(&get_jupiter(t)));
		saturn_xyz.push(calculate_planet(&get_saturn(t)));
		uranus_xyz.push(calculate_planet(&get_uranus(t)));
		neptune_xyz.push(calculate_planet(&get_neptune(t)));
	}

	let mut plot = Plot::new();

	// Sonne
	let sun = Scatter3D::new(vec![0.0], vec![0.0], vec![0.0])
		.mode(Mode::Markers)
		.name("Sun")
		.marker(Marker::new().size(8).color("yellow"));
	plot.add_trace(sun);

	// Planeten
	plot.add_trace(trace_planet(&mercury_xyz, "Mercury"));
	plot.add_trace(trace_planet(&venus_xyz, "Venus"));
	plot.add_trace(trace_planet(&earth_xyz, "Earth"));
	plot.add_trace(trace_planet(&mars_xyz, "Mars"));
	plot.add_trace(trace_planet(&jupiter_xyz, "Jupiter"));
	plot.add_trace(trace_planet(&saturn_xyz, "Saturn"));
	plot.add_trace(trace_planet(&uranus_xyz, "Uranus"));
	plot.add_trace(trace_planet(&neptune_xyz, "Neptune"));

	plot.set_layout(
		Layout::new()
			.width(1000)
			.height(800)
			.title("Solar System Orbits"),
	);

	plot.show();
}
