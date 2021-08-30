//
// Created by marc on 20/08/2021.
//

#include "Raymarching.h"


namespace raymarching {

	vec3 Node::get_normal_at(vec3 p) const {
		const scalar dist = this->distance_estimation(p);
		const vec3 normal = {
				dist - this->distance_estimation({p.x - RAYMARCHING_NORMAL_OFFSET, p.y, p.z}),
				dist - this->distance_estimation({p.x, p.y - RAYMARCHING_NORMAL_OFFSET, p.z}),
				dist - this->distance_estimation({p.x, p.y, p.z - RAYMARCHING_NORMAL_OFFSET})
		};

		return normalize(normal); // This hurts
	}

	bool Node::intersect_ray(ray r, scalar max_dist, vec3* poi, scalar* alpha, vec3* normal, Node** intersected_object) const {

		scalar total_dist = 0;
		vec3 p = r.origin;
		for (int i = 0; i < RAYMARCHING_MAX_STEPS; i++) {

			const scalar dist = this->distance_estimation(p);
			total_dist += dist;
			if (total_dist > max_dist) return false;

			// Successful hit!
			if (dist <= RAYMARCHING_MIN_DIST) { // TODO: this could be signed (as in negative still walk (great english))
				*poi = p;
				*alpha = total_dist;
				*normal = get_normal_at(p);
				*intersected_object = this->get_closest_Object(p);
				return true;
			}

			p = p + r.direction*dist;
		}

		return false;
	}

}
