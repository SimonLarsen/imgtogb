#ifndef IMGTOGB_GB_HPP
#define IMGTOGB_GB_HPP

#include <array>
#include <imgtogb/Image.hpp>

namespace imgtogb {
	void convert(size_t x, size_t y, const Image &img, std::array<unsigned char, 16> &out);
}

#endif
