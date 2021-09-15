#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>  
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>
#include "lodepng.h"
using namespace std;
 
int hex2int(char a, char b) {
    a = (a <= '9') ? a - '0'  : (a & 0x7) + 9;
    b = (b <= '9') ? b - '0'  : (b & 0x7) + 9;
    return (a << 4) + b;
}

int retrieve_color(const char * hex, int i) {
    char a = hex[2*i + 1];
    char b = hex[2*i + 2];
    return hex2int(a, b);
}

class ImageScreen {
    int width;
    int height; 
    string filenamestring;
    unsigned char *image; 

    public:
        void set_image(int w, int h, string f) {
            width = w;
            height = h;  
            filenamestring = f; 
            image = (unsigned char *)calloc(width*height*4, sizeof(unsigned char));
        } 
        void write_to_png() {     
            lodepng_encode32_file( filenamestring.c_str() , image, width, height);
        }
        void update_pixel(int x, int y) {
            image[((y * width) + x) * 4 + 0] = 255;
            image[((y * width) + x) * 4 + 1] = 255;
            image[((y * width) + x) * 4 + 2] = 255;
            image[((y * width) + x) * 4 + 3] = 255;
        }
        void update_pixel(int x, int y, int r, int g, int b) {
            image[((y * width) + x) * 4 + 0] = r;
            image[((y * width) + x) * 4 + 1] = g;
            image[((y * width) + x) * 4 + 2] = b;
            image[((y * width) + x) * 4 + 3] = 255;
        } 
 
        //need to understand how to convert hex string to decimal values
        void update_pixel(int x, int y, const char * hex_color_arr) { 
            int r = retrieve_color(hex_color_arr, 0);
            int g = retrieve_color(hex_color_arr, 1);
            int b = retrieve_color(hex_color_arr, 2);  
            image[((y * width) + x) * 4 + 0] = r;
            image[((y * width) + x) * 4 + 1] = g;
            image[((y * width) + x) * 4 + 2] = b;
            image[((y * width) + x) * 4 + 3] = 255;
        }
}; 

class ImageFrames {
    vector<ImageScreen> frame;
    int index = 0;

    public:
        void add_image_screen(ImageScreen image) {
            frame.push_back(image);
        } 
        vector<ImageScreen> get_frame() {
            return frame;
        }
        int get_index() {
            return index;
        } 
        void set_index(int i) {
            index = i;
        }
};      

ImageScreen create_png(int width, int height, string filename) { 
    ImageScreen imagescreen;  
    imagescreen.set_image(width, height, filename);
    return imagescreen;
}

void tokenize(string const &str, const char delim, vector<string> &out) {
    stringstream ss(str);
    string s;
    while(getline(ss, s, delim)) {
        out.push_back(s);
    }
} 
 
string generate_filename(string filename, int i) {
    int length = 3;
    string id = std::to_string(i);
    for(int index = 0; index < length - id.length(); index++) {
        filename += "0";
    } 
    filename = filename + id + ".png";
    cout << filename << '\n';
    return filename;
}

int main(int argc, char** argv) {  
    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL)
        exit(EXIT_FAILURE); 

    ImageFrames image_frame;

    char* line = NULL;
    size_t len = 0;
    while ((getline(&line, &len, fp)) != -1) { 
        vector<string> out;
        tokenize(line, ' ', out); 
        string keyword = out.at(0); 
        if(keyword.compare("frame") == 0) { 
            int index = stoi(out.at(1));
            image_frame.set_index(index);
        }
        else if(keyword.compare("png") == 0) {
            image_frame.get_frame().reserve(1); 
            int width = stoi(out.at(1));
            int height = stoi(out.at(2)); 
            string filename = out.at(3);
            ImageScreen image_screen = create_png(width, height, filename);
            image_frame.add_image_screen(image_screen);
        }
        else if(keyword.compare("pngs") == 0) {
            int width = stoi(out.at(1));
            int height = stoi(out.at(2));
            string filename = out.at(3);
            int frames = stoi(out.at(4));
            image_frame.get_frame().reserve(frames);
            for(int i = 0; i < frames; i++) {
                string new_filename = generate_filename(filename, i);
                ImageScreen image_screen = create_png(width, height, new_filename);
                image_frame.add_image_screen(image_screen);
            } 
        }
        else if(keyword.compare("xy") == 0) {
            int x = stoi(out.at(1));
            int y = stoi(out.at(2));
            image_frame.get_frame().at(image_frame.get_index()).update_pixel(x, y);
        }
        else if(keyword.compare("xyrgb") == 0) {
            int x = stoi(out.at(1));
            int y = stoi(out.at(2));
            int r = stoi(out.at(3));
            int g = stoi(out.at(4));
            int b = stoi(out.at(5));
            image_frame.get_frame().at(image_frame.get_index()).update_pixel(x, y, r, g, b);
        }
        else if(keyword.compare("xyc") == 0) {
            int x = stoi(out.at(1));
            int y = stoi(out.at(2));
            
            const char * hex_color_arr = out.at(3).c_str();  

            image_frame.get_frame().at(image_frame.get_index()).update_pixel(x, y, hex_color_arr);
        }
    }
    fclose(fp);
    if (line)
        free(line);

    for(int i = 0; i < image_frame.get_frame().size(); i++) {
        ImageScreen curr = image_frame.get_frame().at(i);
        curr.write_to_png();
    }
    return 0;
}

 