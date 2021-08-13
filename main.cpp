
// This file is currently used to test the stuff I add, so it will not contain anything smart

#include "Types.h"
#include "Raytracing.h"
#include "Pathtracing.h"
#include <cstdio>
#include <ctime>
#include <cstring>

using namespace pathtracing;

struct col {
	char r, g, b;
};

//        MUST BE DEVISABLE BY 4
const int width = 2048;
const int height = 2048;

const int headerSize = 14;
const int DIBSize = 40;
const int pixelsSize = width*height*3;

col* pixels;

FILE* fp;


inline void printInt(int v) {
	char* cp = (char*) (&v);
	fputc(*(cp + 0), fp);
	fputc(*(cp + 1), fp);
	fputc(*(cp + 2), fp);
	fputc(*(cp + 3), fp);
}

inline void printInt16(int v) {
	char* cp = (char*) (&v);
	fputc(*(cp + 0), fp);
	fputc(*(cp + 1), fp);
}

inline void printBIH() {
	printInt(DIBSize);
	printInt(width);
	printInt(height);
	printInt16(1);
	printInt16(24);
	printInt(0);
	printInt(pixelsSize);
	printInt(0);
	printInt(0);
	printInt(0);
	printInt(0);
}

inline void dumpPixels(col* px, int num) {
	for (int i = 0; i < num; i++) {
		col pix = px[i];
		fputc(pix.b, fp);
		fputc(pix.g, fp);
		fputc(pix.r, fp);
	}
}

int main() {
	pixels = new col[width*height];

	memset(pixels, 0, width*height*3); // This line could technically be omitted

	printf("starting render...");
	clock_t start, stop;
	start = clock();

	// Setting up the ray
	ray r = {{-2, 0, 0},
	         {0,    0, 1}};

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
			{1,  -1,  0},
			{-1,  1, 0},
			{-1,  -1, 0}
	};

	// The list of Trigons
	polygon::ref_trigon tris_2[] = {
			{2, 1, 0},
			{1, 2, 3}
	};


	// The list of vertices
	vec3 verts_3[] = {
			{0.5,  0.5,  0},
			{0.5,  -0.5,  0},
			{-0.5,  0.5, 0},
			{-0.5,  -0.5, 0}
	};

	// The list of Trigons
	polygon::ref_trigon tris_3[] = {
			{2, 1, 0},
			{1, 2, 3}
	};

	// Composing the Object
	polygon::Object obj_1 = {
			{verts_1, 8, tris_1, 6},
			{{0, 0, 0}, {0, 0, 0}}
	};
	obj_1.mat = {{0.8, 0.8, 0.8}, 0.5, 0.0};
	polygon::Object obj_2 = {
			{verts_2, 4, tris_2, 2},
			{{0, -1, 0}, {1.57, 0, 0}}
	};
	obj_2.mat = {{0.8, 0, 0}, 0.5, 0.0};
	polygon::Object obj_3 = {
			{verts_2, 4, tris_2, 2},
			{{0, 1, 0}, {-1.57, 0, 0}}
	};
	obj_3.mat = {{0, 0.8, 0}, 0.5, 0.0};
	polygon::Object obj_4 = {
			{verts_3, 4, tris_3, 2},
			{{0, 0, 0.9}, {0, 0, 0}}
	};
	obj_4.mat = {{1, 1, 1}, 0.5, 1};
	polygon::Node* l[] = {&obj_1, &obj_2, &obj_3, &obj_4};
	polygon::Group obj = {4, l};
	obj.build();

	Pathtracer pt = {&obj};


	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			r.direction = {1, ((float) x)/width*2 - 1, ((float) y)/height*2 - 1};

			vec3 light = pt.trace_path(r, 1);

			col cc = {(char) (light.x*255), (char) (light.y*255), (char) (light.z*255)};
			pixels[y*width + x] = cc;
		}
	}

	stop = clock();
	printf("\nFinished rendering the frame!");
	printf("\nBeginning writing it to a file (with my dodgey bitmap exporter)...");


	fp = fopen("test.bmp", "wb");

	fputc('B', fp);
	fputc('M', fp);

	printInt(headerSize + DIBSize + pixelsSize);

	fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);

	printInt(headerSize + DIBSize);

	printBIH();
	dumpPixels(pixels, width*height);

	fclose(fp);

	printf("\nFinished writing it to a file!");
	printf("\n\nrendering took %.3f seconds!\n", (double) (stop - start)/CLOCKS_PER_SEC);
}
