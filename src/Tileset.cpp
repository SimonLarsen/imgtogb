#include <iomanip>
#include "Tileset.hpp"
#include <cstring>

/**
 * Generates a tilemap and tileset from a given image.
 *
 * @param image Source image
 */
void Tileset::generate(const Image& image) {
	Tile tile;
	int hash;
	bool found;
	mmap_range range;
	mmap::iterator it;

	tilemap = new int[image.getTilesX()*image.getTilesY()];

	this->image = &image;
	tile.image = &image;
	for(int iy = 0; iy < image.getTilesY(); ++iy) {
		for(int ix = 0; ix < image.getTilesX(); ++ix) {
			tile.x = ix;
			tile.y = iy;
			hash = tile.hash();

			// Look for existing tiles with same hash
			range = map.equal_range(hash);
			// No matches found
			if(range.first == range.second) {
				tilemap[ix+iy*image.getTilesX()] = addTile(tile, hash);
			}
			// Else add it of not already in set
			else {
				found = false;
				for(it = range.first; it != range.second; ++it) {
					if(Tile::compare(tile, it->second) == true) {
						found = true;
						tilemap[ix+iy*image.getTilesX()] = it->second.id;
					}
				}
				if(found == false) {
					tilemap[ix+iy*image.getTilesX()] = addTile(tile, hash);
				}
			}
		}
	}
}

/**
 * Adds a tile to the tile set.
 *
 * @param tile Tile to add
 * @param hash Hash value of tile
 * @return Index of inserted tile
 */
int Tileset::addTile(Tile& tile, int hash) {
	tile.id = count;
	map.insert(std::pair<int,Tile>(hash, tile));
	count++;
	return count-1;
}

/**
 * Emits tileset data as an array to given output stream.
 *
 * @param os Output stream
 * @param name Name of tileset
 */
void Tileset::emitData(std::ostream& os, const std::string& name) {
	unsigned char *tiles;
	unsigned char data[16];
	mmap::iterator it;
	Tile *tile;

	tiles = new unsigned char[count*16];

	for(it = map.begin(); it != map.end(); ++it) {
		tile = &(*it).second;
		image->convert(tile->x, tile->y, data);
		memcpy(&tiles[tile->id*16], data, sizeof(unsigned char)*16);
	}

	os << "#define " << name << "_data_length " << count << std::endl;
	os << "const unsigned char " << name << "_data[] = {" << std::endl;

	for(int i = 0; i < count; ++i) {
		os << "\t";
		for(int j = 0; j < 16; ++j) {
			os << std::setw(3) << (int)tiles[i*16+j] << ", ";
		}
		os << std::endl;
	}

	os << "};" << std::endl;

	delete[] tiles;
}

/**
 * Emits tileset indices to given output stream.
 *
 * @param os Output stream
 * @param name Name of tilemap
 */
void Tileset::emitTilemap(std::ostream& os, const std::string& name, int offset) {
	os << "#define " << name << "_tiles_width " << std::setbase(10) << image->getTilesX() << std::endl;
	os << "#define " << name << "_tiles_height " << std::setbase(10) << image->getTilesY() << std::endl;
	os << "const unsigned char " << name << "_tiles[] = {\n";
	for(int iy = 0; iy < image->getTilesY(); ++iy) {
		os << "\t";
		for(int ix = 0; ix < image->getTilesX(); ++ix) {
			os << std::setw(3) << tilemap[ix+iy*image->getTilesX()]+offset << ", ";
		}
		os << std::endl;
	}
	os << "};" << std::endl;
}
