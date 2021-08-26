//
// Created by xerne on 25/08/2021.
//

#include "Renderer.h"

void Renderer::sample_frame(vec3* target) const {
	// Setting up the ray
	ray r = {{-3, 0, 0},
	         {0,  0, 1}};

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			r.direction = {2, ((scalar) x)/((scalar) width)*2 - 1, ((scalar) y)/((scalar) height)*2 - 1};
			r.direction = normalize(r.direction);

			target[y*width + x] = pt_engine.trace_path(r, MAX_BOUNCE_COUNT, (rng_engine*)(&random_engine));;
		}
	}
}

void Renderer::multisample_frame(vec3* target) const {
	vec3** buffs = new vec3*[no_threads];
	for (int i = 0; i < no_threads; i++) {
		buffs[i] = new vec3[width*height];
	}

	for (int i = 0; i < SAMPLES; i+=no_threads) {
		auto* threads = new std::thread[no_threads];
		for (int t = 0; t < no_threads; t++) {
			threads[t] = std::thread(&Renderer::sample_frame, this, buffs[t]);
		}
		for (int t = 0; t < no_threads; t++) {
			threads[t].join();
		}

		for (int j = 0; j < no_threads; j++) {
			for (int k = 0; k < width*height; ++k) {
				target[k] = target[k] + buffs[j][k];
			}
		}

	}
}

void Renderer::tonemap_frame(vec3* frame) const {
	// TODO: make this func good.
	const int no_pixels = width*height;
	vec3 total_light = {0, 0, 0};

	for (int j = 0; j < no_pixels; ++j) {
		total_light = total_light + frame[j];
	}

	const vec3 average_light = total_light*((scalar) 1/(scalar) no_pixels); // painful division
	const scalar average_strength = max_S(average_light.x, max_S(average_light.y, average_light.z)); // ew

	const scalar factor = (scalar) 1/(average_strength*2);

	for (int j = 0; j < no_pixels; ++j) {
		frame[j] = frame[j]*factor;
	}
}
