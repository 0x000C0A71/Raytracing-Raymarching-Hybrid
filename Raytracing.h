//
// Created by marc on 11.07.21.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H
#define RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H

#include "Types.h"

namespace polygon {
	struct plane {
		// A plane is defined as follows: ax + by + cz + d = 0
		scalar a, b, c, d;
	};

	struct trigon {
		// A, B & C are the 3 corners of the trigon
		vec3 A, B, C;

		// Returns the normal of the trigon.
		inline vec3 get_normal() const { return normalize((B-A)*(C-A)); }

		// Generates the plane coefficients.
		inline plane get_plane(vec3 normal) const {
			const scalar a = normal.x;
			const scalar b = normal.y;
			const scalar c = normal.z;
			const scalar d = (-1)*(a*A.x + b*A.y + c*A.z);
			return {a, b, c, d};
		}
		inline plane get_plane() const { return get_plane(get_normal()); }
	};




	bool ray_plane_intersection(ray R, plane L, vec3 *poi, scalar *alpha);
	bool point_on_trigon(trigon tri, vec3 I, vec3* K);

	inline void gen2DPlaneTrough2Points(scalar x1, scalar y1, scalar x2, scalar y2, scalar* a, scalar* b, scalar* c) {
		const scalar x1d = x1;
		const scalar y1d = y1;
		const scalar x2d = -y2;
		const scalar y2d = x2;



		*c = x1d*x2d + y1d*y2d; // This might lead to inaccuracies in the future.
		if (*c == 0) {
			// May god help me
			// TODO: Pray to god for his help is needed in this case
		} else {
			*b = (x1 - x2)/(x2*y1 - x1*y2) * *c; // Painful division
			*a = (y1 - y2)/(x1*y2 - x2*y1) * *c; // Painful division pt.2
		}
	}

	struct Box {
		// The two corners of the box
		vec3 A, B;

		inline bool point_in_box(vec3 p) const {
			// This function is branchless right now. If any of the conditions are false, the output will be false, so
			// you could return early. This might speed it up or it might not, i don't know. I mainly went with the
			// branchless version because it feels correcter. My gut may be wrong though, so to determine it, I would
			// have to benchmark both versions.
			// TODO: Benchmark this function and compare it to a branched version.
			const bool check_x = (max(A.x, B.x) >= p.x) && (min(A.x, B.x) <= p.x);
			const bool check_y = (max(A.y, B.y) >= p.y) && (min(A.y, B.y) <= p.y);
			const bool check_z = (max(A.z, B.z) >= p.z) && (min(A.z, B.z) <= p.z);
			return check_x && check_y && check_z;
		}

		bool intersect_ray(ray r) const {
			// Easiest case
			const bool origin_in_box = point_in_box(r.origin);
			if (origin_in_box) {return true;}

			// Next easier case
			const vec3 midpoint = (A+B)*0.5;
			const vec3 vec_to_mid = midpoint - r.origin;
			const vec3 nearest_point = r.direction*(vec_to_mid^r.direction) + r.origin;
			const bool nearest_point_in_box = point_in_box(nearest_point);
			if (nearest_point_in_box) { return true; }

			// up to here that's 100 instructions
			// (div can take ~24 cycles)

			// Now onto the most expensive case

			// I made this entire monstrosity to avoid divisions. Having a look at it in compiler explorer makes it
			// obvious, that it is not worth it at all (like really not worth it).
			// TODO: Make a simpler plane intersection for axis aligned planes cause that will be way way way faster
			plane P{};
			vec3 poi{};
			scalar alpha;

			P.c = 0;
			gen2DPlaneTrough2Points(A.x, A.y, B.x, B.y, &P.a, &P.b, &P.d);
			if (ray_plane_intersection(r, P, &poi, &alpha)) {
				if (point_in_box(poi)) { return true; }
			}
			gen2DPlaneTrough2Points(A.x, B.y, B.x, A.y, &P.a, &P.b, &P.d);
			if (ray_plane_intersection(r, P, &poi, &alpha)) {
				if (point_in_box(poi)) { return true; }
			}

			P.a = 0;
			gen2DPlaneTrough2Points(A.y, A.z, B.y, B.z, &P.b, &P.c, &P.d);
			if (ray_plane_intersection(r, P, &poi, &alpha)) {
				if (point_in_box(poi)) { return true; }
			}
			gen2DPlaneTrough2Points(A.y, B.z, B.y, A.z, &P.b, &P.c, &P.d);
			if (ray_plane_intersection(r, P, &poi, &alpha)) {
				if (point_in_box(poi)) { return true; }
			}

			P.b = 0;
			gen2DPlaneTrough2Points(A.z, A.x, B.z, B.x, &P.c, &P.a, &P.d);
			if (ray_plane_intersection(r, P, &poi, &alpha)) {
				if (point_in_box(poi)) { return true; }
			}
			gen2DPlaneTrough2Points(A.z, B.x, B.z, A.x, &P.c, &P.a, &P.d);
			if (ray_plane_intersection(r, P, &poi, &alpha)) {
				if (point_in_box(poi)) { return true; }
			}



			// It appears like it didn't intersect
			return false;
		}
	};
}
#endif //RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H
