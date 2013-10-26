#include <tclap/CmdLine.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "Image.hpp"
#include "Tile.hpp"
#include "Tileset.hpp"

#include "imgtogb.hpp"

/**
 * Produces Game Boy sprite data for each sprite in image.
 *
 * @param image Image containing sprites
 * @param name Name of sprite map
 * @param os Output stream to emit to
 */
void produceSprites(const Image& image, bool size8x16, const std::string& name, std::ostream& os) {
	int ix, iy;
	unsigned char data[16];

	os << "#ifndef __" << name << "_sprites__" << std::endl;
	os << "#define __" << name << "_sprites__" << std::endl << std::endl;

	os << "#define " << name << "_data_length " << image.getTilesX()*image.getTilesY() << std::endl;
	os << "const unsigned char " << name << "_data[] = {" << std::endl; 

	ix = iy = 0;
	while(ix < image.getTilesX() && iy < image.getTilesY()) {
		os << "\t";

		image.convert(ix, iy, data);
		for(int i = 0; i < 16; ++i) {
			os << std::setw(3) << (int)data[i] << ", ";
		}

		if(size8x16) {
			if(iy % 2 == 1) {
				if(ix == image.getTilesX()-1) {
					ix = 0;
				}
				else {
					iy -= 2;
					++ix;
				}
			}
			++iy;
		} else {
			++ix;
			if(ix >= image.getTilesX()) {
				ix = 0;
				++iy;
			}
		}
		os << std::endl;
	}

	os << "};" << std::endl << std::endl;
	os << "#endif" << std::endl;

}

/**
 * Produces tilemap and tile data for image.
 * Consolidates identical tiles into same tile id
 * to save space.
 *
 * @param image Image of tilemap
 * @param name Name of tilemap
 * @param os Output stream to emit to 
 */
void produceTilemap(const Image& image, int offset, const std::string& name, std::ostream& os) {
	Tileset tileset;
	tileset.generate(image);

	os << "#ifndef __" << name << "_tiles__" << std::endl;
	os << "#define __" << name << "_tiles__" << std::endl << std::endl;
	tileset.emitData(os, name);
	os << std::endl;
	tileset.emitTilemap(os, name, offset);
	os << std::endl;
	os << "#endif" << std::endl;
}

/**
 * Gets the basename of a path without the file ending.
 * Given the path "/home/user/filename.png" the string "filename" is returned.
 *
 * @param path File path
 * @return Name of file
 */
std::string getName(const std::string& path) {
	size_t lastSlash = path.find_last_of('/');
	lastSlash = (lastSlash == std::string::npos ? 0 : lastSlash+1);
	size_t lastBSlash = path.find_last_of('\\');
	lastBSlash = (lastBSlash == std::string::npos ? 0 : lastBSlash+1);
	size_t end = path.find_last_of('.');
	size_t start = std::max(lastSlash, lastBSlash);
	return path.substr(start, end-start);
}

int main(int argc, char **argv) {
	Image image;
	std::string imageFilename, outputFilename, outputName;
	int offset;
	bool flagMap, size8x16;

	// Parse arguments
	try {
		TCLAP::CmdLine cmd("Image to Game Boy data utility",' ',"0.2");

		// Action switches
		TCLAP::SwitchArg mapSwitch("","map","Produce tile map", false);
		TCLAP::SwitchArg spriteSwitch("","sprite","Produce sprite data", false);
		cmd.xorAdd(mapSwitch, spriteSwitch);

		// Value arguments
		TCLAP::UnlabeledValueArg<std::string> imageFilenameArg("image","Image to convert", true, "", "IMAGE", cmd);
		TCLAP::ValueArg<std::string> outputFilenameArg("o","out","Output file", false, "", "FILE", cmd);
		TCLAP::ValueArg<std::string> outputNameArg("n","name","Output name", false, "", "NAME", cmd);
		TCLAP::ValueArg<int> offsetArg("O","offset","Add offset to tile map indices", false, 0, "OFFSET", cmd);

		// Sprite size switches
		TCLAP::SwitchArg size8x16Switch("","8x16", "8x16 sprite mode", cmd);

		cmd.parse(argc, argv);

		// Retrieve values
		imageFilename = imageFilenameArg.getValue();
		outputFilename = outputFilenameArg.getValue();
		outputName = outputNameArg.getValue();
		offset = offsetArg.getValue();
		// Retrieve flags
		flagMap = mapSwitch.getValue();
		size8x16 = size8x16Switch.getValue();

	} catch (TCLAP::ArgException &e) {
		std::cerr << "error: " << e.error() << "for arg " << e.argId() << std::endl;
	}

	// Get output name from file name if none is given.
	if(outputName.length() == 0) {
		outputName = getName(imageFilename);
	}

	// Load input image
	try {
		image.load(imageFilename);
	} catch(const ImageException& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	// Open output file if given
	std::ofstream osfile;
	std::ostream &os = outputFilename.length() > 0
		? osfile.open(outputFilename.c_str()), osfile : std::cout;

	// Produce tile map or sprite data based on flags
	if(flagMap) {
		produceTilemap(image, offset, outputName, os);
	}
	else {
		produceSprites(image, size8x16, outputName, os);
	}

	// Close output file
	osfile.close();

	return 0;
}
