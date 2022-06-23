#include <sim.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define DRAG_COEFFICIENT 1.17
// density of medium, radius, velocity
#define AIR_RESISTANCE(p, r, v) (-((((p) * DRAG_COEFFICIENT * pow(r * 2, 2)) / 2) * (v) * (v)))
#define GRAVITY(G, m) ((G) * (m) * 9.8)

#define MULTIPLY_BY_MS(value, ms) (((value) * (ms)) / (sim_vec) 1000)

sim_circle_run_result sim_circle_run(
	sim_circle *self,
	const sim_time delta,
	const bool start,
	const sim_vec height,
	const sim_vec radius
) {
	if (self->center + radius >= height) {
		self->velocity = -self->velocity; // bounce
		fprintf(stderr, "BOUNCED!\n");
	}
	if (!start)
		self->center += MULTIPLY_BY_MS(self->velocity, delta);
	fprintf(stderr, "center = %Lf\n", self->center);
	sim_circle_run_result result;
	result.forces[0] = (sim_force) {
		.force = GRAVITY(self->gravitational_const, self->mass),
		.type = SIM_FORCE_GRAVITY
	};
	fprintf(stderr, "gravity = %Lf\n", result.forces[0].force);
	result.forces[1] = (sim_force) {
		.force = AIR_RESISTANCE(
			self->medium_density_const,
			radius,
			self->velocity
		),
		.type = SIM_FORCE_AIR_RESISTANCE
	};
	fprintf(stderr, "air resistance = %Lf\n", result.forces[1].force);
	fprintf(stderr, "NEXT: %Lf\n", (result.forces[0].force + result.forces[1].force) / self->mass);
	self->velocity += MULTIPLY_BY_MS(
		(result.forces[0].force + result.forces[1].force) / self->mass,
		delta
	);
	fprintf(stderr, "velocity = %Lf\n\n", self->velocity);
	return result;
}

