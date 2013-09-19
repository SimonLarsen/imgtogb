#include <iostream>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include "stb_image.c"

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

/**
 * Loads image from a file.
 * @param filename Path to image
 *
 * @return Loaded image
 */
Image loadImage(char *filename) {
	Image img;

	img.data = stbi_load(filename, &img.width, &img.height, &img.n, 3);
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

void emitTileMap(int *tilemap, Image &img, std::ostream &os) {
	// Print tile map
	os << "unsigned char tile_map[] = {\n";
	for(int iy = 0; iy < img.tilesy; ++iy) {
		os << "\t";
		for(int ix = 0; ix < img.tilesx; ++ix) {
			os << tilemap[ix+iy*img.tilesx] << ", ";
		}
		os << "\n";
	}
	os << "};" << std::endl;
}

void emitTileData(unsigned char *tiledata, int tile_count, std::ostream &os) {
	// Print tile data
	os << "unsigned char tile_data[] = {";
	for(int i = 0; i < tile_count*16; ++i) {
		if(i % 16 == 0) os << "\n\t";
		os << (int)tiledata[i] << ", ";
	}
	os << "\n};" << std::endl;
	std::cerr << "Produced data for " << tile_count << " tiles" << std::endl;
}

int main(int argc, char **argv) {
	char *filename;
	int tile_count, hash;
	bool found;
	Image img;
	Tile tile;
	int *tilemap;
	mmap map;
	mmap::iterator it;
	mmap_range range;

	// Get filename
	filename = argv[1];

	// Load image from file
	img = loadImage(filename);

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

	emitTileMap(tilemap, img, std::cout);

	emitTileData(tiledata, tile_count, std::cout);

	// Free image data
	stbi_image_free(img.data);

	// Clean up 
	delete[] tiledata;
	delete[] tilemap;

	return 0;
}
