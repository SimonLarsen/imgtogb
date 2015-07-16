#define cimg_display 0

#include <string>
#include <iostream>
#include <iomanip>
#include <array>
#include <tclap/CmdLine.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <imgtogb/StdOutput.hpp>
#include <imgtogb/Image.hpp>
#include <imgtogb/gb.hpp>
#include <imgtogb/Tilemap.hpp>

using namespace imgtogb;

void produceSpriteData(
	const Image &img,
	bool size8x16,
	std::vector<unsigned char> &out
) {
	std::array<unsigned char, 16> data;
	size_t ix, iy;

	size_t tiles_x = img.width() / 8;
	size_t tiles_y = img.height() / 8;

	out.clear();

	ix = iy = 0;
	while(ix < tiles_x && iy < tiles_y) {
		convert(ix, iy, img, data);
		for(unsigned char c : data) {
			out.push_back(c);
		}

		if(size8x16) {
			if(iy % 2 == 1) {
				if(ix == tiles_x-1) {
					ix = 0;
				} else {
					iy -= 2;
					ix++;
				}
			}
			iy++;
		} else {
			ix++;
			if(ix == tiles_x) {
				ix = 0;
				iy++;
			}
		}
	}
}

void emitSpriteCHeader(
	const std::vector<unsigned char> &data,
	const std::string &name,
	std::ostream &os
) {
	std::string name_upper = boost::algorithm::to_upper_copy(name);

	os << "#ifndef " << name_upper << "_SPRITES_H\n";
	os << "#define " << name_upper << "_SPRITES_H\n\n";

	os << "#define " << name << "_data_length " << data.size() / 16 << std::endl;
	os << "const unsigned char " << name << "_data[] = {";

	int i = 0;
	for(unsigned char c : data) {
		if(i % 16 == 0) os << "\n\t";
		os << std::setw(3) << (int)c << ", ";
		i++;
	}

	os << "\n};\n\n";
	os << "#endif" << std::endl;
}

void emitMapCHeader(
	const std::vector<unsigned char> &tilemap,
	size_t tiles_x,
	size_t tiles_y,
	int offset,
	const std::vector<unsigned char> &tiledata,
	const std::string &name,
	std::ostream &os
) {
	std::string name_upper = boost::algorithm::to_upper_copy(name);

	os << "#ifndef " << name_upper << "_MAP_H\n";
	os << "#define " << name_upper << "_MAP_H\n\n";

	os << "#define " << name << "_data_length " << tiledata.size() / 16 << std::endl;
	os << "const unsigned char " << name << "_data[] = {";

	int i = 0;
	for(unsigned char c : tiledata) {
		if(i % 16 == 0) os << "\n\t";
		os << std::setw(3) << (int)c << ", ";
		i++;
	}
	os << "\n};\n\n";

	os << "#define " << name << "_tiles_width " << tiles_x << "\n";
	os << "#define " << name << "_tiles_height " << tiles_y << "\n";
	os << "#define " << name << "_offset " << offset << "\n";
	os << "const unsigned char " << name << "_tiles[] = {";

	i = 0;
	for(unsigned char c : tilemap) {
		if(i % tiles_x == 0) os << "\n\t";
		os << std::setw(3) << (int)c + offset << ", ";
		i++;
	}
	os << "\n};\n\n";

	os << "#endif" << std::endl;
}

int main(int argc, const char *argv[]) {
	TCLAP::CmdLine cmd("Convert image to Game Boy sprite or background data",' ',"0.1");
	imgtogb::StdOutput std_output("imgtogb", "Simon Larsen <simonhffh@gmail.com>");
	cmd.setOutput(&std_output);

	TCLAP::ValueArg<int> offsetArg("O", "offset", "Tile map offset.", false, 0, "offset", cmd);
	TCLAP::SwitchArg mapSwitch("", "map", "Produce tile map", false);
	TCLAP::SwitchArg spriteSwitch("", "sprite", "Produce sprite tiles", false);
	cmd.xorAdd(mapSwitch, spriteSwitch);
	TCLAP::SwitchArg size8x16Switch("", "8x16", "Enable 8x16 sprite mode.", cmd);
	TCLAP::SwitchArg rleSwitch("r", "rle", "Compress data using RLE.", cmd);
	TCLAP::UnlabeledValueArg<std::string> imageArg("image", "Image file path", true, "", "IMAGE> <OUTPUT", cmd);

	cmd.parse(argc, argv);

	if(offsetArg.getValue() < 0) {
		std::cerr << "error: Tile map offset must be non-negative." << std::endl;
		return 1;
	}

	Image img(imageArg.getValue().c_str());
	boost::filesystem::path image_path(imageArg.getValue());
	std::string name = boost::filesystem::basename(image_path);

	if(mapSwitch.getValue()) {
		Tilemap map(img);
		std::vector<unsigned char> tilemap, tiledata;
		map.getTileMap(tilemap);
		map.getTileData(tiledata);
		emitMapCHeader(tilemap, map.getTilesX(), map.getTilesY(), offsetArg.getValue(), tiledata, name, std::cout);
	} else {
		std::vector<unsigned char> data;
		produceSpriteData(img, size8x16Switch.getValue(), data);
		emitSpriteCHeader(data, name, std::cout);
	}

	return 0;
}
