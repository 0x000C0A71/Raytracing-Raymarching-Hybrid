//
// Created by marc on 25/08/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_RENDERER_H
#define RAYTRACING_RAYMARCHING_HYBRID_RENDERER_H

#include "Types.h"
#include "Pathtracing.h"
#include <thread>

#ifdef DO_TELEMETRY
#include <cstdio>
#include <ctime>
#endif

struct Renderer {
	pathtracing::Pathtracer pt_engine;

	int width, height;
	scalar _width_f{}, _height_f{};
	int no_threads;
	rng_engine random_engine{};

	Renderer(pathtracing::Pathtracer pt_engine, int width, int height, int no_threads)
			: pt_engine(pt_engine), width(width), height(height), no_threads(no_threads) {}

	void sample_frame(vec3* target);

	void multisample_frame(vec3* target, int samples);

	void tonemap_frame(vec3* frame);

	inline void build() {
		pt_engine.build();
		_width_f = 1/((scalar)width);
		_height_f = 1/((scalar)height);
	}
};

#endif //RAYTRACING_RAYMARCHING_HYBRID_RENDERER_H
