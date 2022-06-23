#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef signed long long sim_vec;
typedef uint32_t sim_time;

typedef struct {
	sim_vec x, y;
} sim_point;

typedef struct {
	sim_vec force;
	enum {
		SIM_FORCE_GRAVITY,
		SIM_FORCE_AIR_RESISTANCE
	} type;
} sim_force;

typedef struct {
	sim_vec gravitational_const,
			air_resistance_const;
	sim_vec mass, center, velocity;
} sim_circle;

typedef struct {
	sim_force forces[2];
} sim_circle_run_result;

sim_circle_run_result sim_circle_run(
	sim_circle *self,
	const sim_time delta,
	const bool start,
	const sim_vec height,
	const sim_vec radius
);

