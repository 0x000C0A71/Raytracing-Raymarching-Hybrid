
// This file is currently used to test the stuff I add, so it will not contain anything smart

#include "Types.h"
#include "Raytracing.h"
#include <cstdio>
#include <ctime>
#include <cstring>

using namespace polygon;

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
	ray r = {{-2.5, 0, 0},
	         {0,    0, 1}};

	// The list of vertices
	vec3 verts[] = {
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
	ref_trigon tris[] = {
			{1, 5, 7},
			{1, 3, 7},
			{1, 3, 0},
			{2, 3, 0},
			{2, 4, 0},
			{2, 4, 6}
	};

	// Composing the Object
	Object obj_1 = {
			{verts, 8, tris, 6},
			{{0, 0, 0}, {1.3, -0.9, 0.5}}
	};
	Object obj_2 = {
			{verts, 8, tris, 6},
			{{0, 0, 0}, {0, 0, 0}}
	};
	Node* l[2] = {&obj_1, &obj_2};
	Group obj = {2, l};
	obj.build();


	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			r.direction = {1, ((float) x)/width*2 - 1, ((float) y)/height*2 - 1};

			vec3 poi{};
			scalar alpha;
			vec3 K{};
			if (obj.intersect_ray(r, &poi, &alpha, &K)) {
				col cc = {(char) (K.x*255), (char) (K.y*255), (char) (K.z*255)};
				pixels[y*width + x] = cc;
			}
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
