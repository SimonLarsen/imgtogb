#ifndef __COMMON_HPP
#define __COMMON_HPP

#include <string>
#include "image.hpp"

struct Tile {
	int id;
	int x, y;
};

unsigned char rgbToLuminance(unsigned char r, unsigned char g, unsigned char b);
void convertTile(int x, int y, Image &img, unsigned char *out);

unsigned int hashTile(int tilex, int tiley, Image &img);
bool compareTiles(int ax, int ay, int bx, int by, Image &img);

void getRGB(int x, int y, Image &img, unsigned char *out);
Image loadImage(std::string &filename);

#endif
