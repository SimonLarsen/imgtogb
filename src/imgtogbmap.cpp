#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include "stb_image.c"

#include "imgtogbmap.hpp"

const char* help_text =
"Usage: imgtogbmap [OPTIONS] IMAGE\n\n"
"Options:\n"
"  -h            Print this help text.\n"
"  -n NAME       Name of tile map.\n"
"  -o FILENAME   Path to output file.\n\n"
"Output is written to STDOUT if no output file is given.\n\n"
"If no NAME is given the basename of IMAGE is used.\n"
"(e.g. \"monster.png\" will produce \"monster_tiles\" and \"monster_data\")\n";

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

/**
 * Loads image from a file.
 * @param filename Path to image
 *
 * @return Loaded image
 */
Image loadImage(std::string &filename) {
	Image img;

	img.data = stbi_load(filename.c_str(), &img.width, &img.height, &img.n, 3);
	if(img.data == NULL) {
		std::cerr << "Error loading image " << filename << std::endl;
	} else {
		std::cerr << "Loaded image " << filename << " successfully" << std::endl;
	}
	std::cerr << "Image size: " << img.width << "x" << img.height << std::endl;
	img.tilesx = img.width/8;
	img.tilesy = img.height/8;
	std::cerr << "Tiles count: " << img.tilesx << "x" << img.tilesy << std::endl;

	return img;
}

/**
 * Emits the generated tile map to given output stream.
 *
 * @param tilemap Tile map as an array of integers.
 * @parma width Width of tile map in tiles
 * @param height Height of tile map in tiles
 * @param os Output stream.
 */
void emitTileMap(int *tilemap, std::string &name, int width, int height, std::ostream &os) {
	// Print tile map
	os << "#define " << std::setbase(10) << name << "_tiles_length " << width*height << std::endl;
	os << std::setbase(16) << std::setfill('0');
	os << "const unsigned char " << name << "_tiles[] = {\n";
	for(int iy = 0; iy < height; ++iy) {
		os << "\t";
		for(int ix = 0; ix < width; ++ix) {
			os << "0x" << std::setw(2) << tilemap[ix+iy*width] << ", ";
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
	int tile_count, hash;
	unsigned int pos;
	bool found, writeToFile;

	Image img;
	Tile tile;
	int *tilemap;

	mmap map;
	mmap::iterator it;
	mmap_range range;

	// Parse arguments
	writeToFile = false;
	for(int i = 1; i < argc; ++i) {
		if(!strcmp(argv[i], "-n")) {
			name = argv[++i];
		} else if (!strcmp(argv[i], "-h")) {
			std::cerr << help_text;
			return EXIT_FAILURE;
		} else if (!strcmp(argv[i], "-o")) {
			outputfile = argv[++i];
			writeToFile = true;
		} else {
			inputfile = argv[i];
		}
	}
	// Check if input file is given
	if(inputfile.length() == 0) {
		std::cerr << "error: Missing input file." << std::endl;
		std::cerr << help_text;
		return EXIT_FAILURE;
	}
	// Generate name from filename
	if(name.length() == 0) {
		pos = inputfile.find_last_of('.');
		if(pos == std::string::npos) {
			name = inputfile;
		} else {
			name = inputfile.substr(0,pos);
		}
	}

	// Load image from file
	img = loadImage(inputfile);

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
	os << "#ifndef __" << name << "__" << std::endl;
	os << "#define __" << name << "__" << std::endl << std::endl;
	emitTileMap(tilemap, name, img.tilesx, img.tilesy, os);
	emitTileData(tiledata, name, tile_count, os);
	os << "#endif" << std::endl;

	// Close output file
	if(writeToFile) osfile.close();

	// Clean up 
	stbi_image_free(img.data);
	delete[] tiledata;
	delete[] tilemap;

	return EXIT_SUCCESS;
}
