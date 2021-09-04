//
// Created by marc on 13/08/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_PATHTRACING_H
#define RAYTRACING_RAYMARCHING_HYBRID_PATHTRACING_H

#include "Types.h"
#include "Raytracing.h"
#include "Raymarching.h"
#include "Raymarching_primitives.h"

namespace pathtracing {


	inline vec3 reflect_diffuse(vec3 normal, vec3 vector, scalar roughness, rng_engine* random_engine) {
		vec3 rand_vec = random_normal(random_engine);
		const vec3 ref_vec = reflect(vector, normal);
		//if ((rand_vec^normal) < 0) rand_vec = rand_vec*(-1);
		// Apparently uncommenting  this line makes it more physically accurate, but looking at the results, I think it looks more
		// physically accurate like this (I am lacking the BSDF)
		//                           vvvvvvvvv---- This would also have to be taken out
		return lerp(ref_vec, rand_vec + normal, roughness);
	}

	inline vec3 reflect_metallic(vec3 normal, vec3 vector, scalar roughness, rng_engine* random_engine) {
		vec3 rand_vec = random_normal(random_engine);
		const vec3 ref_vec = reflect(vector, normal);
		const vec3 out_dir = ref_vec + rand_vec*glm::dot(normal, vector)*roughness;
		return out_dir;
	}


	struct Pathtracer {
		polygon::Node* polygon_root;
		raymarching::Node* raymarching_root;

		inline bool combined_ray_intersect(ray r, vec3* poi, scalar* dist, vec3* normal, Material* material) const;

		vec3 trace_path(ray r, int max_bounce_count, rng_engine* random_engine);

		static inline vec3 get_ambient_color(ray r) {
			// If you want stuff like HDRIs, implement them here
			return (vec3{0.7411764706, 0.8509803922, 1}*(0.2/1.7303841221));
			//return {0.05, 0.05, 0.05};
		}

		inline void build() const {
			polygon_root->build();
			raymarching_root->build();
		}
	};
}

#endif //RAYTRACING_RAYMARCHING_HYBRID_PATHTRACING_H
