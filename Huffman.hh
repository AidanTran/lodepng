//----------------------------------------------------------------------------
// huffman coding for file compression with tries
// copyright 2010 christopher rasmussen
// university of delaware
//----------------------------------------------------------------------------

#include <iostream>
#include <ctype.h>
#include <fstream>
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <tuple>
#include <cstdlib>
#include <string>
#include <math.h>
#include <string.h>

using namespace std;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#define BITS_PER_BYTE                  8
#define BYTES_PER_CHUNK                1      
#define BITS_PER_CHUNK                 (BYTES_PER_CHUNK*BITS_PER_BYTE)

//----------------------------------------------------------------------------

typedef tuple<unsigned char, unsigned char, unsigned char, unsigned char> pixel;


class TrieNode
{
public:

  // default 0-argument constructor

  TrieNode() { parent = NULL; left = NULL; right = NULL; frequency = 0; has_data = false;}

  // note use of default arguments so this constructor can be called with just 2 arguments or the full 5
 
  TrieNode(pixel p, int freq, bool hd, TrieNode *par = NULL, TrieNode *l = NULL, TrieNode *r = NULL)
  { 
    parent = par; 
    left = l;
    right = r;
    frequency = freq; 
    data = p;
    has_data = hd;
  }



  // member variables

  TrieNode *parent;        // parent in tree (NULL if root)
  TrieNode *left, *right;  // children in tree (NULL if leaf)
  int frequency;           // how many occurrences of character
  pixel data;          // R, G, B, A
  string huffcode;         // the Huffman code for this char
  bool has_data;

};

//----------------------------------------------------------------------------

// have to define custom comparison operator on TrieNode pointers or else
// PQ will default to comparing addresses, which would be NOT GOOD

// this will make first element the one with the SMALLEST frequency

class TrieNodePtrCompare
{
public:
  bool operator() (TrieNode * & lhs, TrieNode * & rhs) const
  {
    return lhs->frequency > rhs->frequency;
  }
};

//----------------------------------------------------------------------------

// NOTE: use of "int" type here for counting effectively limits maximum file 
// size this will work with

class Huffman
{
public:

  Huffman();
  void compute_frequencies(vector<unsigned char> &);
  void print_frequencies();
  void build_optimal_trie();
  void merge_two_least_frequent_subtries();
  void print_trie_roots();
  void compute_all_codes_from_trie(TrieNode *);
  int calculate_ascii_file_size();
  int calculate_custom_file_size();
  int calculate_huffman_file_size();
  void print_compression_map();
  void print_decompression_map(ostream &);
  void read_decompression_map(ifstream &);
  void compress(string, string, int);
  void decompress(string, string);
  int binary_2_int(string);
  string int_2_binary(int, int);
  int pad_bit_length(int);
  void write_binary_chunk(string &, ofstream &);

  // optional utility function declarations (not defined for this assignment)

  string compute_code_from_path(TrieNode *);
  int recursive_calculate_huffman_file_size(TrieNode *);

  // first pass: char frequency statistics for file to compress

  map <pixel, int> pixel_counter;     // how many of each char are in the file
  int num_pixels;                 // sum of every entry in char_counter
  int code_table_size;           // how many chars occur at least once

  int huffman_bits;

  int bad_bits_in_last_chunk;   // how many bits in last compressed chunk ARE padding

  // one TrieNode * is the root of a binary tree (aka "trie").
  // a priority queue is used to maintain an entire forest of tries
  // for the Huffman merging procedure

  priority_queue <TrieNode *, vector<TrieNode *>, TrieNodePtrCompare> trie;      

  // these are the mappings between chars and bit codes and vice versa

  map <pixel, string> compression_map;
  map <string, pixel> decompression_map;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
