#ifndef IMGTOGB_TILE_HPP
#define IMGTOGB_TILE_HPP

#include <imgtogb/Image.hpp>

namespace imgtogb {
	class Tile {
		public:
			Tile(size_t x, size_t y, int id)
			: x(x), y(y), id(id) { }

			size_t x, y;
			int id;
	};

	inline bool compare(const Tile &a, const Tile &b, const Image &img) {
		for(size_t iy = 0; iy < 8; ++iy) {
			for(size_t ix = 0; ix < 8; ++ix) {
				unsigned char r1 = img(a.x*8+ix, a.y*8+iy, 0);
				unsigned char r2 = img(b.x*8+ix, b.y*8+iy, 0);

				if(r1 != r2) return false;
			}
		}

		return true;
	}
}

#endif
