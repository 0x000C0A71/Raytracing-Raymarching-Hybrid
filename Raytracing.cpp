//
// Created by marc on 11.07.21.
//

#include "Raytracing.h"

namespace polygon {
	bool ray_plane_intersection(ray R, plane L, vec3 *poi, scalar *alpha) {
		const scalar a = L.a;
		const scalar b = L.b;
		const scalar c = L.c;
		const scalar d = L.d;
		const vec3   C = R.origin;
		const vec3   P = R.direction;

		const scalar N = a*P.x + b*P.y + c*P.z;

		if (N == 0) return false;

		const scalar Z = a*C.x + b*C.y + c*C.z + d;

		const scalar t = -(Z / N); // Painful division
		// TODO: make sure this painful division is really necesarry

		const vec3 i = C + (P*t);

		*poi = i;
		*alpha = t;
		return true;
	}

	bool point_on_trigon(trigon tri, vec3 I, vec3* K) {
		const vec3  Normal = tri.get_normal(); // TODO: this can be used as an input and we can save 1 call to this function like that.
		const vec3 aNormal = (tri.B - tri.C)*Normal;
		const vec3 bNormal = (tri.C - tri.A)*Normal;
		const vec3 cNormal = (tri.A - tri.B)*Normal;

		const vec3 vai = I - tri.A;
		const vec3 vbi = I - tri.B;

		const scalar aDist = (vbi^aNormal) * Q_rsqrt(sqLength(aNormal));
		const scalar bDist = (vai^bNormal) * Q_rsqrt(sqLength(bNormal));
		const scalar cDist = (vai^cNormal) * Q_rsqrt(sqLength(cNormal));

		if ((aDist < 0) || (bDist < 0) || (cDist < 0)) return false;

		const scalar total = aDist + bDist + cDist;

		if (total == 0) return false;

		const scalar fac = 1/total; // performance reasons
		K->x = aDist*fac;
		K->y = bDist*fac;
		K->z = cDist*fac;
		return true;
	}
}