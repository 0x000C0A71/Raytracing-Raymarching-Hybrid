//
// Created by marc on 13/08/2021.
//

#include "Pathtracing.h"

namespace pathtracing {


	inline bool
	Pathtracer::combined_ray_intersect(ray r, vec3* poi, scalar* dist, vec3* normal, Material* material) const {

		vec3 poi_pol{};
		scalar dist_pol{};
		vec3 normal_pol{};

		polygon::Node* hit_Object;

		const bool did_hit_pol = polygon_root->intersect_ray(r, &poi_pol, &dist_pol, &normal_pol, &hit_Object);

		if (!did_hit_pol) return false;

		*material = ((polygon::Object*) hit_Object)->mat; // UNSAFE!!!! (should be good tho)

		*poi = poi_pol;
		*dist = dist_pol;
		*normal = normal_pol;
		return did_hit_pol;
	}

	vec3 Pathtracer::trace_path(ray r, int max_bounce_count) const {

		if (max_bounce_count <= 0) return get_ambient_color(r);

		vec3 poi{};
		scalar dist{};
		vec3 normal{};
		Material mat{};

		const bool did_hit = combined_ray_intersect(r, &poi, &dist, &normal, &mat);
		if (!did_hit) return get_ambient_color(r);


		if (mat.emission > 0.5) return mat.base_color*mat.emission;

		const vec3 out_dir = reflect_rough(normal, r.direction, mat.roughness);
		ray out_r = {poi + (normal*CLEARANCE_DIST), out_dir};
		vec3 incoming = trace_path(out_r, max_bounce_count - 1);

		return {
				mat.base_color.x*incoming.x,
				mat.base_color.y*incoming.y,
				mat.base_color.z*incoming.z,
		};
	}


}