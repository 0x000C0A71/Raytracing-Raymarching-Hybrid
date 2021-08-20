//
// Created by marc on 11.07.21.
//

#include "Raytracing.h"


namespace polygon {
	bool ray_plane_intersection(ray R, plane L, vec3* poi, scalar* alpha) {
		const scalar a = L.a;
		const scalar b = L.b;
		const scalar c = L.c;
		const scalar d = L.d;
		const vec3 C = R.origin;
		const vec3 P = R.direction;

		const scalar N = a*P.x + b*P.y + c*P.z;

		if (N == 0) return false;

		const scalar Z = a*C.x + b*C.y + c*C.z + d;

		const scalar t = -(Z/N); // Painful division
		// TODO: make sure this painful division is really necessary

		const vec3 i = C + (P*t);

		*poi = i;
		*alpha = t;
		return true;
	}

	bool point_on_trigon(trigon tri, vec3 I, vec3* K) {
		const vec3 Normal = tri.get_normal(); // TODO: this can be used as an input and we can save 1 call to this function like that.
		const vec3 aNormal = (tri.B - tri.C)*Normal;
		const vec3 bNormal = (tri.C - tri.A)*Normal;
		const vec3 cNormal = (tri.A - tri.B)*Normal;

		const vec3 vai = I - tri.A;
		const vec3 vbi = I - tri.B;

		const scalar aDist = (vbi^aNormal)*Q_rsqrt(sqLength(aNormal));
		const scalar bDist = (vai^bNormal)*Q_rsqrt(sqLength(bNormal));
		const scalar cDist = (vai^cNormal)*Q_rsqrt(sqLength(cNormal));

		if ((aDist < 0) || (bDist < 0) || (cDist < 0)) return false;

		const scalar total = aDist + bDist + cDist;

		if (total == 0) return false;

		const scalar fac = 1/total; // performance reasons
		K->x = aDist*fac;
		K->y = bDist*fac;
		K->z = cDist*fac;
		return true;
	}

	bool Box::intersect_ray(ray r) const {
		// Easiest case
		const bool origin_in_box = point_in_box(r.origin);
		if (origin_in_box) return true;

		// Next easier case
		const vec3 midpoint = (A + B)*0.5;
		const vec3 vec_to_mid = midpoint - r.origin;
		const vec3 nearest_point = r.direction*(vec_to_mid^r.direction) + r.origin;
		const bool nearest_point_in_box = point_in_box(nearest_point);
		if (nearest_point_in_box) { return true; }

		// Now onto the most expensive case
		const scalar x1 = A.x;
		const scalar y1 = A.y;
		const scalar z1 = A.z;
		const scalar x2 = B.x;
		const scalar y2 = B.y;
		const scalar z2 = B.z;

		const scalar dx1 = x1 - r.origin.x;
		const scalar dy1 = y1 - r.origin.y;
		const scalar dz1 = z1 - r.origin.z;
		const scalar dx2 = x2 - r.origin.x;
		const scalar dy2 = y2 - r.origin.y;
		const scalar dz2 = z2 - r.origin.z;

		const scalar fx1 = dx1/r.direction.x; // might be inf
		const scalar fy1 = dy1/r.direction.y; // might be inf
		const scalar fz1 = dz1/r.direction.z; // might be inf
		const scalar fx2 = dx2/r.direction.x; // might be inf
		const scalar fy2 = dy2/r.direction.y; // might be inf
		const scalar fz2 = dz2/r.direction.z; // might be inf

		if ((fx1 > 0) && (fx1 < INFINITY_S)) {
			const vec3 point = r.origin + (r.direction*fx1);
			const bool check_1 = (max(A.y, B.y) >= point.y) && (min(A.y, B.y) <= point.y);
			const bool check_2 = (max(A.z, B.z) >= point.z) && (min(A.z, B.z) <= point.z);
			if (check_1 && check_2) { return true; }
		}
		if ((fy1 > 0) && (fy1 < INFINITY_S)) {
			const vec3 point = r.origin + (r.direction*fy1);
			const bool check_1 = (max(A.z, B.z) >= point.z) && (min(A.z, B.z) <= point.z);
			const bool check_2 = (max(A.x, B.x) >= point.x) && (min(A.x, B.x) <= point.x);
			if (check_1 && check_2) { return true; }
		}
		if ((fz1 > 0) && (fz1 < INFINITY_S)) {
			const vec3 point = r.origin + (r.direction*fz1);
			const bool check_1 = (max(A.x, B.x) >= point.x) && (min(A.x, B.x) <= point.x);
			const bool check_2 = (max(A.y, B.y) >= point.y) && (min(A.y, B.y) <= point.y);
			if (check_1 && check_2) { return true; }
		}
		if ((fx2 > 0) && (fx2 < INFINITY_S)) {
			const vec3 point = r.origin + (r.direction*fx2);
			const bool check_1 = (max(A.y, B.y) >= point.y) && (min(A.y, B.y) <= point.y);
			const bool check_2 = (max(A.z, B.z) >= point.z) && (min(A.z, B.z) <= point.z);
			if (check_1 && check_2) { return true; }
		}
		if ((fy2 > 0) && (fy2 < INFINITY_S)) {
			const vec3 point = r.origin + (r.direction*fy2);
			const bool check_1 = (max(A.z, B.z) >= point.z) && (min(A.z, B.z) <= point.z);
			const bool check_2 = (max(A.x, B.x) >= point.x) && (min(A.x, B.x) <= point.x);
			if (check_1 && check_2) { return true; }
		}
		if ((fz2 > 0) && (fz2 < INFINITY_S)) {
			const vec3 point = r.origin + (r.direction*fz2);
			const bool check_1 = (max(A.x, B.x) >= point.x) && (min(A.x, B.x) <= point.x);
			const bool check_2 = (max(A.y, B.y) >= point.y) && (min(A.y, B.y) <= point.y);
			if (check_1 && check_2) { return true; }
		}



		// It appears like it didn't intersect
		return false;
	}

	Box Mesh::get_bounding_box() const {
		scalar x1, x2;
		scalar y1, y2;
		scalar z1, z2;
		for (int i = 0; i < number_of_verts; i++) {
			const vec3 vert = verts[i];
			x1 = min(x1, vert.x);
			y1 = min(y1, vert.y);
			z1 = min(z1, vert.z);
			x2 = max(x2, vert.x);
			y2 = max(y2, vert.y);
			z2 = max(z2, vert.z);
		}
		return {{x1, y1, z1},
		        {x2, y2, z2}};
	}

	bool Mesh::intersect_ray(ray r, vec3* poi, scalar* alpha, int* trigon_index, vec3* normal) const {
		// Ouch!!

		bool did_hit = false;

		vec3 best_point{};
		scalar best_alpha = INFINITY;
		vec3 best_normal{};
		int best_index;

		for (int current_index = 0; current_index < number_of_tris; current_index++) {
			// Very expensive for loop

			const trigon tri = get_derefed_tri(current_index);


			vec3 current_point{};
			scalar current_alpha;
			vec3 throwaway_K{};

			const vec3 current_normal = tri.get_normal();

#ifdef DO_BACKFACE_CULLING
			if ((current_normal^r.direction) > 0) continue;
#endif

			// TODO:                       vvvvvvvvvvvvvvv----- This could be precomputed and cached on the trigon
			if (!ray_plane_intersection(r, tri.get_plane(), &current_point, &current_alpha)) continue; // TODO: Cache the plane
			if (!point_on_trigon(tri, current_point, &throwaway_K)) continue;

			did_hit = true;

			if (current_alpha < best_alpha) {
				best_point = current_point;
				best_alpha = current_alpha;
				best_normal = current_normal;
				best_index = current_index;
			}
		}

		*poi = best_point;
		*alpha = best_alpha;
		*normal = best_normal;
		*trigon_index = best_index;

		return did_hit;
	}

	bool Object::intersect_ray(ray r, vec3* poi, scalar* alpha, vec3* normal, Node** intersected_object) const {
		r = transform.deapply(r);

		const bool did_hit_box = bounding_box.intersect_ray(r);
		if (!did_hit_box) return false;

		vec3 loc_normal{};
		int trigon_index;
		const bool did_hit_mesh = mesh.intersect_ray(r, poi, alpha, &trigon_index, &loc_normal);

		*poi = transform.apply(*poi);
		*normal = transform.rotation.rotate(loc_normal);
		*intersected_object = (Node*) this;

		return did_hit_mesh;
	}

	bool Group::intersect_ray(ray r, vec3* poi, scalar* alpha, vec3* normal, Node** intersected_object) const {

		bool did_hit = false;

		vec3 best_point{};
		scalar best_alpha = INFINITY;
		vec3 best_normal{};
		Node* best_Object{};

		for (int i = 0; i < no_children; i++) {
			vec3 current_point{};
			scalar current_alpha;
			vec3 current_normal{};
			Node* current_Object{};

			if (!children[i]->intersect_ray(r, &current_point, &current_alpha, &current_normal, &current_Object)) continue;

			did_hit = true;

			if (current_alpha < best_alpha) {
				best_point = current_point;
				best_alpha = current_alpha;
				best_normal = current_normal;
				best_Object = current_Object;
			}
		}

		*poi = best_point;
		*alpha = best_alpha;
		*normal = best_normal;
		*intersected_object = best_Object;

		return did_hit;
	}
}