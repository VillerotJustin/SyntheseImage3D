#ifndef IMAGE_H
#define IMAGE_H

#include "Matrix.hpp"

class Image {
public:
    Image(int w, int h) : width(w), height(h) {}

private:
    int width;
    int height;
};

#endif // IMAGE_H