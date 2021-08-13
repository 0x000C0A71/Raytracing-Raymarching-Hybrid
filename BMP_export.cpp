//
// Created by marc on 13/08/2021.
//


struct col {
	char r, g, b;
};


const int headerSize = 14;
const int DIBSize = 40;

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

inline void printBIH(int width, int height, int pixelsSize) {
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



void export_image(col* pixels, int width, int height) {
	const int pixelsSize = width*height*3;

	fp = fopen("test.bmp", "wb");

	fputc('B', fp);
	fputc('M', fp);

	printInt(headerSize + DIBSize + pixelsSize);

	fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);

	printInt(headerSize + DIBSize);

	printBIH(width, height, pixelsSize);
	dumpPixels(pixels, width*height);

	fclose(fp);
}
