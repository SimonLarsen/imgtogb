#ifndef IMGTOGB_TILEMAP_HPP
#define IMGTOGB_TILEMAP_HPP

#include <map>
#include <ostream>
#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include <imgtogb/Image.hpp>
#include <imgtogb/Tile.hpp>

namespace imgtogb {
	class Tilemap {
		public:
			Tilemap(const Image &img, int offset);

			size_t getTilesX() const;
			size_t getTilesY() const;
			size_t getTileDataSize() const;
			void getTileMap(std::vector<unsigned char> &out) const;
			void getTileData(std::vector<unsigned char> &out) const;

		private:
			const Image *img;
			int offset;
			size_t tiles_x, tiles_y, ntiles;
			boost::numeric::ublas::matrix<int> tilemap;
			std::multimap<int, Tile> map;
	};
}

#endif
