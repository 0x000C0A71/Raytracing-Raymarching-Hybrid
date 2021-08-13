//
// Created by marc on 13/08/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_PATHTRACING_H
#define RAYTRACING_RAYMARCHING_HYBRID_PATHTRACING_H

#include "Types.h"
#include "Raytracing.h"

namespace pathtracing {

	struct Pathtracer {
		polygon::Node* polygon_root;

		inline bool combined_ray_intersect(ray r, vec3* poi, scalar* dist, vec3* normal, Material* material) const;

		vec3 trace_path(ray r, int max_bounce_count) const;
	};
}

#endif //RAYTRACING_RAYMARCHING_HYBRID_PATHTRACING_H
