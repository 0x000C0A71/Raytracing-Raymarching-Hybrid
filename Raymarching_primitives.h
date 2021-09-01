//
// Created by marc on 20/08/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_RAYMARCHING_PRIMITIVES_H
#define RAYTRACING_RAYMARCHING_HYBRID_RAYMARCHING_PRIMITIVES_H

#include "Raymarching.h"
#include "Types.h"
#include "fast_square_root.h"

namespace raymarching {
	namespace primitives {

		struct Sphere : Object {
			scalar radius;

			explicit Sphere(scalar radius) : radius(radius) {}

			scalar distance_function(vec3 p) const override {
				return fsqrt(sqLength(p)) - radius;
			}
		};

		struct Box : Object {
			vec3 dimensions;
			scalar radius = 0;

			Box(scalar width, scalar depth, scalar height) : dimensions({width, depth, height}) {};
			Box(scalar width, scalar depth, scalar height, scalar radius) : dimensions({width, depth, height}), radius(radius) {};

			scalar distance_function(vec3 p) const override {
				const vec3 q = p.abs() - dimensions;
				const scalar k = max_S(q.y, q.z);
				const scalar l = max_S(q.x, k);
				return fsqrt(sqLength({max_S(q.x, 0.0), max_S(q.y, 0.0), max_S(q.z, 0.0)})) + min_S(l, 0.0) - radius;
			}
		};

		struct Torus : Object {
			scalar primary_radius;
			scalar secondary_radius;

			Torus(scalar primary_radius, scalar secondary_radius) : primary_radius(primary_radius), secondary_radius(secondary_radius) {}

			scalar distance_function(vec3 p) const override {
				const scalar a = fsqrt(p.x*p.x + p.z*p.z) - primary_radius;
				const scalar k = p.y;
				return fsqrt(a*a + k*k) - secondary_radius;
			}
		};

	}
}

#endif //RAYTRACING_RAYMARCHING_HYBRID_RAYMARCHING_PRIMITIVES_H
