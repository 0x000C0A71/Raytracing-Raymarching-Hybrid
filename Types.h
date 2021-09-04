//
// Created by marc on 07/07/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_TYPES_H
#define RAYTRACING_RAYMARCHING_HYBRID_TYPES_H

#include "fast_inverse_square_root.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
//#include <random>


#define max_S(a, b) ((a) > (b) ? (a) : (b))
#define min_S(a, b) ((a) > (b) ? (b) : (a))


// Changing this changes the type for the scalar.
// So you can change this to double for more accuracy
typedef double scalar;
typedef glm::dvec3 vec3;
typedef glm::dmat3x3 mat3;


// Infinities
inline double inf(double probe) {
	const unsigned long long k = 0x7FF0000000000000;
	return *(double*)(&k);
}

inline float inf(float probe) {
	const unsigned long int k = 0x7F800000;
	return *(float*)(&k);
}

#define INFINITY_S (inf((scalar)1))


// Linearly interpolates between vector a & b.
// TODO: make more efficient
inline vec3 lerp(vec3 a, vec3 b, scalar t) {
	return a*(1 - t) + b*t;
}


inline vec3 normalize(vec3 v) {
	const scalar sqLen = glm::length2(v);
	const scalar factor = Q_rsqrt(sqLen);
	return v*factor;
}

inline vec3 reflect(vec3 v, vec3 n) {
	return v - (n*(2*glm::dot(v, n)));
}


struct xorshf96 {
	unsigned long x = 123456789, y = 362436069, z = 521288629;

	inline unsigned long xorshf96_() {          //period 2^96-1
		unsigned long t;
		x ^= x << 16;
		x ^= x >> 5;
		x ^= x << 1;

		t = x;
		x = y;
		y = z;
		z = t^x^y;

		return z;
	}

	static inline float truncate(float b) {
		unsigned long a = *(unsigned long*)(&b);
		a &= 0x7FFFFF;
		a |= 0x3F800000;
		return *(float*)(&a) - 1;
	}

	static inline double truncate(double b) {
		unsigned long long a = *(unsigned long long*)(&b);
		a &= 0xFFFFFFFFFFFFF;
		a |= 0x3FF0000000000000;
		return *(double*)(&a) - 1;
	}

	inline scalar draw_alpha() {
		const unsigned long raw = xorshf96_();
		return truncate(*(float*)(&raw));
		// TODO:          ^^^^^----- This should be 'scalar', but it doesn't work with doubles. Find out why.
	}

	inline scalar draw() {
		return draw_alpha()*2 - 1;
	}
};

/*
struct mersenne_twister {
	std::mt19937 eng;


	inline scalar draw() {
		std::uniform_real_distribution<scalar> distribution(-1.0, 1.0);
		return distribution(eng);
	}

};
 */

typedef xorshf96 rng_engine;


inline vec3 random_normal(rng_engine* engine) {
	const scalar x = engine->draw();
	const scalar y = engine->draw();
	const scalar z = engine->draw();
	vec3 v = vec3{x, y, z};
	while (glm::length2(v) > 1) {
		v.x = v.y;
		v.y = v.z;
		v.z = engine->draw();
	}
	// The line below would normalize the vector, but because that is not needed where I use the function, I commented it out.
	//return normalize(v);
	return v;
}

struct ray {
	vec3 origin;
	vec3 direction;
};


struct rotator {
	scalar roll, pitch, yaw;
	// Disgusting gimbaled rotator!
	// TODO: shift to quaternions or something like that

	mat3 f_mat, b_mat; // Matrices that do the forward and backward rotations

	inline void generate_matrices() {
		// Doing the painful trig functions first and caching them in a rotation matrix

		const scalar sin_r = glm::sin(roll);
		const scalar cos_r = glm::cos(roll);
		const scalar sin_p = glm::sin(pitch);
		const scalar cos_p = glm::cos(pitch);
		const scalar sin_y = glm::sin(yaw);
		const scalar cos_y = glm::cos(yaw);
		// TODO: use different trig functions, as these are not type agnostic (plus I hate having to use a library).

		// forward matrices
		const mat3 roll_f = {
				{1, 0,      0},
				{0, cos_r,  sin_r},
				{0, -sin_r, cos_r}
		};

		const mat3 pitch_f = {
				{cos_p, 0, -sin_p},
				{0,     1, 0},
				{sin_p, 0, cos_p}
		};

		const mat3 yaw_f = {
				{cos_y,  sin_y, 0},
				{-sin_y, cos_y, 0},
				{0,      0,     1}
		};

		f_mat = yaw_f*pitch_f*roll_f;

		// backward matrices
		const mat3 roll_b = {
				{1, 0,     0},
				{0, cos_r, -sin_r},
				{0, sin_r, cos_r}
		};

		const mat3 pitch_b = {
				{cos_p,  0, sin_p},
				{0,      1, 0},
				{-sin_p, 0, cos_p}
		};

		const mat3 yaw_b = {
				{cos_y, -sin_y, 0},
				{sin_y, cos_y,  0},
				{0,     0,      1}
		};

		b_mat = roll_b*pitch_b*yaw_b;

	}

	inline vec3 rotate(vec3 p) const {
		return f_mat*p;
	}

	inline vec3 derotate(vec3 p) const {
		return b_mat*p;
	}
};

struct Transform {
	vec3 translation;
	rotator rotation;
	// vec3 scale;

	inline void build() {
		rotation.generate_matrices();
	}

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


struct Material {
	vec3 base_color;
	scalar roughness;
	scalar emission;
	scalar metallic;
};


#endif //RAYTRACING_RAYMARCHING_HYBRID_TYPES_H
