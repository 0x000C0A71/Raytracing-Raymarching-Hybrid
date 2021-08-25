//
// Created by marc on 11.07.21.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H
#define RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H

#include "Types.h"
#include "Config.h"
#include <cstdio>


namespace polygon {
	struct plane {
		// A plane is defined as follows: ax + by + cz + d = 0
		scalar a, b, c, d;
	};

	struct trigon {
		// A, B & C are the 3 corners of the trigon
		vec3 A, B, C;

		// Returns the normal of the trigon.
		inline vec3 get_normal() const { return normalize((B - A)*(C - A)); }

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

		inline Box operator+(Box o) const {
			const scalar x1 = min(A.x, o.A.x);
			const scalar x2 = max(B.x, o.B.x);
			const scalar y1 = min(A.y, o.A.y);
			const scalar y2 = max(B.y, o.B.y);
			const scalar z1 = min(A.z, o.A.z);
			const scalar z2 = max(B.z, o.B.z);
			return {
					{x1, y1, z1},
					{x2, y2, z2}
			};
		}

		bool intersect_ray(ray r) const;
	};


	struct ref_trigon {
		int index_A, index_B, index_C;
	};

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

		bool intersect_ray(ray r, vec3* poi, scalar* alpha, int* trigon_index, vec3* normal) const;
	};

	struct Node {
		virtual inline void build() {}

		virtual bool intersect_ray(ray r, vec3* poi, scalar* alpha, vec3* normal, Node** intersected_object) const {return false;}
	};

	struct Object : Node {
		Mesh mesh;
		Box bounding_box{};

		Transform transform{};

		Material mat{};

		// Object(Mesh mesh):mesh(mesh){};
		Object(Mesh mesh, Transform transform) : mesh(mesh), transform(transform) {};

		inline void build() override {
			transform.build();
			bounding_box = mesh.get_bounding_box();
		}

		bool intersect_ray(ray r, vec3* poi, scalar* alpha, vec3* normal, Node** intersected_object) const override;
	};

	struct Group : Node {
		Node** children{};
		int no_children{};

		Group() = default;;

		Group(int no_children, Node** children) : children(children), no_children(no_children) {};

		inline void build() override {

			for (int i = 0; i < no_children; i++) {
				children[i]->build();
			}
		}

		bool intersect_ray(ray r, vec3* poi, scalar* alpha, vec3* normal, Node** intersected_object) const override;
	};

	// TODO: Make these member functions of the respective types.
	bool ray_plane_intersection(ray R, plane L, vec3* poi, scalar* alpha);

	bool point_on_trigon(trigon tri, vec3 I, vec3* K);

}
#endif //RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H
