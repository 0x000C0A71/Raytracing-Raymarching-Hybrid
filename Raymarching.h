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

		virtual Node* get_closest_Object(vec3 p) const { return nullptr; };

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
			return transform.apply(this->distance_function(transform.deapply(p)));
		}

		Node* get_closest_Object(vec3 p) const override { return (Node*) this; }
	};

	struct Merger : Node {
		Node** children{};
		int no_children{};
		//Transform transform{};

		Merger() = default;;

		Merger(int no_children, Node** children) : children(children), no_children(no_children) {};

		inline void build() override {
			for (int i = 0; i < no_children; i++) {
				children[i]->build();
			}
		}

		virtual scalar merger_operator(scalar a, scalar b) const { return min_S(a, b); }

		scalar distance_estimation(vec3 p) const override {
			scalar dist = children[0]->distance_estimation(p);
			for (int i = 1; i < no_children; i++) {
				dist = this->merger_operator(dist, children[i]->distance_estimation(p));
			}
			return dist;
		}

		Node* get_closest_Object(vec3 p) const override {
			scalar closest_dist = children[0]->distance_estimation(p);
			Node* closest_node = children[0];
			for (int i = 1; i < no_children; i++) {
				const scalar this_dist = children[i]->distance_estimation(p);
				if (this_dist < closest_dist) {
					closest_dist = this_dist;
					closest_node = children[i];
				}
			}
			return closest_node->get_closest_Object(p);
		}
	};
}

#endif //RAYTRACING_RAYMARCHING_HYBRID_RAYMARCHING_H
