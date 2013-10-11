imgtogb
=======

**imgtogb** is a utility for converting images into Game Boy sprites and background tiles.

During conversion the image is converted into four colors based on luminance. While not required it is recommended to use a monochrome image using colors `#000000`, `#555555`, `#aaaaaa` and `#ffffff` for best results.

The only output format currently supported is C headers for use with GBDK. Assembly files for RGBDS will be added very soon.

[stb_image](http://nothings.org/stb_image.c) is used for image reading so some images (such as indexed PNGs and progressive JPEGs) will not work.

### Usage ###

    USAGE: 

       ./bin/imgtogb  {--map|--sprite} [OPTIONS] <IMAGE>


    OPTIONS: 
       --map
         (OR required)  Produce tile map
         -- OR --
       --sprite
         (OR required)  Produce sprite data


       --8x16
         8x16 sprite mode

       -O <OFFSET>,  --offset <OFFSET>
         Add offset to tile map indices

       -n <NAME>,  --name <NAME>
         Output name

       -o <FILE>,  --out <FILE>
         Output file

       --version
         Displays version information and exits.

       -h,  --help
         Displays usage information and exits.

      <IMAGE>
         (required)  Image to convert

Output is written to STDOUT if no output file is given.
    
Changing the sprite size with the `--size8x16` flag changes the order the sprite tiles appear in sprite VRAM.

    8x8 mode    8x16 mode
    +---+---+   +---+---+
    | 0 | 1 |   | 0 | 2 |
    +---+---+   +---+---+
    | 2 | 3 |   | 1 | 3 |
    +---+---+   +---+---+
