//
// Created by marc on 13/08/2021.
//

#include "Pathtracing.h"

namespace pathtracing {


	inline bool Pathtracer::combined_ray_intersect(ray r, vec3* poi, scalar* dist, vec3* normal, Material* material) const {

		vec3 poi_pol{};
		scalar dist_pol{};
		vec3 normal_pol{};

		polygon::Node* hit_Object;

		const bool did_hit_pol = polygon_root->intersect_ray(r, &poi_pol, &dist_pol, &normal_pol, &hit_Object);
		if (!did_hit_pol) dist_pol = INFINITY_S;


		vec3 poi_mar{};
		scalar dist_mar{};
		vec3 normal_mar{};

		raymarching::Node* hit_Object_mar;

		const bool did_hit_mar = raymarching_root->intersect_ray(r, dist_pol, &poi_mar, &dist_mar, &normal_mar, &hit_Object_mar);
		if (!did_hit_mar) dist_mar = INFINITY_S;


		if (!did_hit_pol && !did_hit_mar) return false;

		if (dist_pol < dist_mar) {
			*material = ((polygon::Object*)hit_Object)->mat; // UNSAFE!!!! (should be good tho)
			*poi = poi_pol;
			*dist = dist_pol;
			*normal = normal_pol;
		} else {
			*material = ((raymarching::Object*)hit_Object_mar)->mat; // UNSAFE!!!! (should be good tho)
			*poi = poi_mar;
			*dist = dist_mar;
			*normal = normal_mar;
		}

		return true;
	}

	vec3 Pathtracer::trace_path(ray r, int max_bounce_count, rng_engine* random_engine) {

		if (max_bounce_count <= 0) return get_ambient_color(r);

		vec3 poi{};
		scalar dist{};
		vec3 normal{};
		Material mat{};

		const bool did_hit = combined_ray_intersect(r, &poi, &dist, &normal, &mat);
		if (!did_hit) return get_ambient_color(r);
		//return (normal + vec3{1, 1, 1})*0.5;
		//return mat.base_color;

#ifdef STOP_BOUNCE_ON_EMISSION
		if (mat.emission > 0.5) return mat.base_color*mat.emission;
#endif

		const vec3 out_dir = reflect_rough(normal, r.direction, mat.roughness, random_engine);
		ray out_r = {poi + (normal*CLEARANCE_DIST), out_dir};
		vec3 incoming = trace_path(out_r, max_bounce_count - 1, random_engine);

#ifndef STOP_BOUNCE_ON_EMISSION
		incoming = incoming + mat.base_color*mat.emission;
#endif

		return {
				mat.base_color.x*incoming.x,
				mat.base_color.y*incoming.y,
				mat.base_color.z*incoming.z,
		};
	}


}