
// This file is currently used to test the stuff I add, so it will not contain anything smart

#include "Types.h"
#include "Raytracing.h"
#include "Pathtracing.h"
#include "BMP_export.cpp"
#include <cstdio>
#include <ctime>
#include <cstring>

#include "Renderer.h"

#define PI_S 3.14159265359

using namespace pathtracing;

//        MUST BE DEVISABLE BY 4
const int width = 1024;
const int height = 1024;


col* pixels;


inline scalar clamp(scalar x, scalar mn, scalar mx) {
	if (x > mx) return mx;
	if (x < mn) return mn;
	return x;
}

inline col from_light(vec3 light) {
	const vec3 l2 = {
			clamp(light.x, 0.0, 1.0),
			clamp(light.y, 0.0, 1.0),
			clamp(light.z, 0.0, 1.0),
	};
	return {(char)(l2.x*255), (char)(l2.y*255), (char)(l2.z*255)};
}

int main() {
	pixels = new col[width*height];

	memset(pixels, 0, width*height*3); // This line could technically be omitted

	printf("starting render...");
	clock_t start, stop;

	// Setting up the ray
	ray r = {{-3, 0, 0},
	         {0,  0, 1}};

	// The list of vertices
	vec3 verts_1[] = {
			{1,  1,  1},
			{1,  1,  -1},
			{1,  -1, 1},
			{1,  -1, -1},
			{-1, 1,  1},
			{-1, 1,  -1},
			{-1, -1, 1},
			{-1, -1, -1}
	};

	// The list of Trigons
	polygon::ref_trigon tris_1[] = {
			{5, 7, 1},
			{1, 7, 3},
			{3, 0, 1},
			{2, 0, 3},
			{4, 0, 2},
			{2, 6, 4}
	};


	// The list of vertices
	vec3 verts_2[] = {
			{1,  1,  0},
			{1,  -1, 0},
			{-1, 1,  0},
			{-1, -1, 0}
	};

	// The list of Trigons
	polygon::ref_trigon tris_2[] = {
			{2, 0, 1},
			{1, 3, 2}
	};


	// The list of vertices
	vec3 verts_3[] = {
			{0.5,  0.5,  0},
			{0.5,  -0.5, 0},
			{-0.5, 0.5,  0},
			{-0.5, -0.5, 0}
	};

	// The list of Trigons
	polygon::ref_trigon tris_3[] = {
			{2, 0, 1},
			{1, 3, 2}
	};

	// Composing the Object
	polygon::Object obj_1 = {
			{verts_1,   8, tris_1, 6},
			{{0, 0, 0}, {0, 0, 0}}
	};
	obj_1.mat = {{0.9, 0.9, 0.9}, 1, 0.0, 0};
	polygon::Object obj_2 = {
			{verts_2,    4, tris_2, 2},
			{{0, -1, 0}, {PI_S/2, 0, 0}}
	};
	obj_2.mat = {{0.9, 0.2, 0.2}, 1, 0.0, 0};
	polygon::Object obj_3 = {
			{verts_2,   4, tris_2, 2},
			{{0, 1, 0}, {-(PI_S/2), 0, 0}}
	};
	obj_3.mat = {{0.2, 0.9, 0.2}, 1, 0.0, 0};
	polygon::Object obj_4 = {
			{verts_3,     4, tris_3, 2},
			{{0, 0, 0.999}, {0, 0, 0}}
	};
	obj_4.mat = {{1, 1, 1}, 1, 1, 0};
	polygon::Node* l[] = {&obj_1, &obj_2, &obj_3, &obj_4};
	polygon::Group obj = {4, l};
	//obj.build();


	//raymarching::Object* obj_m = new raymarching::primitives::Torus(0.4, 0.15);
	raymarching::Object* obj_m = new raymarching::primitives::Box(0.3, 0.3, 0.3, 0.04);
	//raymarching::Object* obj_m = new raymarching::primitives::Sphere(0.5);
	obj_m->transform = {{0, 0, 0},
	                    {1, 1, 1.5}};
	obj_m->mat = {{0.9, 0.9, 0.9}, 1, 0, 1};
	//obj_m->build();

	Pathtracer pt = {&obj, (raymarching::Node*)(obj_m)};
	//pt.build();

	Renderer rr = Renderer{pt, width, height, NUMBER_OF_THREADS};
	rr.build();


	vec3* buff = new vec3[width*height]{{0, 0, 0}};

	start = clock();
	rr.multisample_frame(buff);
	stop = clock();

	printf("\nFinished rendering the frame!");

	printf("\nTonemapping... ");
	rr.tonemap_frame(buff);

	for (int j = 0; j < width*height; ++j) {
		pixels[j] = from_light(buff[j]);
	}
	printf("Done!");

	printf("\nBeginning writing it to a file (with my dodgey bitmap exporter)...");

	export_image(pixels, width, height);

	printf("\nFinished writing it to a file!");
	printf("\n\nrendering took %.3f seconds!\n", (double)(stop - start)/CLOCKS_PER_SEC);
}
