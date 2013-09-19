imgtogbmap
==========

Small utility for converting tiled images into gameboy tile data.
It automatically detects and removes identical tiles in the tile map.

[stb_image](http://nothings.org/stb_image.c) is used for image reading so some images (such as indexed PNGs and progressive JPEGs) will not work.

The only output format currently supported is C headers for use with GBDK. Assembly files for use with RGBDS will be added very soon.

## Usage ##

    Usage: imgtogbmap [OPTIONS] IMAGE

    Options:
      -h            Print this help text.
      -n NAME       Name of tile map.
      -o FILENAME   Path to output file.

    Output is written to STDOUT if no output file is given.

    If no NAME is given the basename of IMAGE is used.
    (e.g. "monster.png" will produce "monster_tiles" and "monster_data")
