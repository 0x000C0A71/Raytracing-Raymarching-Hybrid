//
// Created by xerne on 07/07/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_TYPES_H
#define RAYTRACING_RAYMARCHING_HYBRID_TYPES_H

#include "fast_inverse_square_root.h"



//   ____            _        _______
//  |  _ \          (_)      |__   __|
//  | |_) | __ _ ___ _  ___     | |_   _ _ __   ___  ___
//  |  _ < / _` / __| |/ __|    | | | | | '_ \ / _ \/ __|
//  | |_) | (_| \__ \ | (__     | | |_| | |_) |  __/\__ \
//  |____/ \__,_|___/_|\___|    |_|\__, | .__/ \___||___/
//                                  __/ | |
//                                 |___/|_|





// Changing this changes the type for the scalar.
// So you can change this to double for more accuracy
typedef double scalar;

// 3D vectors
struct vec3 {
	scalar x, y, z;

	inline vec3 operator+(vec3 other) const {
		return {
			x + other.x,
			y + other.y,
			z + other.z,
		};
	}

	inline vec3 operator-(vec3 other) const {
		return {
				x - other.x,
				y - other.y,
				z - other.z,
		};
	}

	// Scaling a vector by a scalar
	inline vec3 operator*(scalar factor) const {
		return {
				x * factor,
				y * factor,
				z * factor,
		};
	}

	// The cross product. It might be a bit confusing to overload it on the multiply operator, but I think it's apt,
	// since multiplication is sometimes written with an X.
	inline vec3 operator*(vec3 other) const {
		return {
				y*other.z - z*other.y,
				z*other.x - x*other.z,
				x*other.y - y*other.x,
		};
	}

	// The Dot product. Again overloading it over the xor operator might be a bit confusing. As opposed to the
	// cross product though, I do not have a reason for it other than having it on an operator makes the code a bit
	// tidier.
	inline scalar operator^(vec3 other) const {
		return x*other.x + y*other.y + z*other.z;
	}
};

// Linearly interpolates between vector a & b.
inline vec3 lerp(vec3 a, vec3 b, scalar t) {
	return {
		(1-t)*a.x + t*b.x,
		(1-t)*a.y + t*b.y,
		(1-t)*a.z + t*b.z,
	};
}

// Return the squared length of a given vector.
inline scalar sqLength(vec3 v) {
	return v.x*v.x + v.y*v.y + v.z*v.z;
}

inline vec3 normalize(vec3 v) {
	const scalar sqLen = sqLength(v);
	const scalar factor = Q_rsqrt(sqLen);
	return v*factor;
}

inline vec3 reflect(vec3 v, vec3 n) {
	return v - (n*(2*(v^n)));
}

struct ray {
	vec3 origin;
	vec3 direction;
};

//   _____      _
//  |  __ \    | |
//  | |__) |__ | |_   _  __ _  ___  _ __
//  |  ___/ _ \| | | | |/ _` |/ _ \| '_ \
//  | |  | (_) | | |_| | (_| | (_) | | | |
//  |_|   \___/|_|\__, |\__, |\___/|_| |_|
//                 __/ | __/ |
//                |___/ |___/

// TODO: Think about whether or not to split this entire section into a separate file.

namespace polygon {
	struct plane {
		// A plane is defined as follows: ax + by + cz + d = 0
		scalar a, b, c, d;
	};

	struct trigon {
		// A, B & C are the 3 corners of the trigon
		vec3 A, B, C;

		// Returns the normal of the trigon.
		inline vec3 get_normal() { return normalize((B-A)*(C-A)); }

		// Generates the plane coefficients.
		inline plane get_plane(vec3 normal) {
			const scalar a = normal.x;
			const scalar b = normal.y;
			const scalar c = normal.z;
			const scalar d = (-1)*(a*A.x + b*A.y + c*A.z);
			return {a, b, c, d};
		}
		inline plane get_plane() { return get_plane(get_normal()); }
	};
}

#endif //RAYTRACING_RAYMARCHING_HYBRID_TYPES_H
