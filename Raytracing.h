//
// Created by marc on 11.07.21.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H
#define RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H

#include "Types.h"
namespace polygon {
	bool ray_plane_intersection(ray R, plane L, vec3 *poi, scalar *alpha);
	bool point_on_trigon(trigon tri, vec3 I, vec3* K);
}
#endif //RAYTRACING_RAYMARCHING_HYBRID_RAYTRACING_H
