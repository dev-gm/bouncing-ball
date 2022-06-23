#include <sim.h>

sim_circle_run_result sim_circle_run(
	sim_circle *self,
	const sim_time delta,
	const bool start,
	const sim_vec height,
	const sim_vec radius
) {
	sim_circle_run_result result;
	result.forces[0] = (sim_force) {
		.force = 9.8 * 167 * self->mass,
		.type = SIM_FORCE_GRAVITY
	};
	result.forces[1] = (sim_force) {
		.force =
			self->air_resistance_const *
			(self->velocity < 0 ? self->velocity : -self->velocity) *
			self->velocity,
		.type = SIM_FORCE_AIR_RESISTANCE
	};
	if (!start) {
		self->velocity += (((result.forces[0].force + result.forces[1].force) / self->mass) * delta) / 1000;
		if (self->center + radius - radius >= height)
			self->velocity = -self->velocity;
		self->center += (self->velocity * delta) / 1000;
	}
	return result;
} 
