//
// Created by marc on 25/08/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_RENDERER_H
#define RAYTRACING_RAYMARCHING_HYBRID_RENDERER_H

#include "Types.h"
#include "Pathtracing.h"
#include <random>
#include <thread>

struct Renderer {
	pathtracing::Pathtracer pt_engine;

	int width, height;
	int no_threads;
	rng_engine random_engine{};

	void sample_frame(vec3* target) const;

	void multisample_frame(vec3* target) const;

	void tonemap_frame(vec3* frame) const;
};

#endif //RAYTRACING_RAYMARCHING_HYBRID_RENDERER_H
