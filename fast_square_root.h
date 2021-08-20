//
// Created by marc on 20/08/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_FAST_SQUARE_ROOT_H
#define RAYTRACING_RAYMARCHING_HYBRID_FAST_SQUARE_ROOT_H

#include "Types.h"
#include "fast_inverse_square_root.h"

// TODO: Get a better function here
inline scalar fsqrt(scalar x) {
	return 1/Q_rsqrt(x); // TODO: Very bad!
}

#endif //RAYTRACING_RAYMARCHING_HYBRID_FAST_SQUARE_ROOT_H
