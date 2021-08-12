//
// Created by marc on 07/07/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_TYPES_H
#define RAYTRACING_RAYMARCHING_HYBRID_TYPES_H

#include "fast_inverse_square_root.h"
#include <cmath> // TODO: Please remove this. I hate depending on libraries. It's my kryptonite


#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) > (b) ? (b) : (a))


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
				x*factor,
				y*factor,
				z*factor,
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
			(1 - t)*a.x + t*b.x,
			(1 - t)*a.y + t*b.y,
			(1 - t)*a.z + t*b.z,
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


struct rotator {
	scalar roll, pitch, yaw;
	// Disgusting gimbaled rotator!
	// TODO: shift to quaternions or something like that

	inline vec3 rotate(vec3 p) const {
		const vec3 i1 = {p.x, cos(roll)*p.y + sin(roll)*p.z, cos(roll)*p.z - sin(roll)*p.y }; // These trig functions are expensive
		const vec3 i2 = {cos(pitch)*i1.x + sin(pitch)*i1.z, i1.y, cos(pitch)*i1.z - sin(pitch)*i1.x }; // These trig functions are expensive
		const vec3 i3 = {cos(yaw)*i2.x + sin(yaw)*i2.y, cos(yaw)*i2.y - sin(yaw)*i2.x, i2.z }; // These trig functions are expensive
		return i3;
	}

	inline vec3 derotate(vec3 p) const {
		const vec3 i1 = {cos(yaw)*p.x + sin(-yaw)*p.y, cos(yaw)*p.y - sin(-yaw)*p.x, p.z }; // These trig functions are expensive
		const vec3 i2 = {cos(pitch)*i1.x + sin(-pitch)*i1.z, i1.y, cos(pitch)*i1.z - sin(-pitch)*i1.x }; // These trig functions are expensive
		const vec3 i3 = {i2.x, cos(roll)*i2.y + sin(-roll)*i2.z, cos(roll)*i2.z - sin(-roll)*i2.y }; // These trig functions are expensive
		return i3;
	}
};

struct transform {
	vec3 translation;
	rotator rotation;
	// vec3 scale;

	inline vec3 apply(vec3 p) const {
		const vec3 r = rotation.rotate(p);
		const vec3 t = r + translation;
		return t;
	}

	inline vec3 deapply(vec3 p) const {
		const vec3 t = p - translation;
		const vec3 r = rotation.derotate(t);
		return r;
	}

	inline ray apply(ray r) const {
		r.origin = apply(r.origin);
		r.direction = rotation.rotate(r.direction);
		return r;
	}

	inline ray deapply(ray r) const {
		r.origin = deapply(r.origin);
		r.direction = rotation.derotate(r.direction);
		return r;
	}
};

#endif //RAYTRACING_RAYMARCHING_HYBRID_TYPES_H
