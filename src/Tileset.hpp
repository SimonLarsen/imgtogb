#ifndef __TILESET_HPP
#define __TILESET_HPP

#include <map>
#include <ostream>
#include <string>
#include "Image.hpp"
#include "Tile.hpp"

typedef std::multimap<int,Tile> mmap;
typedef std::pair<mmap::iterator,mmap::iterator> mmap_range;

class Tileset {
public:
	Tileset() : count(0), tilemap(NULL) { }

	~Tileset() {
		if(tilemap) delete[] tilemap;
	}

	void generate(const Image& image);
	int addTile(Tile& tile, int hash);
	void emitData(std::ostream& os, const std::string& name);
	void emitTilemap(std::ostream& os, const std::string& name, int offset);

private:
	const Image* image;
	int count;
	int *tilemap;
	mmap map;
};

#endif
