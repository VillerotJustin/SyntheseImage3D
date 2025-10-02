// Created by villerot on 23/09/25.

#ifndef VECTOR3_H
#define VECTOR3_H

#include "Vector.h"

namespace math {

    /**
     * @class Vector3
     * @brief A specialized 3D vector class that extends the general Vector class.
     *
     * This class provides convenient access to 3D vector components (x, y, z) and
     * includes specialized methods for 3D vector operations such as cross product.
     */
    class Vector3 : public Vector {
      public:
        /**
         * @brief Default constructor that creates a 3D vector with all elements initialized to 0.0.
         */
        Vector3();

        /**
         * @brief Constructor that creates a 3D vector with specified x, y, z values.
         * @param x The x-coordinate of the vector.
         * @param y The y-coordinate of the vector.
         * @param z The z-coordinate of the vector.
         */
        Vector3(double x, double y, double z);

        /**
         * @brief Constructor that creates a Vector3 from a general Vector.
         * @param v The Vector to convert (must be of size 3).
         * @throws std::invalid_argument if the vector is not of size 3.
         */
        explicit Vector3(const Vector& v);

        /**
         * @brief Get the x-component of the vector.
         * @return The x-coordinate.
         */
        [[nodiscard]] double x() const;

        /**
         * @brief Get the y-component of the vector.
         * @return The y-coordinate.
         */
        [[nodiscard]] double y() const;

        /**
         * @brief Get the z-component of the vector.
         * @return The z-coordinate.
         */
        [[nodiscard]] double z() const;

        /**
         * @brief Set the x-component of the vector.
         * @param x The new x-coordinate.
         */
        void setX(double x);

        /**
         * @brief Set the y-component of the vector.
         * @param y The new y-coordinate.
         */
        void setY(double y);

        /**
         * @brief Set the z-component of the vector.
         * @param z The new z-coordinate.
         */
        void setZ(double z);

        /**
         * @brief Set all components of the vector.
         * @param x The new x-coordinate.
         * @param y The new y-coordinate.
         * @param z The new z-coordinate.
         */
        void set(double x, double y, double z);

        // Vector 3 only function

        /**
         * @brief Calculate the cross product with another Vector3.
         * @param other The other Vector3.
         * @return A new Vector3 that is the cross product of this vector and the other.
         */
        Vector3 cross(const Vector3& other) const;

        /**
         * @brief Check if this vector is parallel to another Vector3.
         * @param other The other Vector3.
         * @return True if the vectors are parallel, false otherwise.
         */
        bool parallel(const Vector3& other) const;

        /**
         * @brief Function to calculate the angle (in radians) between two 3D vectors.
         * @param v The first 3D vector.
         * @param u The second 3D vector.
         * @return The angle between the vectors in radians.
         * @throws std::invalid_argument if the vectors are not 3D or have different sizes.
        */
        double angle (const Vector3& other) const;


        // Vector function overrides

        /**
         * @brief Calculate the dot product with another Vector3.
         * @param other The other Vector3.
         * @return The dot product of this vector and the other.
         */
        double dot(const Vector3& other) const;

        /**
         * @brief Get the length (magnitude) of the vector.
         * @return The length of the vector.
         */
        [[nodiscard]] double length() const;

        /**
         * @brief Get the squared length of the vector (without taking the square root).
         * @return The squared length of the vector.
         */
        [[nodiscard]] double lengthSquared() const;

        /**
         * @brief Normalize the vector (make its length equal to 1).
         * @return A new Vector3 with the same direction but length 1.
         */
        Vector3 normal() const;

        /**
         * @brief Normalize this vector in place.
         */
        void normalize();

        // Override operators to return Vector3 instead of Vector
        Vector3 operator+(const Vector3& other) const;
        Vector3 operator-(const Vector3& other) const;
        Vector3 operator*(double scalar) const;
        
        // Common 3D vector constants
        static const Vector3 ZERO;
        static const Vector3 UNIT_X;
        static const Vector3 UNIT_Y;
        static const Vector3 UNIT_Z;

    };
} // namespace math

#endif //VECTOR3_H


  