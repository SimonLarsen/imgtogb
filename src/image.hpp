#ifndef __IMAGE_HPP
#define __IMAGE_HPP

#include "stb_image.h"
#include <stdexcept>
#include <string>

class ImageLoadException : public std::runtime_error {
public:
	ImageLoadException(const std::string &filename)
		: std::runtime_error(std::string("error: Could not read image ").append(filename)) { }
};

class Image {
public:
	Image() : data(0), width(0), height(0) { }

	void load(std::string &filename) throw (const ImageLoadException&) {
		data = stbi_load(filename.c_str(), &width, &height, &n, 3);
		if(data == NULL) {
			throw ImageLoadException(filename);
		}
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
