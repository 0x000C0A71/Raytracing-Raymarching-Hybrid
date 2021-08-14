
// This file is currently used to test the stuff I add, so it will not contain anything smart

#include "Types.h"
#include "Raytracing.h"
#include "Pathtracing.h"
#include "BMP_export.cpp"
#include <cstdio>
#include <ctime>
#include <cstring>

using namespace pathtracing;

//        MUST BE DEVISABLE BY 4
const int width = 512;
const int height = 512;


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
	return {(char) (l2.x*255), (char) (l2.y*255), (char) (l2.z*255)};
}

int main() {
	pixels = new col[width*height];

	memset(pixels, 0, width*height*3); // This line could technically be omitted

	printf("starting render...");
	clock_t start, stop;
	start = clock();

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
			{5, 1, 7},
			{1, 3, 7},
			{3, 1, 0},
			{2, 3, 0},
			{4, 2, 0},
			{2, 4, 6}
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
			{2, 1, 0},
			{1, 2, 3}
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
			{2, 1, 0},
			{1, 2, 3}
	};

	// Composing the Object
	polygon::Object obj_1 = {
			{verts_1,   8, tris_1, 6},
			{{0, 0, 0}, {0, 0, 0}}
	};
	obj_1.mat = {{0.8, 0.8, 0.8}, 1, 0.0};
	polygon::Object obj_2 = {
			{verts_2,    4, tris_2, 2},
			{{0, -1, 0}, {1.57, 0, 0}}
	};
	obj_2.mat = {{0.8, 0, 0}, 0.7, 0.0};
	polygon::Object obj_3 = {
			{verts_2,   4, tris_2, 2},
			{{0, 1, 0}, {-1.57, 0, 0}}
	};
	obj_3.mat = {{0, 0.8, 0}, 0.7, 0.0};
	polygon::Object obj_4 = {
			{verts_3,     4, tris_3, 2},
			{{0, 0, 0.9}, {0, 0, 0}}
	};
	obj_4.mat = {{1, 1, 1}, 0.5, 4};
	polygon::Node* l[] = {&obj_1, &obj_2, &obj_3, &obj_4};
	polygon::Group obj = {4, l};
	obj.build();

	Pathtracer pt = {&obj};

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			r.direction = {2, ((float) x)/width*2 - 1, ((float) y)/height*2 - 1};
			r.direction = normalize(r.direction);


			vec3 light{};
			for (int i = 0; i < SAMPLES; i++) {
				light = light + pt.trace_path(r, MAX_BOUNCE_COUNT);
			}

			light = light*(1.0/SAMPLES);

			col cc = from_light(light);
			pixels[y*width + x] = cc;
		}
	}

	stop = clock();
	printf("\nFinished rendering the frame!");
	printf("\nBeginning writing it to a file (with my dodgey bitmap exporter)...");

	export_image(pixels, width, height);

	printf("\nFinished writing it to a file!");
	printf("\n\nrendering took %.3f seconds!\n", (double) (stop - start)/CLOCKS_PER_SEC);
}
