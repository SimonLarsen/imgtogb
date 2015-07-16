#ifndef IMGTOGB_RLE_HPP
#define IMGTOGB_RLE_HPP

#include <vector>

namespace imgtogb {
	void rle_encode(const std::vector<unsigned char> &in, std::vector<unsigned char> &out);
}

#endif
