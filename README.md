imgtogbmap
==========

Small utility for converting tiled images into gameboy tile data.
It automatically detects and removes identical tiles in the tile map.

The image is converted into four colors based on luminance. While not required it is recommended to use a monochrome image using colors `#000000`, `#555555`, `#aaaaaa` and `#ffffff` for best results.

The only output format currently supported is C headers for use with GBDK. Assembly files for use with RGBDS will be added very soon.

[stb_image](http://nothings.org/stb_image.c) is used for image reading so some images (such as indexed PNGs and progressive JPEGs) will not work.

## Usage ##

    Usage: imgtogbmap [OPTIONS] IMAGE

    Options:
      -h            Print this help text.
      -n NAME       Name of tile map.
      -o FILENAME   Path to output file.

    Output is written to STDOUT if no output file is given.

    If no NAME is given the basename of IMAGE is used.
    (e.g. "monster.png" will produce "monster_tiles" and "monster_data")

## Example ##

![4x4 tiles test image](images/test128x128.png)

Running `imgtogbmap test.png -o test.h` on above image (enlarged 800%) produces the following file

    #ifndef __patterns__
    #define __patterns__
    
    #define patterns_tiles_length 4
    const unsigned char patterns_tiles[] = {
    	0x00, 0x01, 
    	0x00, 0x02, 
    };
    
    #define patterns_data_length 3
    const unsigned char patterns_data[] = {
    	0xff, 0xf0, 0xff, 0xf0, 0xff, 0xf0, 0xff, 0xf0, 0x00, 0xf0, 0x00, 0xf0, 0x00, 0xf0, 0x00, 0xf0, 
    	0x41, 0x48, 0x82, 0x90, 0x04, 0x20, 0x08, 0x41, 0x10, 0x82, 0x20, 0x04, 0x41, 0x09, 0x82, 0x12, 
    	0x00, 0xff, 0x7e, 0xff, 0x42, 0xc3, 0x5a, 0xc3, 0x5a, 0xc3, 0x42, 0xc3, 0x7e, 0xff, 0x00, 0xff, 
    };
    
    #endif
