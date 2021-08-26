//
// Created by marc on 07/07/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_TYPES_H
#define RAYTRACING_RAYMARCHING_HYBRID_TYPES_H

#include "fast_inverse_square_root.h"
#include <cmath> // TODO: Please remove this. I hate depending on libraries. It's my kryptonite
#include <cstdlib> // TODO: Please remove this. I hate depending on libraries. It's my kryptonite
#include <random>


#define max_S(a, b) ((a) > (b) ? (a) : (b))
#define min_S(a, b) ((a) > (b) ? (b) : (a))


// Infinities
inline double inf(double probe) {
	const unsigned long long k = 0x7FF0000000000000;
	return *(double*) (&k);
}

inline float inf(float probe) {
	const unsigned long int k = 0x7F800000;
	return *(float*) (&k);
}

#define INFINITY_S (inf((scalar)1))


// Changing this changes the type for the scalar.
// So you can change this to double for more accuracy
typedef double scalar;


//typedef std::mt19937 rng_engine;
typedef std::mersenne_twister_engine<unsigned int, 32, 624, 397, 31, 0x9908b0df, 11, 0xffffffff, 7, 0x9d2c5680, 15, 0xefc60000, 18, 1812433253> rng_engine;

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


	inline vec3 abs() const {
		return {
				x < 0 ? -x : x,
				y < 0 ? -y : y,
				z < 0 ? -z : z
		};
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

inline vec3 random_normal(rng_engine* engine) {
	std::uniform_real_distribution<scalar> distribution(-1, 1.0);
	scalar x = distribution(*engine);
	scalar y = distribution(*engine);
	scalar z = distribution(*engine);
	while (sqLength({x, y, z}) > 1) {
		x = distribution(*engine);
		y = distribution(*engine);
		z = distribution(*engine);
	}
	// TODO: The normalization is not really necessary for reflect_rough. Removing it will speed up the engine and give better reflections.
	//return normalize({x, y, z});
	return {x, y, z};
}

struct ray {
	vec3 origin;
	vec3 direction;
};


struct mat3 {
	vec3 col_1, col_2, col_3;

	inline vec3 operator*(vec3 v) const {
		return col_1*v.x + col_2*v.y + col_3*v.z;
	}

	inline mat3 operator*(mat3 o) const {
		return {operator*(o.col_1), operator*(o.col_2), operator*(o.col_3)};
	}
};


struct rotator {
	scalar roll, pitch, yaw;
	// Disgusting gimbaled rotator!
	// TODO: shift to quaternions or something like that

	mat3 f_mat, b_mat; // Matrices that do the forward and backward rotations

	inline void generate_matrices() {
		// Doing the painful trig functions first and caching them in a rotation matrix

		const scalar sin_r = sin(roll);
		const scalar cos_r = cos(roll);
		const scalar sin_p = sin(pitch);
		const scalar cos_p = cos(pitch);
		const scalar sin_y = sin(yaw);
		const scalar cos_y = cos(yaw);
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
};


#endif //RAYTRACING_RAYMARCHING_HYBRID_TYPES_H
