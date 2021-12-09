//----------------------------------------------------------------------------
// huffman coding for file compression with tries
// copyright 2010 christopher rasmussen
// university of delaware
// v. 1.01 -- fix for problem with tellg() on strauss
//----------------------------------------------------------------------------

#include "Huffman.hh"
#include "lodepng.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

extern bool debug_flag;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

// compressed files have .huf suffix added -- assume for now uncompressed

Huffman::Huffman()
{
  ifstream inStream;
  

  code_table_size = 0;
  num_pixels = 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void Huffman::merge_two_least_frequent_subtries()
{
  TrieNode *new_root, *first, *second;

  first = trie.top();
  trie.pop();
  second = trie.top();
  trie.pop();

  new_root = new TrieNode(pixel{0,0,0,0}, first->frequency + second->frequency, false, NULL, first, second);
  first->parent = new_root;
  second->parent = new_root;
  trie.push(new_root);
}

//----------------------------------------------------------------------------


// recursively traverse binary tree rooted at T
// to determine the huffcode string at every leaf

// if T is a leaf, make sure to execute the 
// compression_map and decompression_map assignments

void Huffman::compute_all_codes_from_trie(TrieNode *T)
{
  // if T is a leaf:
  if (T == NULL) {
    return;
  }

  if (T->has_data) {
    compression_map[T->data] = T->huffcode;
    decompression_map[T->huffcode] = T->data;
  }
  if (T->left != NULL) {
    T->left->huffcode = T->huffcode + "0";
    compute_all_codes_from_trie(T->left);
  } 
  if (T->right != NULL) {
    T->right->huffcode = T->huffcode + "1";
    compute_all_codes_from_trie(T->right);
  }
}

//----------------------------------------------------------------------------

// return how many bits huffman code takes

// this is the sum of the frequency * huffcode length
// over every leaf

int Huffman::calculate_huffman_file_size()
{
  TrieNode *root = trie.top();

  return recursive_calculate_huffman_file_size(root);
}

int Huffman::recursive_calculate_huffman_file_size(TrieNode *T) {
  if (T == NULL) {
    return 0;
  }
  if (T->has_data) {
    return T->frequency * T->huffcode.size();
  }

  return recursive_calculate_huffman_file_size(T->left) + recursive_calculate_huffman_file_size(T->right);

}


//----------------------------------------------------------------------------

// read file character by character and keep track of how many times
// each character occurs

// debug with print_frequencies()

void Huffman::compute_frequencies(vector<unsigned char> &image)
{
  map<pixel,int>::iterator it;
  // read chars

  for(long unsigned int i = 0; i < image.size(); i += 4) {
    pixel temp{image[i], image[i+1], image[i+2], image[i+3]};

    it = pixel_counter.find(temp);
    if (it != pixel_counter.end()) {
      (*it).second += 1;
    } else {
      code_table_size++;
      pixel_counter[temp] = 1;
    }
    num_pixels++;
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

// Huffman's algorithm for building trie (aka code table) with minimal
// cost based on character frequencies

// debug with print_trie_roots(), before and/or after merging

void Huffman::build_optimal_trie()
{
  // fill trie PQ "forest"

  for (pair<pixel, int> entry: pixel_counter){ 
    trie.push(new TrieNode(entry.first, entry.second, true));
  }

  // merge least-frequent subtries together until there's only one

  while (trie.size() > 1)
    merge_two_least_frequent_subtries();

  // populate code table

  compute_all_codes_from_trie(trie.top());


  huffman_bits = calculate_huffman_file_size();

  cout << "Huffman bits: " << huffman_bits << endl;

  bad_bits_in_last_chunk = BITS_PER_BYTE - (huffman_bits % BITS_PER_CHUNK);
  if (bad_bits_in_last_chunk == BITS_PER_BYTE)
    bad_bits_in_last_chunk = 0;
}

//----------------------------------------------------------------------------

// print char and frequency info for root node of every trie in PQ "forest".
// does NOT do a full traversal of underlying binary tree

void Huffman::print_trie_roots()
{
  cout<<"trie PQ contains " << trie.size() << " elements.\n";

  TrieNode *x;
  vector <TrieNode *> V;

  V.clear();

  // take each element out of PQ one by one, print it, and store in temporary vector

  while (!trie.empty()) {
    x = trie.top();
    cout << (int)get<0>(x->data) << "|" << (int)get<1>(x->data) << "|" << (int)get<2>(x->data) << "|" << (int)get<3>(x->data) << ": " << x->frequency << endl;  
    trie.pop(); 
    V.push_back(x);
  }

  // re-create PQ from temp vector

  while (!V.empty()) {
    x = V.back();
    V.pop_back();
    trie.push(x);
  }
}

//----------------------------------------------------------------------------

// convert string of 0's and 1's to integer equivalent

int Huffman::binary_2_int(string s)
{
  int i, value;

  value = 0;

  for (i = s.length() - 1; i >= 0; i--) {
    if (s[i] == '1')
      value += pow(2, s.length() - i - 1);
  }

  return value;
}

//----------------------------------------------------------------------------

// convert integer to equivalent string of 0's and 1's.  if string length
// is < n, pad with 0's on left

string Huffman::int_2_binary(int value, int n)
{
  int i, quotient, remainder, orig_value;
  string s;

  orig_value = value;

  do {
    quotient = value / 2;
    remainder = value % 2;

    if (remainder == 0)
      s.insert(0, "0");
    else
      s.insert(0, "1");

    value = quotient;

  } while (quotient != 0);

  while (s.length() < n)
    s.insert(0, "0");

  return s;
}

//----------------------------------------------------------------------------

// iterate through mapping between chars and bitcodes, printing pairs

void Huffman::print_compression_map()
{
  map <pixel, string>::iterator cur;
  pixel c;


  cout << "Printing compression map" << endl;
  for (cur = compression_map.begin(); cur != compression_map.end(); cur++) {
    c = (*cur).first;
    cout << (int)get<0>(c) << "|" << (int)get<1>(c) << "|" << (int)get<2>(c) << "|" << (int)get<3>(c) <<": " << (*cur).second << endl;
  }

  // map <pixel, int>::iterator curr;
  // cout << "Printing pixel_counter map" << endl;
  // for (curr = pixel_counter.begin(); curr != pixel_counter.end(); curr++) {
  //   c = (*curr).first;
  //   cout << (int)get<0>(c) << "|" << (int)get<1>(c) << "|" << (int)get<2>(c) << "|" << (int)get<3>(c) <<": " << (*curr).second << endl;
  // }



}

//----------------------------------------------------------------------------

int Huffman::pad_bit_length(int num_bits)
{
  return BITS_PER_BYTE * (int) ceil((float) num_bits / (float) BITS_PER_BYTE);
}

//----------------------------------------------------------------------------

// iterate through mapping between bitcodes and chars, printing pairs

void Huffman::print_decompression_map(ostream & outStream)
{
  map <string, pixel>::iterator cur;
  pixel c;
  unsigned char ucx;
  string s, t;
  int pad_length, len, i;

    char *buffer = new char[BYTES_PER_CHUNK];

    ucx = decompression_map.size();   // assuming this can't be bigger than 256 
    outStream.write((char *) &ucx, 1);

    for (cur = decompression_map.begin(); cur != decompression_map.end(); cur++) {

      // get bit string from map

      s = (*cur).first;

      // write bit string length

      ucx = s.length();   // again, assuming no code is longer than 256 bits
      outStream.write((char *) &ucx, 1);

      // write bit string, 0-padded on left to nearest byte

      len = s.length();
      pad_length = pad_bit_length(len);
      for (i = 0; i < pad_length - len; i++)
        s.insert(0, "0");
      for (i = 0; i < pad_length; i += BITS_PER_BYTE) {
        t = s.substr(i, BITS_PER_BYTE);
        ucx = binary_2_int(t);   // assuming return value <= 256
        outStream.write((char *) &ucx, 1);
      }

      // write corresponding pixel from map

      c = (*cur).second;
      ucx = get<0>(c);
      outStream.write((char *) &ucx, 1);
      ucx = get<1>(c);
      outStream.write((char *) &ucx, 1);
      ucx = get<2>(c);
      outStream.write((char *) &ucx, 1);
      ucx = get<3>(c);
      outStream.write((char *) &ucx, 1);
    }
}

//----------------------------------------------------------------------------

// iterate through mapping between bitcodes and chars, printing pairs

void Huffman::read_decompression_map(ifstream & inStream)
{
  unsigned char ucx;
  int num_elements, i, j, code_length, pad_length;
  string s;

  decompression_map.clear();

  // binary version

  //  read code table size (must be <= 256)

  inStream.read((char *) &ucx, 1);   
  num_elements = ucx;
  unsigned char buffer[4];

  for (i = 0; i < num_elements; i++) {

    s.clear();

    // read bitstring length (must be <= 256)

    inStream.read((char *) &ucx, 1);    
    code_length = ucx;

    // read bitstring itself, 0-padded on left to nearest byte

    pad_length = pad_bit_length(code_length);
    for (j = 0; j < pad_length; j += BITS_PER_BYTE) {
      inStream.read((char *) &ucx, 1);
      s += int_2_binary(ucx, BITS_PER_BYTE);
    }

    s.erase(0, pad_length - code_length);

    // read pixel that bitstring translates to
    for (char i = 0; i < 4; i++) {
      inStream.read((char *) &ucx, 1);
      buffer[i] = ucx;
    }


    // put in map
    cout << "Reading " << s << ": " << (int)buffer[0] << "|" << (int)buffer[1] << "|" << (int)buffer[2] << "|" << (int)buffer[3] << endl;
    decompression_map[s] = pixel{buffer[0], buffer[1], buffer[2], buffer[3]};
  }
}

//----------------------------------------------------------------------------

// fill char buffer from bitstring s and write to file in binary

void Huffman::write_binary_chunk(string & s, ofstream & outStream)
{
  int i;
  char buffer[BYTES_PER_CHUNK];

  for (i = 0; i < BYTES_PER_CHUNK; i++) 
    buffer[i] = binary_2_int(s.substr(i * BITS_PER_BYTE, BITS_PER_BYTE));

  outStream.write(buffer, BYTES_PER_CHUNK);
}

//----------------------------------------------------------------------------

// read file character by character and keep track of how many times
// each character occurs

void Huffman::compress(string in_filename, string out_filename)
{
  ofstream outStream;
  char c;
  unsigned char ucx;
  int i;
  string char_queue, s;


  cout << "COMPRESSING to " << out_filename << endl;

  // INITIALIZE -- open file to compress

  std::vector<unsigned char> image; //the raw pixels
  unsigned width, height;
  //decode
  unsigned error = lodepng::decode(image, width, height, in_filename);
  //if there's an error, display it
  width = (unsigned short)width;
  height = (unsigned short)height;
  if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

  // FIRST PASS -- compute character statistics, build trie

  compute_frequencies(image);
  build_optimal_trie();

  // SECOND PASS -- rewind to beginning of input, encode to output file

  outStream.open(out_filename.c_str(), ios::binary);

  // Write WIDTH AND HEIGHT

  outStream.write((char *) &width, 2);
  outStream.write((char *) &height, 2);

  // WRITE code table as header
  print_compression_map();
  print_decompression_map(outStream);

  // BINARY ONLY: WRITE excess bit information so decompressor knows when to stop

  ucx = bad_bits_in_last_chunk;
  outStream.write((char *) &ucx, 1);

  // ENCODE body of file

  for (unsigned long int i = 0; i < image.size(); i += 4) {
    pixel temp{image[i], image[i+1], image[i+2], image[i+3]};
    // cout << "Adding " << compression_map[temp] << " to the char_queue" << endl;
    char_queue += compression_map[temp];

    // if we have enough code bits queued up to write a chunk, turn bit string
    // into chars and emit them

    if (char_queue.length() >= BITS_PER_CHUNK) {
      s = char_queue.substr(0, BITS_PER_CHUNK);
      // cout << "Writing binary chunk: " << s << endl;
      write_binary_chunk(s, outStream);
      char_queue.erase(0, BITS_PER_CHUNK);
    }
  }

  // binary only: write remaining data after RIGHT-padding with 0's to fill chunk

  if (char_queue.length() > 0) {
    while (char_queue.length() < BITS_PER_CHUNK)
      char_queue += "0";
    // cout << "Writing char_queue: " << char_queue << endl;
    write_binary_chunk(char_queue, outStream);
  }

  // clean up

  outStream.close();
}

//----------------------------------------------------------------------------

// read file character by character and keep track of how many times
// each character occurs

void Huffman::decompress(string in_filename, string out_filename)
{
  ifstream inStream;
  pixel c;
  unsigned int width, height;
  unsigned char ucx;
  unsigned short temp;
  int i, j, file_length, file_pos;
  vector<unsigned char> image; //the raw pixels

  cout << "DECOMPRESSING to " << out_filename << endl;

  // INITIALIZE 

  inStream.open(in_filename.c_str(), ios::binary);
  if (inStream.fail()) {
      cout << "Failed to open input file\n";
      exit(1);
  }
  inStream.seekg (0, ios::end); // Don't really know what this does but I think it's important
  file_length = inStream.tellg();
  inStream.seekg (0, ios::beg);

  // Grab width and height from file. Uses some bitwise operators which assume it has been stored in a certain endianess.
  inStream.read((char *) &ucx, 1);
  width = ucx;
  inStream.read((char *) &ucx, 1);
  temp = ucx << 8;
  width = width | temp;
  inStream.read((char *) &ucx, 1);
  height = ucx;
  inStream.read((char *) &ucx, 1);
  temp = ucx << 8;
  height = height | temp;

  cout << "width and height: " << width << " " << height << endl;

  // Resize image vector to proper size
  image.resize(width * height * 4);

  // READ code table from header
  
  read_decompression_map(inStream);



  // BINARY ONLY: READ excess bit information so decompressor knows when to stop
  
  inStream.read((char *) &ucx, 1);
  bad_bits_in_last_chunk = ucx;

  // DECODE body of file

  string char_queue, s;
  map <string, pixel>::iterator cur;
  char *buffer = new char[BYTES_PER_CHUNK];
  int bits_read = 0;
  int value, code_len;
  bool got_a_match;
  long unsigned int x = 0;

  while (1) {

    inStream.read(buffer, BYTES_PER_CHUNK);

    // hit end of file

    if (!inStream) {
      if (inStream.gcount() > 0 || char_queue.size() > 0) 
        cout << "binary decompression error: reached end of file with " << inStream.gcount() << " bytes unread and/or non-empty char queue " << char_queue << endl;
      break;
    }

    // in the middle of file

    else {
      bits_read += BITS_PER_BYTE * BYTES_PER_CHUNK;

      // grab a chunk's worth of bytes, turn into binary, and add to character queue
      // (aka "bit stream")

      for (j = 0; j < BYTES_PER_CHUNK; j++) {
        value = (int) buffer[j];
        if (value < 0)
          value = 256 + value;	
        char_queue += int_2_binary(value, BITS_PER_BYTE);
      }

      // if we just got the last bits in the file, snip off padding bits from last byte

      file_pos = inStream.tellg();
      if (file_pos == file_length) {
        char_queue.erase(char_queue.length() - bad_bits_in_last_chunk, bad_bits_in_last_chunk);
        bits_read -= bad_bits_in_last_chunk;
      }

      // can we decode anything in the bit stream we've queued up so far?
      // if we can decode one thing, try again with what's left, and so on until
      // we fail...then we need to read more in before trying again.


      do {

        got_a_match = false;

        // try taking substrings from character queue of increasing length
        // and seeing if they're legal

        
        for (code_len = 1; code_len <= char_queue.length(); code_len++) {
          s = char_queue.substr(0, code_len);
          cur = decompression_map.find(s); 
          if (cur != decompression_map.end()) {
            pixel temp = (*cur).second;
            // cout << "Found pixel " << (int)get<0>(temp) << "|" << (int)get<1>(temp) << "|" << (int)get<2>(temp) << "|" << (int)get<3>(temp) << endl;
            image[x++] = get<0>(temp);
            image[x++] = get<1>(temp);
            image[x++] = get<2>(temp);
            image[x++] = get<3>(temp);
            
            char_queue.erase(0, code_len);
            got_a_match = true;
            break;
          }
        }
      } while (got_a_match);
    }
  }

  // Write to png
  for (x = 0; x < image.size(); x+=4) {
    // cout << (int)image[x] << "|" << (int)image[x+1] << "|" << (int)image[x+2] << "|" << (int)image[x+3] << "|" << endl;
  }
  lodepng::encode(out_filename, image, width, height);

  // clean up

  delete buffer;

  inStream.close();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------