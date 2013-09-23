#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iomanip>

#include "imgtogbmap.hpp"

const char* help_text =
"Usage: imgtogbmap [OPTIONS] IMAGE\n\n"
"Options:\n"
"  -h            Print this help text.\n"
"  -n NAME       Name of tile map.\n"
"  -o FILENAME   Path to output file.\n"
"  -O OFFSET     Adds offset to tile map indices\n\n"
"Output is written to STDOUT if no output file is given.\n\n"
"If no NAME is given the basename of IMAGE is used.\n"
"(e.g. \"monster.png\" will produce \"monster_tiles\" and \"monster_data\")\n";

/**
 * Emits the generated tile map to given output stream.
 *
 * @param tilemap Tile map as an array of integers.
 * @parma width Width of tile map in tiles
 * @param height Height of tile map in tiles
 * @param os Output stream.
 */
void emitTileMap(int *tilemap, std::string &name, int width, int height, int offset, std::ostream &os) {
	// Print tile map
	os << "#define " << name << "_tiles_width " << std::setbase(10) << width << std::endl;
	os << "#define " << name << "_tiles_height " << std::setbase(10) << height << std::endl;
	os << std::setbase(16) << std::setfill('0');
	os << "const unsigned char " << name << "_tiles[] = {\n";
	for(int iy = 0; iy < height; ++iy) {
		os << "\t";
		for(int ix = 0; ix < width; ++ix) {
			os << "0x" << std::setw(2) << tilemap[ix+iy*width]+offset << ", ";
		}
		os << "\n";
	}
	os << "};" << std::endl << std::endl;
}

/**
 * Emits the generated tile data to given output stream.
 * @param tiledata Tile data as an array of unsigned chars.
 * @param n Number of tiles in data array.
 * @param os Output stream.
 */
void emitTileData(unsigned char *tiledata, std::string &name, int n, std::ostream &os) {
	// Print tile data
	os << "#define " << std::setbase(10) << name << "_data_length " << n << std::endl;
	os << "const unsigned char " << name << "_data[] = {";
	os << std::setbase(16) << std::setfill('0');
	for(int i = 0; i < n*16; ++i) {
		if(i % 16 == 0) os << "\n\t";
		os << "0x" << std::setw(2) << (int)tiledata[i] << ", ";
	}
	os << "\n};" << std::endl << std::endl;
}

int main(int argc, char **argv) {
	std::string inputfile, outputfile, name;
	int tile_count, hash, offset;
	bool found, writeToFile;

	Image img;
	Tile tile;
	int *tilemap;

	mmap map;
	mmap::iterator it;
	mmap_range range;

	// Parse arguments
	writeToFile = false;
	offset = 0;
	for(int i = 1; i < argc; ++i) {
		if(!strcmp(argv[i], "-n")) {
			name = argv[++i];
		} else if (!strcmp(argv[i], "-h")) {
			std::cerr << help_text;
			return 1;
		} else if (!strcmp(argv[i], "-o")) {
			outputfile = argv[++i];
			writeToFile = true;
		} else if (!strcmp(argv[i], "-O")) {
			offset = atoi(argv[++i]);
		} else if(argv[i][0] != '-') {
			inputfile = argv[i];
		}
	}
	// Check if input file is given
	if(inputfile.length() == 0) {
		std::cerr << "error: Missing input file." << std::endl;
		std::cerr << help_text;
		return 1;
	}
	// Generate name from filename
	if(name.length() == 0) {
		unsigned int start, end;
		start = inputfile.find_last_of('/');
		start = (start < std::string::npos ? start+1 : 0);
		end = inputfile.find_last_of('.');
		end = (end < std::string::npos ? end : inputfile.length()-1);
		name = inputfile.substr(start,end-start);
	}

	// Load image from file
	try {
		img.load(inputfile);
	} catch(const ImageLoadException &e) {
		std::cerr << e.what() << inputfile << std::endl;
		return 1;
	}

	// Create tile map
	tilemap = new int[img.tilesx*img.tilesy];

	// Generate tile map
	tile_count = 0;
	for(int iy = 0; iy < img.tilesy; ++iy) {
		for(int ix = 0; ix < img.tilesx; ++ix) {
			hash = hashTile(ix, iy, img);
			// Check if tile is already in map
			range = map.equal_range(hash);
			if(range.first == range.second) {
				// Add new tile
				tile.x = ix;
				tile.y = iy;
				tile.id = tile_count;
				tile_count++;

				map.insert(std::pair<int,Tile>(hash, tile));
				tilemap[ix+iy*img.tilesx] = tile.id;
			} else {
				found = false;
				for(it = range.first; it != range.second; ++it) {
					if(compareTiles(ix, iy, it->second.x, it->second.y, img) == true) {
						found = true;
						tilemap[ix+iy*img.tilesx] = it->second.id;
						break;
					}
				}
				if(found == false) {
					// Add new tile
					tile.x = ix;
					tile.y = iy;
					tile.id = tile_count;
					tile_count++;

					map.insert(std::pair<int,Tile>(hash, tile));
					tilemap[ix+iy*img.tilesx] = tile.id;
				}
			}
		}
	}

	// Generate tile data
	unsigned char *tiledata = new unsigned char[tile_count*16];
	for(it = map.begin(); it != map.end(); ++it) {
		convertTile(it->second.x*8, it->second.y*8, img, &tiledata[it->second.id*16]);
	}

	// Open file is output filename is given, use std::cout otherwise
	std::ofstream osfile;
	std::ostream &os = writeToFile
		? osfile.open(outputfile.c_str()), osfile : std::cout;

	// Emit header file
	os << "#ifndef __" << name << "_tiles__" << std::endl;
	os << "#define __" << name << "_tiles__" << std::endl << std::endl;
	emitTileMap(tilemap, name, img.tilesx, img.tilesy, offset, os);
	emitTileData(tiledata, name, tile_count, os);
	os << "#endif" << std::endl;

	// Close output file
	if(writeToFile) osfile.close();

	// Clean up 
	delete[] tiledata;
	delete[] tilemap;

	return 0;
}
