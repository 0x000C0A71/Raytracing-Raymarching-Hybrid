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

			scalar distance_function(vec3 p) const override {
				vec3 z = p;
				scalar dr = 1.0;
				scalar r = 0.0;
				for (int i = 0; i < Iterations; i++) {
					const scalar r_sq = glm::length2(z);
					const scalar r_inv = Q_rsqrt(r_sq);
					r = 1/r_inv;
					if (r_sq > (Bailout*Bailout)) break;

					// convert to polar coordinates
					const scalar theta = glm::fastAcos(z.z * r_inv); // gruusig
					const scalar phi = glm::fastAtan(z.y, z.x); // gruusig

					const scalar r_p2 = r_sq;
					const scalar r_p4 = r_p2*r_p2;

					dr = (r_p4*r_p2*r)*8*dr + 1.0;

					// scale and rotate the point
					const scalar zr = r_p4*r_p4;
					const scalar theta_2 = theta*8;
					const scalar phi_2 = phi*8;

					// convert back to cartesian coordinates
					const scalar sin_theta = glm::fastSin(theta_2);
					const scalar cos_theta = glm::fastCos(theta_2);
					const scalar sin_phi = glm::fastSin(phi_2);
					const scalar cos_phi = glm::fastCos(phi_2);
					z = zr*vec3{sin_theta*cos_phi,sin_phi*sin_theta, cos_theta};
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
