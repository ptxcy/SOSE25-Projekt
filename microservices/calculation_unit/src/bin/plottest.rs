use calculation_unit::game::orbit::{get_mars, get_mercury};
use calculation_unit::game::planet::{anomaly, distance_vector, get_timefactor, julian_day};
use plotters::prelude::*;

fn main() -> Result<(), Box<dyn std::error::Error>> {
	let mut vxy_mercury = Vec::new();
	let mut vxy_mars = Vec::new();
	let d = julian_day(2020, 3, 3);

	for i in 0..700 {
		let T = get_timefactor(d - i as f64);
		let mercury = get_mercury(T);
		let mars = get_mars(T);

		let E = anomaly(mercury.M, mercury.e);
		vxy_mercury.push(distance_vector(mercury.a, E, mercury.e));

		let E = anomaly(mars.M, mars.e);
		vxy_mars.push(distance_vector(mars.a, E, mars.e));
	}

	let root = BitMapBackend::new("orbits.png", (800, 800)).into_drawing_area();
	root.fill(&WHITE)?;
	let mut chart = ChartBuilder::on(&root)
		.caption("Orbits of Mercury and Mars", ("sans-serif", 30))
		.margin(10)
		.x_label_area_size(30)
		.y_label_area_size(30)
		.build_cartesian_2d(-2.0..2.0, -2.0..2.0)?;

	chart.configure_mesh().draw()?;

	chart.draw_series(PointSeries::of_element(
		vec![(0.0, 0.0)],
		5,
		&RED,
		&|c, s, st| EmptyElement::at(c) + Circle::new((0, 0), s, st.filled()),
	))?;

	chart.draw_series(LineSeries::new(vxy_mercury.clone(), &BLUE))?;
	chart.draw_series(LineSeries::new(vxy_mars.clone(), &GREEN))?;

	Ok(())
}
