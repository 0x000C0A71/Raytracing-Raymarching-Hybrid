
// This file is currently used to test the stuff I add, so it will not contain anything smart

#include <stdio.h>
#include "Types.h"
#include "Raytracing.h"
#include <time.h>

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

col pixels[width*height];

FILE *fp;



void printInt(int v) {
	char* cp = (char*)(&v);
	fputc(*(cp + 0), fp);
	fputc(*(cp + 1), fp);
	fputc(*(cp + 2), fp);
	fputc(*(cp + 3), fp);
}

void printInt16(int v) {
	char* cp = (char*)(&v);
	fputc(*(cp + 0), fp);
	fputc(*(cp + 1), fp);
}

void printBIH() {
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

void dumpPixels(col* px, int num) {
	for (int i = 0; i < num; i++) {
		col pix = px[i];
		fputc(pix.b, fp);
		fputc(pix.g, fp);
		fputc(pix.r, fp);
	}
}
int main() {
	printf("starting render...");
	clock_t start, stop;
	start = clock();

	const trigon t = {{1, 0, -0.75}, {1, 0.866025, 0.75}, {1, -0.866025, 0.75}};
	ray r = {{0, 0, 0}, {0, 0, 1}};
	const plane a = t.get_plane();

	vec3 total = {0, 0, 0};

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			r.origin = {-0, 0, 0};
			r.direction = {1, ((float)x)/width*2-1, ((float)y)/height*2-1};

			vec3 i;
			scalar bl;
			if (ray_plane_intersection(r, a, &i, &bl)) {

				vec3 c;
				if (point_on_trigon(t, i, &c)) {
					col cc = {(char)(c.x*255), (char)(c.y*255), (char)(c.z*255)};
					pixels[y*width+x] = cc;
				}
			}
		}
	}

	stop = clock();
	printf("\nFinished rendering the frame!");
	printf("\nBeginning writing it to a file (with my dodgey bitmap exporter)...");


	fp = fopen("test.bmp", "w");

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
	printf("\n\nrendering took %.3f seconds!\n", (double)(stop - start) / CLOCKS_PER_SEC);
}
