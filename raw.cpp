#include "raw.hh"

using namespace std;

void compress_raw(string in_filename, string out_filename) {
    std::vector<unsigned char> image; //the raw pixels
    unsigned width, height;
    //decode
    unsigned error = lodepng::decode(image, width, height, in_filename);
    //if there's an error, display it
    width = (unsigned short)width;
    height = (unsigned short)height;
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    ofstream outStream;
    unsigned char ucx;


    cout << "COMPRESSING to " << out_filename << endl;
    cout << width << " " << height << endl;
    outStream.open(out_filename.c_str(), ios::binary);

    // Write width and height as header.

    outStream.write((char *) &width, 2);
    outStream.write((char *) &height, 2);

    vector<unsigned char>::iterator curr;
    for(curr = image.begin(); curr != image.end(); curr++) {
        ucx = (*curr);
        outStream.write((char *) &ucx, 1);
    }

    outStream.close();
}

void decompress_raw(string in_filename, string out_filename) { 
    ifstream inStream;
    unsigned char ucx;
    unsigned int width, height;
    unsigned short temp;
    int i, j, file_length, file_pos;

    // Open file and get size of file.
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

    cout << width << " " << height << endl;
    std::vector<unsigned char> image; //the raw pixels
    image.resize(width * height * 4);


    for (int i = 0; i < width * height * 4; i++) {
        inStream.read((char *) &ucx, 1);
        image[i] = ucx;
    }

    lodepng::encode(out_filename, image, width, height);


    inStream.close();
    
}

int example() {
    const char *filename = "testlarge.raw";
    const char *filename2 = "testlargetest.png";
    const char *filename3 = "testlarge.png";

    compress_raw(filename3, filename);
    decompress_raw(filename, filename2);
}

