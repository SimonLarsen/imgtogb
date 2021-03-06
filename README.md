imgtogb
=======

**imgtogb** is a utility for converting images into Game Boy sprites and background tiles.

During conversion the image is converted into four colors based solely on the red channel.
Use a monochrome image with colors `#000000`, `#555555`, `#aaaaaa` and `#ffffff` for best results.

### Compilation ###

```
git clone --recursive https://SimonLarsen@github.com/SimonLarsen/imgtogb.git
cd imgtogb
cmake .
make
```

### 8x16 mode ###

Changing the sprite size with the `--8x16` flag changes the order the sprite tiles appear in sprite VRAM.

```
8x8 mode    8x16 mode
+---+---+   +---+---+
| 0 | 1 |   | 0 | 2 |
+---+---+   +---+---+
| 2 | 3 |   | 1 | 3 |
+---+---+   +---+---+
```

### RLE compression ###

The `--rle` flag will compress the data tables using run-length encoding. A run is denoted by the value repeated twice followed by the run length, while single characters will appear unchanged.

```
Original:
  10000023111111000

Compressed:
  100523116003
```
