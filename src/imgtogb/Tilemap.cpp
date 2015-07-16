#include <iomanip>
#include <cstring>
#include <vector>
#include <array>
#include <algorithm>
#include <imgtogb/Tilemap.hpp>
#include <imgtogb/gb.hpp>

namespace  {
	int tile_hash(const imgtogb::Tile &tile, const imgtogb::Image &img) {
		int hash = 0;
		for(size_t iy = 0; iy < 8; ++iy) {
			for(size_t ix = 0; ix < 8; ++ix) {
				hash += img(tile.x*8+ix, tile.y*8+iy, 0);
			}
		}
		return hash;
	}
}

namespace imgtogb {
	typedef std::multimap<int, Tile> mmap;
	typedef std::pair<mmap::iterator,mmap::iterator> mmap_range;

	Tilemap::Tilemap(
		const Image &img
	)
	: img(&img)
	, tiles_x(img.width() / 8)
	, tiles_y(img.height() / 8)
	, ntiles(0)
	, tilemap(tiles_x, tiles_y)
	{
		for(size_t iy = 0; iy < tiles_y; ++iy) {
			for(size_t ix = 0; ix < tiles_x; ++ix) {
				Tile tile(ix, iy, ntiles);
				int hash = tile_hash(tile, img);

				// Look for existing tiles with same hash
				mmap_range range = map.equal_range(hash);
				bool found = range.first != range.second;

				if(found) {
					found = false;
					for(mmap::iterator it = range.first; it != range.second; ++it) {
						if(compare(tile, it->second, img) == true) {
							found = true;
							tilemap(ix, iy) = it->second.id;
							break;
						}
					}
				}

				// Add new tile if not found
				if(!found) {
					map.insert(std::pair<int,Tile>(hash, tile));
					tilemap(ix, iy) = ntiles++;
				}
			}
		}
	}

	size_t Tilemap::getTilesX() const {
		return tiles_x;
	}

	size_t Tilemap::getTilesY() const {
		return tiles_y;
	}

	size_t Tilemap::getTileDataSize() const {
		return ntiles;
	}

	void Tilemap::getTileMap(std::vector<unsigned char> &out) const {
		out.resize(tilemap.size1() * tilemap.size2());
		for(size_t iy = 0; iy < tilemap.size2(); ++iy) {
			for(size_t ix = 0; ix < tilemap.size1(); ++ix) {
				out[ix + iy*tilemap.size1()] = tilemap(ix, iy);
			}
		}
	}

	void Tilemap::getTileData(std::vector<unsigned char> &out) const {
		std::array<unsigned char, 16> data;

		out.resize(ntiles * 16);
		for(mmap::const_iterator it = map.begin(); it != map.end(); ++it) {
			const Tile &tile = it->second;
			convert(tile.x, tile.y, *img, data);
			std::copy(data.begin(), data.end(), out.begin() + tile.id*16);
		}
	}
}
