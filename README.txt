Aidan Tran Project 3 CISC220-80

Use make to compile and make clean to remove all.

./huffman [-raw | -q 1-254] filename

When -raw is flagged any .png file inputted will be decompressed to a .raw file.
Does nothing when a .raw or .huf file is compressed.

The -q flag, when followed by a number 1-254, will attempt to quantize the input. This form of huffman coding is very inefficient and will not work 
if the .png has more than 256 unique pixels. This is a problem as any gradient will destroy this. If you suspect your image has too many unique pixels,
you can pass in a -q x flag which will round each RGBA value in all pixels by the equation: val -= val % x. Use higher numbers for higher levels of compression.

.huf is converted to .png
When .raw is converted to .png
When .png is converted to .huf by default unless -raw is flagged.

This project provides an executible that can decompress png files to ".raw" files, which is the equivalent of storing 4 bytes for every pixel (RGBA). 
This executible also give the option to switch from .png files to .huf files, which is the equivalent of a .raw file but compressed using the Huffman coding techniques
taught in the CISC220 class project. 

This project forks the lodepng GitHub repo for purposes of encoding and decoding png files. This repository provides the option to turn a .png file to a vector of pixels x 4 bytes storing
the raw data of the file. This repo can also take these files and turn them back into .png's and is the main way that I am turning my .raw and .huf files back to .pngs.

The work I did in this project falls into three parts.
1. Creating the I/O code for .raw files
2. Changing the Trie Node and Huffman.hh and Huffman.cpp files to work instead with "pixels" which are tuples of 4 unsigned chars
3. Changing the existing I/O of the Huffman.cpp to work properly with this new "pixel" data type.

