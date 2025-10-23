//
// Created by villerot on 23/10/25.
//

#ifndef LIGHT_H
#define LIGHT_H

#include "../Math/Vector.hpp"
#include "../Geometry/Vector3D.h"
#include "./RGBA_Color.h"
#include <iostream>

using namespace geometry;

namespace rendering {

    /**
     * @class Light
     * @brief Represents a point light source in 3D space with position and color.
     */
    class Light {
    public:
        /**
         * @brief Constructor for Light
         * @param position The position of the light in 3D space
         * @param color The color of the light (default is white)
         * @param intensity The intensity of the light (default is 1.0)
         */
        Light(const Vector3D& position, const RGBA_Color& color = RGBA_Color(1.0, 1.0, 1.0, 1.0), double intensity = 1.0);

        /**
         * @brief Get the position of the light
         * @return The position as a Vector3D
         */
        const Vector3D& getPosition() const { return position; }

        /**
         * @brief Get the color of the light
         * @return The color as an RGBA_Color
         */
        const RGBA_Color& getColor() const { return color; }

        /**
         * @brief Get the intensity of the light
         * @return The intensity as a double
         */
        double getIntensity() const { return intensity; }

        /**
         * @brief Set the position of the light
         * @param pos The new position as a Vector3D
         */
        void setPosition(const Vector3D& pos) { position = pos; }

        /**
         * @brief Set the color of the light
         * @param col The new color as an RGBA_Color
         */
        void setColor(const RGBA_Color& col) { color = col; }

        /**
         * @brief Set the intensity of the light ! value will be clamped between 0.0 and 1.0
         * @param inten The new intensity as a double
         */
        void setIntensity(double inten) { intensity = std::max(0.0, std::min(1.0, inten)); }

        /**
         * @brief Translate the light by a given vector
         * @param translation The vector to translate the light
         */
        void translate(const Vector3D& translation);

        /**
         * @brief Equality operator to compare two lights
         * @param other The other light to compare with
         * @return True if both lights have the same position, color, and intensity; false otherwise
         */
        bool operator==(const Light& other) const {
            return position == other.position && color == other.color && intensity == other.intensity;
        }

    private:
        Vector3D position;
        RGBA_Color color = RGBA_Color(1.0, 1.0, 1.0, 1.0); // Default to white light
        double intensity = 1.0;
    };

} // namespace rendering

#endif // LIGHT_H