#ifndef __IMAGE_HPP
#define __IMAGE_HPP

#include "stb_image.h"

class Image {
public:
	Image() : data(0), width(0), height(0) { }

	void load(std::string &filename) {
		data = stbi_load(filename.c_str(), &width, &height, &n, 3);
		tilesx = width/8;
		tilesy = height/8;
	}

	~Image() {
		if(data != NULL) stbi_image_free(data);
	}

	unsigned char *data;
	int width, height, n;
	int tilesx, tilesy;
};

#endif
