#ifndef __IMAGE_HPP
#define __IMAGE_HPP

#include <stb_image.h>
#include <string>
#include <exception>

class ImageException : public std::exception {
public:
	ImageException(const std::string& text = "Undefined exception")
		: std::exception(), text(text) { }

	~ImageException() throw() { }

	const char *what() const throw() {
		return text.c_str();
	}

private:
	std::string text;
};

class Image {
public:
	Image()
		: data(NULL), width(0), height(0), tilesx(0), tilesy(0) { }

	~Image() {
		if(data == NULL) stbi_image_free(data);
	}

	/**
	 * Reads image data from a file.
	 *
	 * @param filename Path to image file
	 */
	void load(const std::string& filename) throw (const ImageException&) {
		data = stbi_load(filename.c_str(), &width, &height, &n, 3);
		if(data == NULL) {
			throw ImageException(std::string("Could not load image").append(filename));
		}
		tilesx = width/8;
		tilesy = height/8;
	}

	/**
	 * Gets RGB values from pixel
	 *
	 * @param x X-coordinate of pixel
	 * @param y Y-coordinate of pixel
	 * @param out Output buffer of 3 bytes for storing RGB values
	 */
	void getRGB(int x, int y, unsigned char *out) const {
		int in;
		in = (x + y*width)*3;
		out[0] = data[in];
		out[1] = data[in+1];
		out[2] = data[in+2];
	}

	/**
	 * Converts tile (x,y) into Game Boy tile data.
	 * Coordinates (x,y) will return data for tile at
	 * position (x*8, y*8)
	 *
	 * @param x Tile X-coordinate 
	 * @param y Tile Y-coordinate
	 * @param out Output buffer of 16 bytes for storing tile data
	 */
	void convert(int x, int y, unsigned char *out) const {
		unsigned char l1, l2;
		unsigned char lum, col;
		unsigned char rgb[3];

		for(int iy = 0; iy < 8; ++iy) {
			l1 = l2 = 0;
			for(int ix = 0; ix < 8; ++ix) {
				getRGB(x*8+ix, y*8+iy, rgb);
				lum = luminance(rgb[0], rgb[1], rgb[2]);
				col = 3 - lum / 64;
				l1 |= (col & 1) << (7 - ix);
				l2 |= ((col & 2) >> 1) << (7 - ix);
			}
			out[iy*2]   = l1;
			out[iy*2+1] = l2;
		}
	}

	int getWidth() const { return width; }
	int getHeight() const { return height; }

	int getTilesX() const { return tilesx; }
	int getTilesY() const { return tilesy; }

private:
	unsigned char *data;
	int width, height, n;
	int tilesx, tilesy;

	/**
	 * Returns (approximation) of luminance for given RGB values.
	 *
	 * @param r Red channel value (0-255)
	 * @param g Green channel value (0-255)
	 * @param b Blue channel value (0-255)
	 * @return Luminance (0-255)
	 */
	static unsigned char luminance(unsigned char r, unsigned char g, unsigned char b) {
		return (2*r + 7*g + b)/10;
	}
};

#endif
