#ifndef __TILE_HPP
#define __TILE_HPP

class Tile {
public:
	const Image *image;
	int id, x, y;

	/**
	 * Produces hash value for contents of tile.
	 * Very basic. Only used for avoiding comparing all tiles
	 * during tilemap generation.
	 *
	 * @return Hash value
	 */
	int hash() const {
		int hash;
		unsigned char rgb[3];

		hash = 0;
		for(int iy = 0; iy < 8; ++iy) {
			for(int ix = 0; ix < 8; ++ix) {
				image->getRGB(x*8+ix, y*8+iy, rgb);
				hash += rgb[0];
				hash += rgb[1];
				hash += rgb[2];
			}
		}

		return hash;
	}

	/**
	 * Compares the data of two tiles for equality.
	 *
	 * @param a First Tile
	 * @param b Second Tile
	 * @return True if Tiles are equal, false otherwise
	 */
	static bool compare(const Tile& a, const Tile& b) {
		unsigned char argb[3], brgb[3];
		for(int iy = 0; iy < 8; ++iy) {
			for(int ix = 0; ix < 8; ++ix) {
				a.image->getRGB(a.x*8+ix, a.y*8+iy, argb);
				b.image->getRGB(b.x*8+ix, b.y*8+iy, brgb);

				for(int i = 0; i < 3; ++i) {
					if(argb[i] != brgb[i]) return false;
				}
			}
		}
		return true;
	}
};

#endif
