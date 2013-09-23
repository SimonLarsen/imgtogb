#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <iomanip>

#include "common.hpp"

const char* help_text = 
"Usage: imgtogbspr [OPTIONS] IMAGE\n\n"
"Options:\n"
"  -h            Print this help text.\n"
"  -n NAME       Name of output.\n"
"  -o FILENAME   Path to output file.\n"
"  -s SIZE       Sprite size. 8x8 or 8x16\n\n"
"Output is written to STDOUT if no output file is given.\n\n"
"If no NAME is given the basename of IMAGE is used.\n"
"(e.g. \"enemies.png\" will produce \"enemies_data\")\n";

int main(int argc, char **argv) {
	std::string inputfile, outputfile, name;
	Image img;
	bool writeToFile, sizeTall;
	unsigned char out[16];
	int ix, iy;

	writeToFile = sizeTall = false;
	for(int i = 1; i < argc; ++i) {
		if(!strcmp(argv[i], "-o")) {
			outputfile = argv[++i];
			writeToFile = true;
		} else if(!strcmp(argv[i], "-n")) {
			name = argv[++i];
		} else if(!strcmp(argv[i], "-s")) {
			++i;
			if(!strcmp(argv[i], "8x8")) {
				sizeTall = false;
			} else if(!strcmp(argv[i], "8x16")) {
				sizeTall = true;
			} else {
				std::cerr << "error: Unknown sprite size " << argv[i] << std::endl;
				std::cerr << help_text;
				return 1;
			}
		} else if(argv[i][0] != '-'){
			inputfile = argv[i];
		}
	}
	if(inputfile.length() == 0) {
		std::cerr << "error: Missing input file." << std::endl;
		std::cerr << help_text;
		return 1;
	}
	// Generate name from filename
	if(name.length() == 0) {
		unsigned int start, end;
		start = inputfile.find_last_of('/');
		start = (start < std::string::npos ? start+1 : 0);
		end = inputfile.find_last_of('.');
		end = (end < std::string::npos ? end : inputfile.length()-1);
		name = inputfile.substr(start,end-start);
	}

	// Load image from file
	try {
		img.load(inputfile);
	} catch(ImageLoadException &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	// If 8x16 mode, check if number of number of vertical tiles
	// is divisible by 2.
	if(sizeTall && (img.tilesy % 2 != 0)) {
		std::cerr << "error: The number of vertical tiles is not divisible by two." << std::endl;
		std::cerr << "You must either pad the image or use 8x8 sprite size." << std::endl;
		return 1;
	}

	// Open file if output is given, use std::cout otherwise
	std::ofstream osfile;
	std::ostream &os = writeToFile
		? osfile.open(outputfile.c_str()), osfile : std::cout;

	// Print inclusion guard
	os << "#ifndef __" << name << "_sprites__" << std::endl;
	os << "#define __" << name << "_sprites__" << std::endl << std::endl;

	// Generate sprite data
	os << "#define " << name << "_data_length " << img.tilesx*img.tilesy << std::endl;
	os << "const unsigned char " << name << "_data[] = {" << std::endl;
	os << std::setbase(16) << std::setfill('0');
	ix = iy = 0;
	while(ix < img.tilesx && iy < img.tilesy) {
		os << "\t";
		// Generate tile data
		convertTile(ix*8, iy*8, img, out);
		for(int i = 0; i < 16; ++i) {
			os << "0x" << std::setw(2) << (int)out[i] << ", ";
		}
		// Advance tile
		// TODO Do this in less idiotic way
		if(sizeTall == false) {
			++ix;
			if(ix >= img.tilesx) {
				ix = 0;
				++iy;
			}
		} else {
			if(iy % 2 == 1) {
				if(ix == img.tilesx-1) {
					ix = 0;
				}
				else {
					iy -= 2;
					++ix;
				}
			}
			++iy;
		}
		os << "\n";
	}
	os << "};" << std::endl << std::endl;

	// End header
	os << "#endif" << std::endl;

	// Close output file
	if(writeToFile) osfile.close();

	return 0;
}
