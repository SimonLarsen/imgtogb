#include <imgtogb/rle.hpp>

#include <algorithm>

#define MAX_RUN 128

namespace imgtogb {
	void rle_encode(const std::vector<unsigned char> &in, std::vector<unsigned char> &out) {
		out.clear();

		unsigned char run = 1;
		size_t i = 1;
		while(i < in.size()) {
			if(in[i] == in[i-1]) {
				run++;
			} else {
				if(run > 1) {
					out.push_back(in[i-1]);
					out.push_back(in[i-1]);
					out.push_back(run);
				} else {
					out.push_back(in[i-1]);
				}
				run = 1;
			}

			i++;
		}

		out.push_back(in[in.size()-1]);
		if(run > 1) {
			out.push_back(in[in.size()-1]);
			out.push_back(run);
		}
	}
}
