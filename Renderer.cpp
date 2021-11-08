//
// Created by marc on 25/08/2021.
//

#include "Renderer.h"

void Renderer::sample_frame(vec3* target) {
	// Setting up the ray
	ray r = {{-4, 0, 0},
	         {0,  0, 1}};

	const scalar off_x = random_engine.draw_alpha();
	const scalar off_y = random_engine.draw_alpha();
	// TODO              ^^^^^^^^^^^^^^^^^^^^^^^^^^---- This method of supersampling is rather crude and can be improved

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			r.direction = {3, ((scalar)x + off_x)*_width_f*2 - 1, ((scalar)y + off_y)*_height_f*2 - 1};
			r.direction = normalize(r.direction);

			target[y*width + x] = pt_engine.trace_path(r, MAX_BOUNCE_COUNT, &random_engine);;
		}
	}
}

void Renderer::multisample_frame(vec3* target) {
	vec3** buffs = new vec3* [no_threads];
	auto* threads = new std::thread[no_threads];
	for (int i = 0; i < no_threads; i++) {
		buffs[i] = new vec3[width*height]{{0, 0, 0}};
	}

#ifdef DO_TELEMETRY
	putchar(10);
	auto start_time = clock();
	auto current_time = clock();
#endif

	for (int i = 0; i < SAMPLES; i += no_threads) {
		for (int t = 0; t < no_threads; t++) {
			threads[t] = std::thread(&Renderer::sample_frame, this, buffs[t]);
		}

#ifdef DO_TELEMETRY
		const double elapsed_t = (double)(current_time - start_time)/CLOCKS_PER_SEC;
		const double percent = i*((double)1/SAMPLES);
		const double total_t = elapsed_t/percent;
		printf("\rRendered %i of %i samples (%.f%%). ETA: %.1f sec (total: %.1f sec)        ",
			   i, SAMPLES, percent*100, total_t - elapsed_t, total_t);
#endif

		for (int t = 0; t < no_threads; t++) {
			threads[t].join();
		}

		for (int j = 0; j < no_threads; j++) {
			for (int k = 0; k < width*height; ++k) {
				target[k] = target[k] + buffs[j][k];
			}
		}

#ifdef DO_TELEMETRY
		current_time = clock();
#endif
	}
}

void Renderer::tonemap_frame(vec3* frame) {
	// TODO: make this func good.
	const int no_pixels = width*height;
	vec3 total_light = {0, 0, 0};

	for (int j = 0; j < no_pixels; ++j) {
		total_light = total_light + frame[j];
	}

	const vec3 average_light = total_light*((scalar)1/(scalar)no_pixels); // painful division
	const scalar average_strength = max_S(average_light.x, max_S(average_light.y, average_light.z)); // ew

	const scalar factor = (scalar)1/(average_strength*2);

	for (int j = 0; j < no_pixels; ++j) {
		frame[j] = frame[j]*factor;
	}
}
