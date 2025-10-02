//
// Created by villerot on 02/10/25.
//

#ifndef RGBA_COLOR_H
#define RGBA_COLOR_H

#include "../Math/Vector.h"
#include <iostream>

namespace rendering {

    /**
     * @class RGBA_Color
     * @brief An RGBA color class that extends the Vector class for color representation.
     *
     * This class provides convenient access to RGBA color components (red, green, blue, alpha)
     * and includes basic color operations. Each component is represented as a double value
     * typically in the range [0.0, 1.0] for normalized colors.
     */
    class RGBA_Color : public math::Vector {
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
        explicit RGBA_Color(const math::Vector& v);

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
         * @brief Output stream operator for debugging and display purposes.
         * @param os The output stream.
         * @param color The color to output.
         * @return The output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const RGBA_Color& color);
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
