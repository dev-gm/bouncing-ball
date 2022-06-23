#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <sim.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int window_size[2] = { 1280, 720 },
	circle_radius = 75;

int main(int argc, char **argv) {
	size_t num_of_circles = 0, i = 1;
	for (i = 1; i < (size_t) argc; ++i) {
		if (argv[i][0] == '-' && argv[i][1] == '-') {
			++num_of_circles;
		} else if (num_of_circles == 0) {
			if (memcmp("radius=", argv[i], 7) == 0)
				circle_radius = atoi(argv[i] + 7);
			else if (memcmp("window_x=", argv[i], 9) == 0)
				window_size[0] = atoi(argv[i] + 9);
			else if (memcmp("window_y=", argv[i], 9) == 0)
				window_size[0] = atoi(argv[i] + 9);
		}
	}
	if (num_of_circles == 0) {
		puts("Zero circles! Aborting...");
		return 0;
	}
	sim_circle circles[num_of_circles];
	size_t circles_index = 0;
	circles[0] = (sim_circle) {
		.mass = 5000,
		.gravitational_const = 1,//167,
		.air_resistance_const = 10,
		.velocity = 0,
		.center = 0
	};
	for (i = 1; i < (size_t) argc; ++i) {
		if (argv[i][0] == '-' && argv[i][1] == '-') {
			++circles_index;
			circles[circles_index] = (sim_circle) {
				.mass = 5000,
				.gravitational_const = 10,
				.air_resistance_const = 10,
				.velocity = 0,
				.center = 0
			};
			continue;
		}
		if (memcmp("mass=", argv[i], 5) == 0)
			circles[circles_index].mass = atof(argv[i] + 5);
		else if (memcmp("G=", argv[i], 2) == 0)
			circles[circles_index].gravitational_const *= atof(argv[i] + 2);
		else if (memcmp("air_resistance=", argv[i], 15) == 0)
			circles[circles_index].air_resistance_const = atof(argv[i] + 15);
	}
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
		fprintf(stderr, "Failed to initialize simulation: %s", SDL_GetError());
		return 1;
	}
	SDL_Window *window = SDL_CreateWindow(
		"Physics Simulation",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		window_size[0],
		window_size[1],
		0
	);
	if (!window) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Renderer *renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED
	);
	if (!renderer) {
		fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Event event;
	size_t circle_col_inc = window_size[0] / num_of_circles, col;
	size_t delay = 1000 / 60;
	sim_time prev_millis[num_of_circles], now_millis[num_of_circles];
	sim_circle_run_result circle_result;
	bool first_time = true;
	FILE *force_file = fopen("out-data/forces.data", "w+");
	FILE *center_file = fopen("out-data/center.data", "w+");
	FILE *velocity_file = fopen("out-data/velocity.data", "w+");
	FILE *energy_file = fopen("out-data/energy.data", "w+");
	for (;;) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				goto end;
		}
		SDL_Delay(delay);
		SDL_SetRenderTarget(renderer, NULL);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		for (i = 0, col = 0; i < num_of_circles; ++i, col += circle_col_inc) {
			circle_result = sim_circle_run(
				circles + i,
				(now_millis[i] = SDL_GetTicks64()) - prev_millis[i],
				first_time,
				window_size[1],
				circle_radius
			);
			first_time = false;
			if (circles[i].velocity == 0 && circles[i].center >= window_size[1])
				goto end;
			fprintf(force_file, "%lld \"gravity\"\n", circle_result.forces[0].force);
			fprintf(force_file, "%lld\"air resistance\"\n\n", circle_result.forces[1].force);
			printf("gravity = %lld\n", circle_result.forces[0].force);
			printf("air resistance = %lld\n\n", circle_result.forces[1].force);
			fprintf(center_file, "%lld\"center\"\n", window_size[1] - circles[i].center);
			fprintf(velocity_file, "%lld\"velocity\"\n\n", circles[i].velocity);
			printf("center = %lld\n", window_size[1] - circles[i].center);
			printf("velocity = %lld\n\n", circles[i].velocity);
			fprintf(energy_file, "%lld\"potential\"\n", (sim_vec) (circles[i].mass * 9.8 * window_size[1]));
			fprintf(energy_file, "%lld\"kinetic\"\n\n", (sim_vec) (0.5 * circles[i].mass * circles[i].velocity * circles[i].velocity));
			printf("potential energy = %lld\n", (sim_vec) (circles[i].mass * 9.8 * window_size[1]));
			printf("kinetic energy = %lld\n\n", (sim_vec) (circles[i].mass * 0.5 * pow(circles[i].velocity, 2)));
			prev_millis[i] = now_millis[i];
			filledCircleRGBA(
				renderer,
				(int) col - circle_radius,
				(int) circles[i].center - circle_radius,
				circle_radius,
				0, 0, 0, 255
			);
		}
		SDL_RenderPresent(renderer);
	}
	end:;
	fclose(force_file);
	fclose(center_file);
	fclose(velocity_file);
	fclose(energy_file);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
