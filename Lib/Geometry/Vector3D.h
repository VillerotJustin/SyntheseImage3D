// Created by villerot on 03/10/25.
// Mathematical 3D vector class with geometric operations

#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <iostream>
#include <cmath>
#include <stdexcept>
#include "../Math/Vector.hpp"

namespace geometry {

    /**
     * @class Vector3D
     * @brief A specialized 3D mathematical vector class with geometric operations.
     *
     * This class provides mathematical vector operations for 3D geometry including
     * dot product, cross product, normalization, and other geometric computations.
     * Unlike the template Vector class which manages pointers, this class stores
     * actual double values for mathematical computations.
     */
    class Vector3D {
    public:
        /**
         * @brief Default constructor that creates a 3D vector with all elements initialized to 0.0.
         */
        Vector3D();

        /**
         * @brief Constructor that creates a 3D vector with specified x, y, z values.
         * @param x The x-coordinate of the vector.
         * @param y The y-coordinate of the vector.
         * @param z The z-coordinate of the vector.
         */
        Vector3D(double x, double y, double z);

        /**
         * @brief Constructor that creates a Vector3D from a Vector template.
         * @param data Vector containing the data [x, y, z].
         * @throws std::invalid_argument if data size is not 3.
         */
        explicit Vector3D(const math::Vector<double>& data);

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
         * @brief Accessor method to get the value at a specific index.
         * @param index The index (0=x, 1=y, 2=z).
         * @return The value at the specified index.
         * @throws std::out_of_range if index is out of bounds.
         */
        [[nodiscard]] double at(int index) const;

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

        /**
         * @brief Equality operator that compares two vectors.
         * @param other The vector to compare with.
         * @return True if vectors are equal (within epsilon), false otherwise.
         */
        bool operator==(const Vector3D& other) const;

        /**
         * @brief Inequality operator that compares two vectors.
         * @param other The vector to compare with.
         * @return True if vectors are not equal, false otherwise.
         */
        bool operator!=(const Vector3D& other) const;

        /**
         * @brief Addition operator that adds two vectors element-wise.
         * @param other The vector to add.
         * @return A new vector containing the result of the addition.
         */
        Vector3D operator+(const Vector3D& other) const;

        /**
         * @brief Subtraction operator that subtracts two vectors element-wise.
         * @param other The vector to subtract.
         * @return A new vector containing the result of the subtraction.
         */
        Vector3D operator-(const Vector3D& other) const;

        /**
         * @brief Scalar multiplication operator.
         * @param scalar The scalar value to multiply by.
         * @return A new vector scaled by the scalar.
         */
        Vector3D operator*(double scalar) const;

        /**
         * @brief Scalar division operator.
         * @param scalar The scalar value to divide by.
         * @return A new vector divided by the scalar.
         * @throws std::invalid_argument if scalar is zero.
         */
        Vector3D operator/(double scalar) const;

        /**
         * @brief Unary minus operator.
         * @return A new vector with negated components.
         */
        Vector3D operator-() const;

        /**
         * @brief Calculate the dot product with another Vector3D.
         * @param other The other Vector3D.
         * @return The dot product of the two vectors.
         */
        double dot(const Vector3D& other) const;

        /**
         * @brief Calculate the cross product with another Vector3D.
         * @param other The other Vector3D.
         * @return A new Vector3D that is the cross product.
         */
        Vector3D cross(const Vector3D& other) const;

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
         * @return A new Vector3D with the same direction but length 1.
         * @throws std::invalid_argument if vector has zero length.
         */
        Vector3D normal() const;

        /**
         * @brief Normalize this vector in place.
         * @throws std::invalid_argument if vector has zero length.
         */
        void normalize();

        /**
         * @brief Calculate the Euclidean distance between two vectors.
         * @param other The other vector.
         * @return The distance between the two vectors.
         */
        double distance(const Vector3D& other) const;

        /**
         * @brief Calculate the squared distance between two vectors.
         * @param other The other vector.
         * @return The squared distance between the two vectors.
         */
        double squaredDistance(const Vector3D& other) const;

        /**
         * @brief Check if this vector is parallel to another Vector3D.
         * @param other The other Vector3D.
         * @return True if the vectors are parallel, false otherwise.
         */
        bool parallel(const Vector3D& other) const;

        /**
         * @brief Calculate the angle (in radians) between two 3D vectors.
         * @param other The other Vector3D.
         * @return The angle between the vectors in radians.
         * @throws std::invalid_argument if either vector has zero length.
         */
        double angle(const Vector3D& other) const;

        /**
         * @brief Check if all elements of the vector are zero.
         * @return True if all elements are zero, false otherwise.
         */
        bool zero() const;

        /**
         * @brief Get the size of the vector (always 3 for Vector3D).
         * @return The size (3).
         */
        [[nodiscard]] int size() const { return 3; }

        /**
         * @brief Convert to Vector template.
         * @return A Vector containing the x, y, z components.
         */
        math::Vector<double> toVector() const;

        /**
         * @brief Output stream operator for debugging and display purposes.
         * @param os The output stream.
         * @param v The vector to output.
         * @return The output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const Vector3D& v);

        // Common 3D vector constants
        static const Vector3D ZERO;
        static const Vector3D UNIT_X;
        static const Vector3D UNIT_Y;
        static const Vector3D UNIT_Z;

    private:
        double components[3]; ///< Array storing x, y, z components
        static constexpr double EPSILON = 1e-9; ///< Epsilon for floating point comparisons
    };

    /**
     * @brief Scalar multiplication with scalar on the left.
     * @param scalar The scalar value.
     * @param vector The vector to multiply.
     * @return A new vector scaled by the scalar.
     */
    Vector3D operator*(double scalar, const Vector3D& vector);

} // namespace geometry

#endif //VECTOR3D_H