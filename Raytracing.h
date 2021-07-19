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

		bool intersect_ray(ray r) const;
	};


	struct ref_trigon { int index_A, index_B, index_C; };

	struct Mesh {
		vec3* verts;
		int number_of_verts;
		ref_trigon* tris;
		int number_of_tris;

		inline trigon get_derefed_tri(int index) const {
			const ref_trigon rt = tris[index];
			const vec3 A = verts[rt.index_A];
			const vec3 B = verts[rt.index_B];
			const vec3 C = verts[rt.index_C];
			return {A, B, C};
		}

		Box get_bounding_box() const;
		bool intersect_ray(ray r, vec3* poi, scalar* alpha) const;
	};

	struct Object {
		// I should do transformations here aswell, but I can't be bothered right now.
		Mesh mesh;
		Box bounding_box;

		inline void load_bounding_box() { bounding_box = mesh.get_bounding_box(); }
	};

	// TODO: Make these member functions of the repsective types.
	bool ray_plane_intersection(ray R, plane L, vec3 *poi, scalar *alpha);
	bool point_on_trigon(trigon tri, vec3 I, vec3* K);

}
#endif //RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H
