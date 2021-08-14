//
// Created by marc on 13/08/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_PATHTRACING_H
#define RAYTRACING_RAYMARCHING_HYBRID_PATHTRACING_H

#include "Types.h"
#include "Raytracing.h"

namespace pathtracing {


	inline vec3 reflect_rough(vec3 normal, vec3 vector, scalar roughness) {
		vec3 rand_vec = random_normal();
		const vec3 ref_vec = reflect(vector, normal);
		// TODO:              v-- This is *theoretically* the wrong way around. It seems to work, but investigate into it.
		if ((rand_vec^normal) > 0) rand_vec = rand_vec*(-1);

		return normalize(lerp(ref_vec, rand_vec, roughness));
	}


	struct Pathtracer {
		polygon::Node* polygon_root;

		inline bool combined_ray_intersect(ray r, vec3* poi, scalar* dist, vec3* normal, Material* material) const;

		vec3 trace_path(ray r, int max_bounce_count) const;

		static inline vec3 get_ambient_color(ray r) {
			// If you want stuff like HDRIs, implement them here
			return {0.02, 0.02, 0.02};
		}
	};
}

#endif //RAYTRACING_RAYMARCHING_HYBRID_PATHTRACING_H
