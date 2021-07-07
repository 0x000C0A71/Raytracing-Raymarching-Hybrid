//
// Created by xerne on 07/07/2021.
//

#ifndef RAYTRACING_RAYMARCHING_HYBRID_FAST_INVERSE_SQUARE_ROOT_H
#define RAYTRACING_RAYMARCHING_HYBRID_FAST_INVERSE_SQUARE_ROOT_H

#include <iostream>

// This has been copied out of quake 3 arena
inline
float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

// This has been adapted to work with doubles
inline
double Q_rsqrt( double number )
{
	long long i;
	double x2, y;
	const double threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long long * ) &y;                  // evil floating point bit level hacking
	i  = 0x5fe6eb50c7b537a9 - ( i >> 1 );       // what the fuck?
	y  = * ( double * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}


#endif //RAYTRACING_RAYMARCHING_HYBRID_FAST_INVERSE_SQUARE_ROOT_H
