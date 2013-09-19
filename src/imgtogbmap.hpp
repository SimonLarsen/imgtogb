#ifndef __IMGTOGBMAP_HPP
#define __IMGTOGBMAP_HPP

#include <map>
#include "common.hpp"

typedef std::multimap<int,Tile> mmap;
typedef std::pair<mmap::iterator,mmap::iterator> mmap_range;

void emitTileMap(int *tilemap, std::string &name, int width, int height, std::ostream &os);
void emitTileData(unsigned char *tiledata, std::string &name, int n, std::ostream &os);

#endif
