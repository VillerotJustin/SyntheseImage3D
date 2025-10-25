//
// Created by villerot on 02/10/25.
//

#include "Image.h"
#include <stdexcept>
#include <cstdlib> // For system() command
#include <algorithm>
#include <cstdio>
#include <cstring>

namespace rendering
{
    Image::Image() : width(0), height(0), pixels(0,0) {}


    // Constructor with width and height - initializes all colors to black
    Image::Image(int w, int h) : width(0), height(0), pixels(1, 1) {
        if (w <= 0 || h <= 0)
        {
            throw std::invalid_argument("Image dimensions must be positive");
        }

        width = static_cast<size_t>(w);
        height = static_cast<size_t>(h);
        pixels = math::Matrix<RGBA_Color>(height, width);

        // Initialize all colors to black (0, 0, 0, 1)
        for (size_t y = 0; y < height; ++y)
        {
            for (size_t x = 0; x < width; ++x)
            {
                pixels(y, x) = RGBA_Color(0.0, 0.0, 0.0, 1.0);
            }
        }
    }

    // Constructor from color matrix
    Image::Image(math::Matrix<RGBA_Color> colorMatrix) : pixels(colorMatrix) {
        height = colorMatrix.getRows();
        width = colorMatrix.getCols();

        if (width == 0 || height == 0)
        {
            throw std::invalid_argument("Color matrix must have positive dimensions");
        }
    }

    // Copy constructor (Matrix has its own copy semantics)
    Image::Image(const Image &other) : width(other.width), height(other.height), pixels(other.pixels) {}

    // Constructor from file
    Image::Image(const std::string &filename, const std::string &filePath)
        : width(0), height(0), pixels(1, 1) // initialize pixels to avoid requiring a default ctor
    {
        std::string fullPath = filePath + filename;
        
        // Get image dimensions using ImageMagick identify command
        std::string command = "identify -format \"%w %h\" \"" + fullPath + "\" 2> /dev/null";
        FILE *pipe = popen(command.c_str(), "r");
        if (!pipe)
        {
            throw std::runtime_error("Failed to open pipe for image identification");
        }
        
        int w, h;
        if (fscanf(pipe, "%d %d", &w, &h) != 2) {
            pclose(pipe);
            throw std::runtime_error("Failed to read image dimensions from file: " + fullPath);
        }
        pclose(pipe);
        
        if (w <= 0 || h <= 0) {
            throw std::runtime_error("Invalid image dimensions in file: " + fullPath);
        }
        
        // Initialize image with detected dimensions (assign because pixels was already constructed)
        width = static_cast<size_t>(w);
        height = static_cast<size_t>(h);
        pixels = math::Matrix<RGBA_Color>(height, width);
        
        // Read pixel data using ImageMagick convert command
        // Convert to RGBA text format for easy parsing
        std::string convertCmd = "magick \"" + fullPath + "\" -depth 8 txt:- 2> /dev/null";
        FILE *convertPipe = popen(convertCmd.c_str(), "r");
        if (!convertPipe) {
            throw std::runtime_error("Failed to open pipe for image conversion");
        }
        
        char line[512];
        // Skip the header line
        if (!fgets(line, sizeof(line), convertPipe)) {
            pclose(convertPipe);
            throw std::runtime_error("Failed to read image data header");
        }
        
        // Read pixel data line by line
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                if (!fgets(line, sizeof(line), convertPipe)) {
                    pclose(convertPipe);
                    throw std::runtime_error("Unexpected end of image data");
                }
                
                // Parse line format: "x,y: (r,g,b,a) #RRGGBBAA color_name"
                int px, py, r, g, b, a;
                if (sscanf(line, "%d,%d: (%d,%d,%d,%d)", &px, &py, &r, &g, &b, &a) == 6) {
                    // Convert from 0-255 range to 0.0-1.0 range
                    double red = r / 255.0;
                    double green = g / 255.0;
                    double blue = b / 255.0;
                    double alpha = a / 255.0;
                    
                    pixels(y, x) = RGBA_Color(red, green, blue, alpha);
                } else if (sscanf(line, "%d,%d: (%d,%d,%d)", &px, &py, &r, &g, &b) == 5) {
                    // RGB format without alpha
                    double red = r / 255.0;
                    double green = g / 255.0;
                    double blue = b / 255.0;
                    
                    pixels(y, x) = RGBA_Color(red, green, blue, 1.0);
                } else {
                    pclose(convertPipe);
                    throw std::runtime_error("Failed to parse pixel data at position (" + 
                                           std::to_string(x) + "," + std::to_string(y) + ")");
                }
            }
        }
        
        pclose(convertPipe);
    }

    // Assignment operator
    Image &Image::operator=(const Image &other) {
        if (this == &other)
        {
            return *this;
        }

        width = other.width;
        height = other.height;
        pixels = other.pixels; // Matrix copy assignment

        return *this;
    }

    size_t Image::getWidth() const {
        return width;
    }

    size_t Image::getHeight() const {
        return height;
    }

    size_t Image::getNumPixels() const {
        return width * height;
    }

    bool Image::isValid() const {
        // Valid if dimensions are positive and the internal matrix matches
        if (width == 0 || height == 0) return false;
        if (pixels.getRows() != height || pixels.getCols() != width) return false;
        return true;
    }

    const RGBA_Color Image::getPixel(size_t x, size_t y) const {
        if (x >= width || y >= height)
        {
            throw std::out_of_range("Color coordinates out of bounds");
        }

        return pixels(y, x);
    }

    void Image::setPixel(size_t x, size_t y, const RGBA_Color &color) {
        if (x >= width || y >= height)
        {
            throw std::out_of_range("Color coordinates out of bounds");
        }

        pixels(y, x) = color;
    }

    void Image::fill(const RGBA_Color &fillColor) {
        for (size_t y = 0; y < height; ++y)
        {
            for (size_t x = 0; x < width; ++x)
            {
                pixels(y, x) = fillColor;
            }
        }
    }

    void Image::clear() {
        fill(RGBA_Color(0.0, 0.0, 0.0, 1.0)); // Fill with black
    }

    void Image::resize(size_t newWidth, size_t newHeight) {
        if (newWidth == 0 || newHeight == 0)
        {
            throw std::invalid_argument("dimensions must be positive");
        }
        // Determine region to keep
        size_t minWidth = std::min(width, newWidth);
        size_t minHeight = std::min(height, newHeight);

        // Preserve old matrix
        math::Matrix<RGBA_Color> old = pixels;

        // Allocate new matrix
        pixels = math::Matrix<RGBA_Color>(newHeight, newWidth);

        // Copy preserved pixels and initialize new ones to black
        for (size_t y = 0; y < newHeight; ++y) {
            for (size_t x = 0; x < newWidth; ++x) {
                if (y < minHeight && x < minWidth) {
                    pixels(y, x) = old(y, x);
                } else {
                    pixels(y, x) = RGBA_Color(0.0, 0.0, 0.0, 1.0);
                }
            }
        }

        width = newWidth;
        height = newHeight;
    }

    void Image::toGrayscale() {
        for (size_t y = 0; y < height; ++y)
        {
            for (size_t x = 0; x < width; ++x)
            {
                pixels(y, x) = pixels(y, x).toGrayscale();
            }
        }
    }

    void Image::toBlackAndWhite(double threshold) {
        for (size_t y = 0; y < this->getHeight(); ++y)
        {
            for (size_t x = 0; x < this->getWidth(); ++x)
            {
                const RGBA_Color color = getPixel(x, y);
                double gray = 0.299 * color.r() + 0.587 * color.g() + 0.114 * color.b();
                RGBA_Color bwColor = (gray >= threshold) ? RGBA_Color(1.0, 1.0, 1.0, color.a()) : RGBA_Color(0.0, 0.0, 0.0, color.a());
                this->setPixel(x, y, bwColor);
            }
        }
    }

    void Image::invertColors() {
        for (size_t y = 0; y < height; ++y)
        {
            for (size_t x = 0; x < width; ++x)
            {
                pixels(y, x).invert();
            }
        }
    }

    void Image::toBitmapFile(const std::string &filename, const std::string &filePath) const {
        // BMP file format specifications
        const int headerSize = 54;                              // BMP header size
        const int bytesPerPixel = 4;                            // RGBA (we'll write 32bpp)
        int w_i = static_cast<int>(width);
        int h_i = static_cast<int>(height);
        const int rowSize = (w_i * bytesPerPixel + 3) & (~3); // Row size must be a multiple of 4
        const int dataSize = rowSize * h_i;
        const int fileSize = headerSize + dataSize;

        unsigned char header[headerSize] = {
            'B', 'M',             // Signature
            0, 0, 0, 0,           // File size
            0, 0,                 // Reserved1
            0, 0,                 // Reserved2
            headerSize, 0, 0, 0,  // Offset to pixel data
            40, 0, 0, 0,          // DIB header size
            0, 0, 0, 0,           // Width
            0, 0, 0, 0,           // Height
            1, 0,                 // Color planes
            bytesPerPixel * 8, 0, // Bits per pixel
            0, 0, 0, 0,           // Compression (none)
            0, 0, 0, 0,           // Image size (can be zero for uncompressed)
            0x13, 0x0B, 0, 0,     // Horizontal resolution (2835 pixels/meter)
            0x13, 0x0B, 0, 0,     // Vertical resolution (2835 pixels/meter)
            0, 0, 0, 0,           // Number of colors in palette (default)
            0, 0, 0, 0            // Important colors (all)
        };

        // Fill in file size (little endian)
        header[2] = (unsigned char)(fileSize & 0xFF);
        header[3] = (unsigned char)((fileSize >> 8) & 0xFF);
        header[4] = (unsigned char)((fileSize >> 16) & 0xFF);
        header[5] = (unsigned char)((fileSize >> 24) & 0xFF);

        // Fill in width and height (little endian)
        header[18] = (unsigned char)(w_i & 0xFF);
        header[19] = (unsigned char)((w_i >> 8) & 0xFF);
        header[20] = (unsigned char)((w_i >> 16) & 0xFF);
        header[21] = (unsigned char)((w_i >> 24) & 0xFF);
        header[22] = (unsigned char)(h_i & 0xFF);
        header[23] = (unsigned char)((h_i >> 8) & 0xFF);
        header[24] = (unsigned char)((h_i >> 16) & 0xFF);
        header[25] = (unsigned char)((h_i >> 24) & 0xFF);

        // Normalize filePath to ensure it ends with a directory separator
        std::string dirPathPrefix = filePath;
        if (!dirPathPrefix.empty()) {
            char last = dirPathPrefix.back();
            if (last != '/' && last != '\\') {
                dirPathPrefix.push_back('/');
            }
        }

        // Create directory if it doesn't exist
        std::string fullPath = dirPathPrefix + filename + ".bmp";

        // Extract directory path
        size_t lastSlash = fullPath.find_last_of("/\\");
        if (lastSlash != std::string::npos)
        {
            std::string dirPath = fullPath.substr(0, lastSlash);

            // Create directory recursively using system command
            // This is a simple cross-platform approach
            #ifdef _WIN32
                std::string mkdirCmd = "mkdir \"" + dirPath + "\" 2>nul";
            #else
                std::string mkdirCmd = "mkdir -p \"" + dirPath + "\" 2>/dev/null";
            #endif
            system(mkdirCmd.c_str());
        }

        // Open file for binary writing
        FILE *file = fopen(fullPath.c_str(), "wb");
        if (!file)
        {
            throw std::runtime_error("Failed to open file for writing: " + fullPath);
        }

        // Write header
        fwrite(header, sizeof(unsigned char), headerSize, file);

        // Write pixel data (bottom-up)
        unsigned char *rowData = new unsigned char[rowSize];
        for (int yi = 0; yi < h_i; ++yi)
        {
            int y = h_i - 1 - yi; // bottom-up
            // Fill row data
            for (int x = 0; x < w_i; ++x)
            {
                const RGBA_Color color = pixels(static_cast<size_t>(y), static_cast<size_t>(x));
                rowData[x * 4 + 0] = static_cast<unsigned char>(std::clamp(color.b() * 255.0, 0.0, 255.0)); // Blue
                rowData[x * 4 + 1] = static_cast<unsigned char>(std::clamp(color.g() * 255.0, 0.0, 255.0)); // Green
                rowData[x * 4 + 2] = static_cast<unsigned char>(std::clamp(color.r() * 255.0, 0.0, 255.0)); // Red
                rowData[x * 4 + 3] = static_cast<unsigned char>(std::clamp(color.a() * 255.0, 0.0, 255.0)); // Alpha
            }
            // Pad row with zeros if necessary
            for (int p = w_i * 4; p < rowSize; ++p)
            {
                rowData[p] = 0;
            }
            fwrite(rowData, sizeof(unsigned char), rowSize, file);
        }

        delete[] rowData;
        fclose(file);
    }

    void Image::toPngFile(const std::string &filename, const std::string &filePath) const {
        std::string fullPath = filePath + filename + ".png";
        
        // Create directory if it doesn't exist
        size_t lastSlash = fullPath.find_last_of("/\\");
        if (lastSlash != std::string::npos)
        {
            std::string dirPath = fullPath.substr(0, lastSlash);
            #ifdef _WIN32
                std::string mkdirCmd = "mkdir \"" + dirPath + "\" 2>nul";
            #else
                std::string mkdirCmd = "mkdir -p \"" + dirPath + "\" 2>/dev/null";
            #endif
            system(mkdirCmd.c_str());
        }
        
        // Create a temporary BMP file first
        std::string tempBmpPath = filePath + filename + ".tmp.bmp";
        toBitmapFile(filename + ".tmp", filePath);
        
        // Convert BMP to PNG using ImageMagick
        std::string convertCmd = "magick \"" + tempBmpPath + "\" \"" + fullPath + "\"";
        int result = system(convertCmd.c_str());
        
        // Check if conversion was successful before cleaning up
        if (result != 0) {
            // Clean up temporary BMP file before throwing
            #ifdef _WIN32
                std::string removeCmd = "del \"" + tempBmpPath + "\" 2>nul";
            #else
                std::string removeCmd = "rm -f \"" + tempBmpPath + "\" 2>/dev/null";
            #endif
            system(removeCmd.c_str());
            throw std::runtime_error("Failed to convert image to PNG format. ImageMagick might not be installed or accessible.");
        }
        
        // Clean up temporary BMP file only after successful conversion
        #ifdef _WIN32
            std::string removeCmd = "del \"" + tempBmpPath + "\" 2>nul";
        #else
            std::string removeCmd = "rm -f \"" + tempBmpPath + "\" 2>/dev/null";
        #endif
        system(removeCmd.c_str());
    }

    void Image::toJpegFile(const std::string &filename, const std::string &filePath) const {
        std::string fullPath = filePath + filename + ".jpg";
        
        // Create directory if it doesn't exist
        size_t lastSlash = fullPath.find_last_of("/\\");
        if (lastSlash != std::string::npos)
        {
            std::string dirPath = fullPath.substr(0, lastSlash);
            #ifdef _WIN32
                std::string mkdirCmd = "mkdir \"" + dirPath + "\" 2>nul";
            #else
                std::string mkdirCmd = "mkdir -p \"" + dirPath + "\" 2>/dev/null";
            #endif
            system(mkdirCmd.c_str());
        }
        
        // Create a temporary BMP file first
        std::string tempBmpPath = filePath + filename + ".tmp.bmp";
        toBitmapFile(filename + ".tmp", filePath);
        
        // Convert BMP to JPEG using ImageMagick with quality setting
        std::string convertCmd = "magick \"" + tempBmpPath + "\" -quality 90 \"" + fullPath + "\"";
        int result = system(convertCmd.c_str());
        
        // Check if conversion was successful before cleaning up
        if (result != 0) {
            // Clean up temporary BMP file before throwing
            #ifdef _WIN32
                std::string removeCmd = "del \"" + tempBmpPath + "\" 2>nul";
            #else
                std::string removeCmd = "rm -f \"" + tempBmpPath + "\" 2>/dev/null";
            #endif
            system(removeCmd.c_str());
            throw std::runtime_error("Failed to convert image to JPEG format. ImageMagick might not be installed or accessible.");
        }
        
        // Clean up temporary BMP file only after successful conversion
        #ifdef _WIN32
            std::string removeCmd = "del \"" + tempBmpPath + "\" 2>nul";
        #else
            std::string removeCmd = "rm -f \"" + tempBmpPath + "\" 2>/dev/null";
        #endif
        system(removeCmd.c_str());
    }

    void Image::toTiffFile(const std::string &filename, const std::string &filePath) const {
        std::string fullPath = filePath + filename + ".tiff";
        
        // Create directory if it doesn't exist
        size_t lastSlash = fullPath.find_last_of("/\\");
        if (lastSlash != std::string::npos)
        {
            std::string dirPath = fullPath.substr(0, lastSlash);
            #ifdef _WIN32
                std::string mkdirCmd = "mkdir \"" + dirPath + "\" 2>nul";
            #else
                std::string mkdirCmd = "mkdir -p \"" + dirPath + "\" 2>/dev/null";
            #endif
            system(mkdirCmd.c_str());
        }
        
        // Create a temporary BMP file first
        std::string tempBmpPath = filePath + filename + ".tmp.bmp";
        toBitmapFile(filename + ".tmp", filePath);
        
        // Convert BMP to TIFF using ImageMagick with LZW compression
        std::string convertCmd = "magick \"" + tempBmpPath + "\" -compress lzw \"" + fullPath + "\"";
        int result = system(convertCmd.c_str());
        
        // Check if conversion was successful before cleaning up
        if (result != 0) {
            // Clean up temporary BMP file before throwing
            #ifdef _WIN32
                std::string removeCmd = "del \"" + tempBmpPath + "\" 2>nul";
            #else
                std::string removeCmd = "rm -f \"" + tempBmpPath + "\" 2>/dev/null";
            #endif
            system(removeCmd.c_str());
            throw std::runtime_error("Failed to convert image to TIFF format. ImageMagick might not be installed or accessible.");
        }
        
        // Clean up temporary BMP file only after successful conversion
        #ifdef _WIN32
            std::string removeCmd = "del \"" + tempBmpPath + "\" 2>nul";
        #else
            std::string removeCmd = "rm -f \"" + tempBmpPath + "\" 2>/dev/null";
        #endif
        system(removeCmd.c_str());
    }

    Image Image::copy() const {
        return Image(*this); // Use copy constructor
    }

    const math::Matrix<RGBA_Color> &Image::getPixelMatrix() const {
        return pixels;
    }

}