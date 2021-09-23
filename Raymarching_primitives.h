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
				return fsqrt(glm::length2(p)) - radius;
			}
		};

		struct Box : Object {
			vec3 dimensions;
			scalar radius = 0;

			Box(scalar width, scalar depth, scalar height) : dimensions({width, depth, height}) {};

			Box(scalar width, scalar depth, scalar height, scalar radius) : dimensions({width, depth, height}), radius(radius) {};

			scalar distance_function(vec3 p) const override {
				const vec3 q = glm::abs(p) - dimensions;
				const scalar k = max_S(q.y, q.z);
				const scalar l = max_S(q.x, k);
				return fsqrt(glm::length2(vec3{max_S(q.x, 0.0), max_S(q.y, 0.0), max_S(q.z, 0.0)})) + min_S(l, 0.0) - radius;
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

		struct Mandelbulb : Object {
			int Iterations;
			int Bailout;

			Mandelbulb(int Iterations, int Bailout) : Iterations(Iterations), Bailout(Bailout) {}

			/*
			void boxFold(vec3* z, scalar* dz) const {
				*z = glm::clamp(*z, -foldingLimit, foldingLimit)*2.0 - *z;
			}

			void sphereFold(vec3* z, scalar* dz) const {
				scalar r2 = glm::dot(*z, *z);
				if (r < minRadius2) {
					// linear inner scalling
					scalar temp = (fixedRadius2/minRadius2);
					*z *= temp;
					*dz *= temp;
				} else if (r2 < fixedRadius2) {
					// this is the actual sphere inversion
					scalar temp = (fixedRadius2/r2);
					*z *= temp;
					*dz *= temp;
				}
			}

			scalar distance_function(vec3 p) const override {
				vec3 z = p;
				vec3 offset = z;
				scalar dr = 1.0;
				for (int n = 0; n < Iterations; n++) {
					boxFold(&z, &dr);
					sphereFold(&z, &dr);

					z = Scale*z + offset;
					dr = dr*abs(Scale)+1.0;
				}
				scalar r = glm::length(z);
				return r/abs(dr);
			}*/


			scalar distance_function(vec3 p) const override {
				vec3 z = p;
				scalar dr = 1.0;
				scalar r = 0.0;
				for (int i = 0; i < Iterations; i++) {
					r = glm::length2(z);
					if (r > (Bailout*Bailout)) break;

					// convert to polar coordinates
					scalar theta = glm::fastAcos(z.z/r); // gruusig
					scalar phi = glm::fastAtan(z.y, z.x); // gruusig
					dr = (r*r*r*r*r*r*r)*8*dr + 1.0;

					// scale and rotate the point
					scalar zr = r*r*r*r*r*r*r*r;
					theta = theta*8;
					phi = phi*8;

					// convert back to cartesian coordinates
					z = zr*vec3{glm::fastSin(theta)*glm::fastCos(phi), glm::fastSin(phi)*glm::fastSin(theta), glm::fastCos(theta)};
					z = z + p;
				}
				return 0.5*glm::fastLog(r)*r/dr;
			}

		};

		struct Intersect : Merger {
			scalar merger_operator(scalar a, scalar b) const override { return max_S(a, b); }
		};

		struct Union : Merger {
			scalar merger_operator(scalar a, scalar b) const override { return min_S(a, b); }
		};

		struct SmoothUnion : Merger {
			scalar k;
			scalar _i_k;

			SmoothUnion(scalar k) : k(k) {};

			inline void build() override {
				_i_k = 1/k;
				Merger::build();
			}

			scalar merger_operator(scalar a, scalar b) const override {
				scalar h = glm::clamp(0.5 + 0.5*(b - a)*_i_k, 0.0, 1.0);
				return glm::mix(b, a, h) - k*h*(1.0-h);
			}
		};

		struct SmoothSubtraction : Merger {
			scalar k;
			scalar _i_k;

			SmoothSubtraction(scalar k) : k(k) {};

			inline void build() override {
				_i_k = 1/k;
				Merger::build();
			}

			scalar merger_operator(scalar a, scalar b) const override {
				scalar h = glm::clamp(0.5 - 0.5*(b + a)*_i_k, 0.0, 1.0);
				return glm::mix(b, -a, h) + k*h*(1.0-h);
			}
		};

	}
}

#endif //RAYTRACING_RAYMARCHING_HYBRID_RAYMARCHING_PRIMITIVES_H
