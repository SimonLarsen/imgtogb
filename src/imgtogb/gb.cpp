#include <imgtogb/gb.hpp>

namespace imgtogb {
	void convert(size_t x, size_t y, const Image &img, std::array<unsigned char, 16> &out) {
		unsigned char l1, l2;
		for(size_t iy = 0; iy < 8; ++iy) {
			l1 = l2 = 0;

			for(size_t ix = 0; ix < 8; ++ix) {
				unsigned char col = 3 - img(x*8+ix, y*8+iy, 0) / 64;
				l1 |= (col & 1) << (7 - ix);
				l2 |= ((col & 2) >> 1) << (7 - ix);
			}
			out[iy*2] = l1;
			out[iy*2+1] = l2;
		}
	}
}
