//
// Created by marc on 13/08/2021.
//

#include "Pathtracing.h"

namespace pathtracing {


	inline bool Pathtracer::combined_ray_intersect(ray r, vec3* poi, scalar* dist, vec3* normal, Material* material) const {

		vec3 poi_pol{};
		scalar dist_pol{};
		vec3 K_pol{};

		polygon::Node* hit_Object;

		const bool did_hit_pol = polygon_root->intersect_ray(r, &poi_pol, &dist_pol, &hit_Object);

		if (!did_hit_pol) return false;

		*material = ((polygon::Object*) hit_Object)->mat; // UNSAFE!!!! (should be good tho)

		*poi = poi_pol;
		*dist = dist_pol;
		*normal = K_pol;
		return did_hit_pol;
	}

	vec3 Pathtracer::trace_path(ray r, int max_bounce_count) const {

		vec3 poi{};
		scalar dist{};
		vec3 normal{};
		Material mat{};

		const bool did_hit = combined_ray_intersect(r, &poi, &dist, &normal, &mat);
		if (!did_hit) return {0, 0, 0}; // TODO: This is the ambient light. Make that changeable (this would also be the spot to implement HDRIs)


		return mat.base_color;
	}
}