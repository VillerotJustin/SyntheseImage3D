#ifndef IMAGE_H
#define IMAGE_H

#include "../Math/Matrix.hpp"
#include "RGBA_Color.h"

namespace rendering {

    /**
     * @class Image
     * @brief A class representing a 2D image composed of RGBA colors.
     *
     * This class provides functionality for creating, manipulating, and accessing
     * color data in a 2D image format. Uses a Matrix<RGBA_Color> for internal storage.
     */
    class Image {
    public:
        /**
         * @brief Constructs an image with the given width and height.
         * Initializes all colors to black (0, 0, 0, 1).
         * @param w The width of the image in pixels.
         * @param h The height of the image in pixels.
         * @throws std::invalid_argument if width or height is non-positive.
         */
        Image(int w, int h);

        /**
         * @brief Constructs an image from a given matrix of colors.
         * The matrix dimensions determine the image width and height.
         * @param colorMatrix A matrix containing the color data.
         * @throws std::invalid_argument if matrix dimensions are non-positive.
         */
        Image(math::Matrix<RGBA_Color> colorMatrix);

        /**
         * @brief Copy constructor.
         * @param other The image to copy.
         */
        Image(const Image& other);

        /**
         * @brief Constructs an image from a file.
         * @param filename The name of the file to load.
         * @param filePath The path to the file (default is current directory).
         */
        Image(const std::string& filename, const std::string& filePath = "./");

        /**
         * @brief Assignment operator.
         * @param other The image to assign from.
         * @return Reference to this image.
         */
        Image& operator=(const Image& other);

        /**
         * @brief Destructor that cleans up allocated color memory.
         */
        ~Image();

        /**
         * @brief Get the width of the image.
         * @return The width in pixels.
         */
        int getWidth() const;

        /**
         * @brief Get the height of the image.
         * @return The height in pixels.
         */
        int getHeight() const;

        /**
         * @brief Get the total number of pixels in the image.
         * @return The number of pixels (width * height).
         */
        int getNumPixels() const;

        /**
         * @brief Check if the image is valid (no null colors).
         * @return True if all colors are non-null, false otherwise.
         */
        bool isValid() const;

        /**
         * @brief Get a pointer to the color at the specified coordinates.
         * @param x The x-coordinate (column).
         * @param y The y-coordinate (row).
         * @return Pointer to the color at the specified location.
         * @throws std::out_of_range if coordinates are out of bounds.
         */
        const RGBA_Color* getPixel(int x, int y) const;

        /**
         * @brief Set a color at the specified coordinates.
         * @param x The x-coordinate (column).
         * @param y The y-coordinate (row).
         * @param color The color to set.
         * @throws std::out_of_range if coordinates are out of bounds.
         */
        void setPixel(int x, int y, const RGBA_Color& color);

        /**
         * @brief Fill the entire image with a single color.
         * @param fillColor The color to fill the image with.
         */
        void fill(const RGBA_Color& fillColor);

        /**
         * @brief Clear the image to black.
         */
        void clear();

        /**
         * @brief Resize the image to new dimensions.
         * Existing pixel data is discarded and new pixels are initialized to black.
         * @param newWidth The new width in pixels.
         * @param newHeight The new height in pixels.
         * @throws std::invalid_argument if new dimensions are non-positive.
         */
        void resize(int newWidth, int newHeight);

        /**
         * @brief Convert the entire image to grayscale.
         */
        void toGrayscale();

        void toBlackAndWhite(double threshold = 0.5);

        void invertColors();

        void toBitmapFile(const std::string& filename, const std::string& filePath = "./") const;

        void toPngFile(const std::string& filename, const std::string& filePath = "./") const;

        void toJpegFile(const std::string& filename, const std::string& filePath = "./") const;

        void toTiffFile(const std::string& filename, const std::string& filePath = "./") const;

        /**
         * @brief Create a copy of this image.
         * @return A new Image that is a copy of this one.
         */
        Image copy() const;

        /**
         * @brief Get a const reference to the internal color matrix.
         * @return Const reference to the color matrix.
         */
        const math::Matrix<RGBA_Color>& getPixelMatrix() const;

    private:
        int width;                          ///< Width of the image in pixels
        int height;                         ///< Height of the image in pixels
        math::Matrix<RGBA_Color> pixels;    ///< Matrix storing color data as pointers
    };

}

#endif // IMAGE_H