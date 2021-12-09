Aidan Tran Project 3 CISC220-80

This project provides an executible that can decompress png files to ".raw" files, which is the equivalent of storing 4 bytes for every pixel (RGBA). 
This executible also give the option to switch from .png files to .huf files, which is the equivalent of a .raw file but compressed using the Huffman coding techniques
taught in the CISC220 class project. 

This project forks the lodepng GitHub repo for purposes of encoding and decoding png files. This repository provides the option to turn a .png file to a vector of pixels x 4 bytes storing
the raw data of the file. This repo can also take these files and turn them back into .png's and is the main way that I am turning my .raw and .huf files back to .pngs.

The work I did in this project falls into three parts.
1. Creating the I/O code for .raw files
2. Changing the Trie Node and Huffman.hh and Huffman.cpp files to work instead with "pixels" which are tuples of 4 unsigned chars
3. Changing the existing I/O of the Huffman.cpp to work properly with this new "pixel" data type.

