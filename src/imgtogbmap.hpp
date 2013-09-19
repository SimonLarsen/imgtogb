#ifndef __IMGTOGBMAP_HPP
#define __IMGTOGBMAP_HPP

#include <map>

struct Image {
	unsigned char *data;
	int width, height, n;
	int tilesx, tilesy;
};

struct Tile {
	int id;
	int x, y;
};

typedef std::multimap<int,Tile> mmap;
typedef std::pair<mmap::iterator,mmap::iterator> mmap_range;

unsigned char rgbToLuminance(unsigned char r, unsigned char g, unsigned char b);
void convertTile(int x, int y, Image &img, unsigned char *out);

void getRGB(int x, int y, Image &img, unsigned char *out);
unsigned int hashTile(int tilex, int tiley, Image &img);
bool compareTiles(int ax, int ay, int bx, int by, Image &img);

Image loadImage(std::string &filename);

void emitTileMap(int *tilemap, std::string &name, int width, int height, std::ostream &os);
void emitTileData(unsigned char *tiledata, std::string &name, int n, std::ostream &os);

#endif
