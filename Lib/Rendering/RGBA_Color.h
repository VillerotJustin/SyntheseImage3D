//
// Created by villerot on 02/10/25.
//

#ifndef RGBA_COLOR_H
#define RGBA_COLOR_H

#include "../Math/Vector.hpp"
#include <iostream>

namespace rendering {

    /**
     * @class RGBA_Color
     * @brief An RGBA color class that provides color-specific operations.
     *
     * This class provides convenient access to RGBA color components (red, green, blue, alpha)
     * and includes only color operations that make semantic sense. Each component is represented 
     * as a double value typically in the range [0.0, 1.0] for normalized colors.
     * 
     * Unlike inheriting from Vector, this class uses composition to avoid exposing 
     * vector operations that don't make sense for colors (like normalization, distance, etc.).
     */
    class RGBA_Color {
    public:
        /**
         * @brief Default constructor that creates an RGBA color with all components set to 0.0 (transparent black).
         */
        RGBA_Color();

        /**
         * @brief Constructor that creates an RGBA color with specified red, green, blue, and alpha values.
         * @param r The red component (typically 0.0 to 1.0).
         * @param g The green component (typically 0.0 to 1.0).
         * @param b The blue component (typically 0.0 to 1.0).
         * @param a The alpha component (typically 0.0 to 1.0).
         */
        RGBA_Color(double r, double g, double b, double a = 1.0);

        /**
         * @brief Constructor that creates an RGBA_Color from a general Vector.
         * @param v The Vector to convert (must be of size 4 for RGBA).
         * @throws std::invalid_argument if the vector is not of size 4.
         */
        explicit RGBA_Color(const math::Vector<double>& v);

        /**
         * @brief Destructor that cleans up allocated memory for the Vector components.
         */
        ~RGBA_Color();

        /**
         * @brief Copy constructor that creates a deep copy of another RGBA_Color.
         * @param other The RGBA_Color to copy.
         */
        RGBA_Color(const RGBA_Color& other);

        /**
         * @brief Assignment operator that performs a deep copy of another RGBA_Color.
         * @param other The RGBA_Color to copy.
         * @return Reference to this RGBA_Color after assignment.
         */
        RGBA_Color& operator=(const RGBA_Color& other);

        // === Component Access ===
        
        /**
         * @brief Get the red component of the color.
         * @return The red component value.
         */
        [[nodiscard]] double r() const;

        /**
         * @brief Get the green component of the color.
         * @return The green component value.
         */
        [[nodiscard]] double g() const;

        /**
         * @brief Get the blue component of the color.
         * @return The blue component value.
         */
        [[nodiscard]] double b() const;

        /**
         * @brief Get the alpha component of the color.
         * @return The alpha component value.
         */
        [[nodiscard]] double a() const;

        /**
         * @brief Set the red component of the color.
         * @param red The new red component value.
         */
        void setR(double red);

        /**
         * @brief Set the green component of the color.
         * @param green The new green component value.
         */
        void setG(double green);

        /**
         * @brief Set the blue component of the color.
         * @param blue The new blue component value.
         */
        void setB(double blue);

        /**
         * @brief Set the alpha component of the color.
         * @param alpha The new alpha component value.
         */
        void setA(double alpha);

        /**
         * @brief Set all RGBA components at once.
         * @param red The red component value.
         * @param green The green component value.
         * @param blue The blue component value.
         * @param alpha The alpha component value.
         */
        void setRGBA(double red, double green, double blue, double alpha = 1.0);

        /**
         * @brief Invert the color (1.0 - component) for RGB, alpha remains unchanged.
         */
        void invert();

        // === Color-Specific Operations ===

        /**
         * @brief Add two colors component-wise (color mixing).
         * @param other The color to add.
         * @return A new color with the sum of components.
         */
        RGBA_Color operator+(const RGBA_Color& other) const;

        /**
         * @brief Subtract two colors component-wise.
         * @param other The color to subtract.
         * @return A new color with the difference of components.
         */
        RGBA_Color operator-(const RGBA_Color& other) const;

        /**
         * @brief Multiply color by a scalar (brightness scaling).
         * @param scalar The scalar to multiply by.
         * @return A new color with scaled components.
         */
        RGBA_Color operator*(double scalar) const;

        /**
         * @brief Multiply two colors component-wise (color filtering).
         * @param other The color to multiply with.
         * @return A new color with multiplied components.
         */
        RGBA_Color operator*(const RGBA_Color& other) const;

        /**
         * @brief Check if two colors are equal.
         * @param other The color to compare with.
         * @return True if colors are equal, false otherwise.
         */
        bool operator==(const RGBA_Color& other) const;

        /**
         * @brief Check if two colors are not equal.
         * @param other The color to compare with.
         * @return True if colors are not equal, false otherwise.
         */
        bool operator!=(const RGBA_Color& other) const;

        /**
         * @brief Clamp all color components to the range [0.0, 1.0].
         * @return A new RGBA_Color with clamped components.
         */
        RGBA_Color clamp() const;

        /**
         * @brief Convert the color to grayscale using standard luminance weights.
         * @return A new grayscale RGBA_Color (R=G=B=luminance, A=original alpha).
         */
        RGBA_Color toGrayscale(const double& rw = 0.299, const double& gw = 0.587, const double& bw = 0.114) const;

        /**
         * @brief Linearly interpolate between this color and another.
         * @param other The target color.
         * @param t The interpolation factor (0.0 = this color, 1.0 = other color).
         * @return A new interpolated color.
         */
        RGBA_Color lerp(const RGBA_Color& other, double t) const;

        /**
         * @brief Blend this color with another using alpha blending.
         * @param background The background color.
         * @return A new blended color.
         */
        RGBA_Color alphaBlend(const RGBA_Color& background) const;

        /**
         * @brief Get the underlying vector representation.
         * @return Const reference to the internal vector.
         */
        [[nodiscard]] const math::Vector<double>& asVector() const;

        /**
         * @brief Output stream operator for debugging and display purposes.
         * @param os The output stream.
         * @param color The color to output.
         * @return The output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const RGBA_Color& color);

        /**
         * @brief Scalar multiplication (scalar * color).
         * @param scalar The scalar value.
         * @param color The color.
         * @return A new scaled color.
         */
        friend RGBA_Color operator*(double scalar, const RGBA_Color& color);

    private:
        math::Vector<double> components; ///< Internal storage for RGBA components
    };

    // Convenience functions for common colors
    namespace Colors {
        /**
         * @brief Create a black color (0, 0, 0, 1).
         */
        RGBA_Color black();

        /**
         * @brief Create a white color (1, 1, 1, 1).
         */
        RGBA_Color white();

        /**
         * @brief Create a red color (1, 0, 0, 1).
         */
        RGBA_Color red();

        /**
         * @brief Create a green color (0, 1, 0, 1).
         */
        RGBA_Color green();

        /**
         * @brief Create a blue color (0, 0, 1, 1).
         */
        RGBA_Color blue();

        /**
         * @brief Create a transparent color (0, 0, 0, 0).
         */
        RGBA_Color transparent();
    }
}

#endif //RGBA_COLOR_H
