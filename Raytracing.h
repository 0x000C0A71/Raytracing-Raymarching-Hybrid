//
// Created by marc on 11.07.21.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H
#define RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H

#include "Types.h"
#include <cmath>

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

			// Now onto the most expensive case
			const scalar x1 = A.x;
			const scalar y1 = A.y;
			const scalar z1 = A.z;
			const scalar x2 = B.x;
			const scalar y2 = B.y;
			const scalar z2 = B.z;

			const scalar dx1 = x1 - r.origin.x;
			const scalar dy1 = y1 - r.origin.y;
			const scalar dz1 = z1 - r.origin.z;
			const scalar dx2 = x2 - r.origin.x;
			const scalar dy2 = y2 - r.origin.y;
			const scalar dz2 = z2 - r.origin.z;

			const scalar fx1 = dx1/r.direction.x; // might be inf
			const scalar fy1 = dy1/r.direction.y; // might be inf
			const scalar fz1 = dz1/r.direction.z; // might be inf
			const scalar fx2 = dx2/r.direction.x; // might be inf
			const scalar fy2 = dy2/r.direction.y; // might be inf
			const scalar fz2 = dz2/r.direction.z; // might be inf

			if ((fx1 > 0) && (fx1 < INFINITY)) {
				const vec3 point = r.origin + (r.direction*fx1);
				const bool check_1 = (max(A.y, B.y) >= point.y) && (min(A.y, B.y) <= point.y);
				const bool check_2 = (max(A.z, B.z) >= point.z) && (min(A.z, B.z) <= point.z);
				if (check_1 && check_2) { return true; }
			}
			if ((fy1 > 0) && (fy1 < INFINITY)) {
				const vec3 point = r.origin + (r.direction*fy1);
				const bool check_1 = (max(A.z, B.z) >= point.z) && (min(A.z, B.z) <= point.z);
				const bool check_2 = (max(A.x, B.x) >= point.x) && (min(A.x, B.x) <= point.x);
				if (check_1 && check_2) { return true; }
			}
			if ((fz1 > 0) && (fz1 < INFINITY)) {
				const vec3 point = r.origin + (r.direction*fz1);
				const bool check_1 = (max(A.x, B.x) >= point.x) && (min(A.x, B.x) <= point.x);
				const bool check_2 = (max(A.y, B.y) >= point.y) && (min(A.y, B.y) <= point.y);
				if (check_1 && check_2) { return true; }
			}
			if ((fx2 > 0) && (fx2 < INFINITY)) {
				const vec3 point = r.origin + (r.direction*fx2);
				const bool check_1 = (max(A.y, B.y) >= point.y) && (min(A.y, B.y) <= point.y);
				const bool check_2 = (max(A.z, B.z) >= point.z) && (min(A.z, B.z) <= point.z);
				if (check_1 && check_2) { return true; }
			}
			if ((fy2 > 0) && (fy2 < INFINITY)) {
				const vec3 point = r.origin + (r.direction*fy2);
				const bool check_1 = (max(A.z, B.z) >= point.z) && (min(A.z, B.z) <= point.z);
				const bool check_2 = (max(A.x, B.x) >= point.x) && (min(A.x, B.x) <= point.x);
				if (check_1 && check_2) { return true; }
			}
			if ((fz2 > 0) && (fz2 < INFINITY)) {
				const vec3 point = r.origin + (r.direction*fz2);
				const bool check_1 = (max(A.x, B.x) >= point.x) && (min(A.x, B.x) <= point.x);
				const bool check_2 = (max(A.y, B.y) >= point.y) && (min(A.y, B.y) <= point.y);
				if (check_1 && check_2) { return true; }
			}



			// It appears like it didn't intersect
			return false;
		}
	};


	bool ray_plane_intersection(ray R, plane L, vec3 *poi, scalar *alpha);
	bool point_on_trigon(trigon tri, vec3 I, vec3* K);

}
#endif //RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H
