//----------------------------------------------------------------------------
// huffman coding for file compression with tries
// copyright 2010 christopher rasmussen
// university of delaware
//----------------------------------------------------------------------------

#include "Huffman.hh"
#include "lodepng.h"
#include "raw.hh"
#include <iostream>



//----------------------------------------------------------------------------

bool raw_flag = false;
unsigned char q = 0;

//----------------------------------------------------------------------------

// ** FILL THIS FUNCTION IN ** 

// assumes that trie has AT LEAST 2 elements in it

// (1) remove trie in PQ with SMALLEST frequency, assign to first
// (2) remove trie in PQ with NEXT SMALLEST frequency, assign to second
// (3) make new trie node (new_root) and plug first and second tries in as children, 
//     set new_root's frequency to first->frequency + second->frequency, and 
//     insert in new_root in PQ

// hints:
// * look at TrieNode constructor functions
// * see STL priority_queue documentation at http://www.sgi.com/tech/stl/priority_queue.html
//   for more details 
// * see example_function() below for sample calls to trie PQ 


int main(int argc, char **argv)
{
  if (argc < 2) {
    cout << "huffman [-raw, -q 1-254] <filename>\n";
    exit(1);
  }

  // flags?

  for (int i = 1; i < argc; i++) {
    if (!strcmp("-raw", argv[i]))		
      raw_flag = true;
    else if (!strcmp("-q", argv[i]))		
      q = atoi(argv[i+1]);
  }

  string input_filename(argv[argc - 1]);
  string output_filename(argv[argc - 1]);
  Huffman H;

  // DECOMPRESS!!! output will end in .png

  if (input_filename.substr(input_filename.length() - 4, 4) == ".huf") {
    output_filename += ".png";
    // output_filename.replace(output_filename.length() - 4, 4, ".png");
    H.decompress(input_filename, output_filename);
  }

  else if (input_filename.substr(input_filename.length() - 4, 4) == ".raw") {
    output_filename += ".png";
    // output_filename.replace(output_filename.length() - 4, 4, ".png");
    decompress_raw(input_filename, output_filename);
  }

  // COMPRESS!!! output will end in .huf

  else {
    if (input_filename.substr(input_filename.length() - 4, 4) == ".png") {
      if (raw_flag) {
        output_filename.replace(output_filename.length() - 4, 4, ".raw");
        compress_raw(input_filename, output_filename);    
      }
      else {
        output_filename.replace(output_filename.length() - 4, 4, ".huf");
        H.compress(input_filename, output_filename, q);
      }
    }
    else {
      std::cout << "input a file ending in .png, .huf, or .raw!" << std::endl;
      return 1;
    }
  }


  return 1;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
