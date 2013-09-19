#include <iostream>

#include "image.hpp"
#include "imgtogbmap.hpp"

/**
 * Converts RGB color to (approximation of) luminance.
 */
unsigned char rgbToLuminance(unsigned char r, unsigned char g, unsigned char b) {
	return (2*r + 7*g + b)/10;
}

/**
 * Returns data values for 8x8 tile at (x,y).
 * @param x X-coordinate for upper left corner of tile.
 * @param y Y-coordinate for upper left corner of tile.
 * @param w Width of image in pixels.
 * @param data Pixel data for image (in 3BPP).
 * @param out Buffer for the 16 data values.
 */
void convertTile(int x, int y, Image &img, unsigned char *out) {
	unsigned char l1, l2;
	unsigned char lum, col;
	unsigned int in;
	for(int iy = 0; iy < 8; ++iy) {
		l1 = l2 = 0;
		for(int ix = 0; ix < 8; ++ix) {
			in = (x+ix + (y+iy)*img.width)*3;
			lum = rgbToLuminance(img.data[in], img.data[in+1], img.data[in+2]);
			col = 3 - lum / 64;
			l1 |= (col & 1) << (7 - ix);
			l2 |= ((col & 2) >> 1) << (7 - ix);
		}
		out[iy*2]   = l1;
		out[iy*2+1] = l2;
	}
}

/**
 * Retrieves RGB values for a pixel.
 * @param x X-coordinate of pixel.
 * @param y Y-coordinate of pixel.
 * @param img Image to retrieve from.
 * @param out Buffer array for output.
 */
void getRGB(int x, int y, Image &img, unsigned char *out) {
	int in;
	in = (x + y*img.width)*3;
	out[0] = img.data[in];
	out[1] = img.data[in+1];
	out[2] = img.data[in+2];
}

/**
 * Calculates hash value for tile.
 * @param tilex X-coordinate of tile
 * @param tiley Y-coordinate of tile
 * @param img Image containing tile
 *
 * @return hash value of tile at (tilex,tiley)
 */
unsigned int hashTile(int tilex, int tiley, Image &img) {
	unsigned int hash;
	unsigned char rgb[3];

	hash = 0;
	for(int iy = 0; iy < 8; ++iy) {
		for(int ix = 0; ix < 8; ++ix) {
			getRGB(tilex*8+ix, tiley*8+iy, img, rgb);
			for(int i = 0; i < 3; ++i) {
				//hash <<= 0;
				hash += rgb[i];
			}
		}
	}
	return hash;
}

/**
 * Compare two tiles for equality.
 * @param ax X-coordinate of first tile
 * @param ay Y-coordinate of first tile
 * @param bx X-coordinate of second tile
 * @param by Y-coordinate of second tilesx
 *
 * @return True if tiles are equal, false otherwise
 */
bool compareTiles(int ax, int ay, int bx, int by, Image &img) {
	unsigned char argb[3], brgb[3];
	for(int iy = 0; iy < 8; ++iy) {
		for(int ix = 0; ix < 8; ++ix) {
			getRGB(ax*8+ix, ay*8+iy, img, argb);
			getRGB(bx*8+ix, by*8+iy, img, brgb);
			if(argb[0] != brgb[0]
			|| argb[1] != brgb[1]
			|| argb[2] != brgb[2]) {
				return false;
			}
		}
	}
	return true;
}
