//
// Created by marc on 20/08/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_RAYMARCHING_H
#define RAYTRACING_RAYMARCHING_HYBRID_RAYMARCHING_H

#include "Types.h"
#include "Config.h"

namespace raymarching {
	struct Node {
		virtual inline void build() {}

		virtual scalar distance_estimation(vec3 p) const { return INFINITY_S; }

		virtual Node* get_closest_Object(vec3 p) const {};

		vec3 get_normal_at(vec3 p) const;

		bool intersect_ray(ray r, scalar max_dist, vec3* poi, scalar* alpha, vec3* normal, Node** intersected_object) const;
	};

	struct Object : Node {
		Transform transform{};

		Material mat{};

		Object(Transform transform, Material mat) : transform(transform), mat(mat) {}

		Object() = default;

		inline void build() override {
			transform.build();
		}

		virtual scalar distance_function(vec3 p) const { return INFINITY_S; } // To be overridden

		scalar distance_estimation(vec3 p) const override {
			return this->distance_function(transform.deapply(p));
		}

		Node* get_closest_Object(vec3 p) const override { return (Node*) this; }
	};

}

#endif //RAYTRACING_RAYMARCHING_HYBRID_RAYMARCHING_H
