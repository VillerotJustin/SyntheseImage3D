// Created by villerot on 03/10/25.
// Mathematical 3D vector class implementation

#include "Vector3D.h"
#include "../Math/math_common.h"

namespace geometry {

    // Static constant definitions
    const Vector3D Vector3D::ZERO(0.0, 0.0, 0.0);
    const Vector3D Vector3D::UNIT_X(1.0, 0.0, 0.0);
    const Vector3D Vector3D::UNIT_Y(0.0, 1.0, 0.0);
    const Vector3D Vector3D::UNIT_Z(0.0, 0.0, 1.0);

    // Constructors
    Vector3D::Vector3D() {
        components[0] = 0.0;
        components[1] = 0.0;
        components[2] = 0.0;
    }

    Vector3D::Vector3D(double x, double y, double z) {
        components[0] = x;
        components[1] = y;
        components[2] = z;
    }

    Vector3D::Vector3D(const math::Vector<double>& data) {
        if (data.size() != 3) {
            throw std::invalid_argument("Vector must be of size 3 to convert to Vector3D");
        }
        components[0] = data[0];
        components[1] = data[1];
        components[2] = data[2];
    }

    // Getter methods
    double Vector3D::x() const {
        return components[0];
    }

    double Vector3D::y() const {
        return components[1];
    }

    double Vector3D::z() const {
        return components[2];
    }

    double Vector3D::at(int index) const {
        if (index < 0 || index >= 3) {
            throw std::out_of_range("Vector3D index out of bounds");
        }
        return components[index];
    }

    // Setter methods
    void Vector3D::setX(double x) {
        components[0] = x;
    }

    void Vector3D::setY(double y) {
        components[1] = y;
    }

    void Vector3D::setZ(double z) {
        components[2] = z;
    }

    void Vector3D::set(double x, double y, double z) {
        components[0] = x;
        components[1] = y;
        components[2] = z;
    }

    // Comparison operators
    bool Vector3D::operator==(const Vector3D& other) const {
        return std::abs(components[0] - other.components[0]) < EPSILON &&
               std::abs(components[1] - other.components[1]) < EPSILON &&
               std::abs(components[2] - other.components[2]) < EPSILON;
    }

    bool Vector3D::operator!=(const Vector3D& other) const {
        return !(*this == other);
    }

    // Arithmetic operators
    Vector3D Vector3D::operator+(const Vector3D& other) const {
        return Vector3D(
            components[0] + other.components[0],
            components[1] + other.components[1],
            components[2] + other.components[2]
        );
    }

    Vector3D Vector3D::operator-(const Vector3D& other) const {
        return Vector3D(
            components[0] - other.components[0],
            components[1] - other.components[1],
            components[2] - other.components[2]
        );
    }

    Vector3D Vector3D::operator*(double scalar) const {
        return Vector3D(
            components[0] * scalar,
            components[1] * scalar,
            components[2] * scalar
        );
    }

    Vector3D Vector3D::operator/(double scalar) const {
        if (std::abs(scalar) < EPSILON) {
            throw std::invalid_argument("Cannot divide by zero");
        }
        return Vector3D(
            components[0] / scalar,
            components[1] / scalar,
            components[2] / scalar
        );
    }

    Vector3D Vector3D::operator-() const {
        return Vector3D(-components[0], -components[1], -components[2]);
    }

    // Vector operations
    double Vector3D::dot(const Vector3D& other) const {
        return components[0] * other.components[0] +
               components[1] * other.components[1] +
               components[2] * other.components[2];
    }

    Vector3D Vector3D::cross(const Vector3D& other) const {
        double cx = components[1] * other.components[2] - components[2] * other.components[1];
        double cy = components[2] * other.components[0] - components[0] * other.components[2];
        double cz = components[0] * other.components[1] - components[1] * other.components[0];
        return Vector3D(cx, cy, cz);
    }

    double Vector3D::length() const {
        return std::sqrt(lengthSquared());
    }

    double Vector3D::lengthSquared() const {
        return components[0] * components[0] +
               components[1] * components[1] +
               components[2] * components[2];
    }

    Vector3D Vector3D::normal() const {
        double len = length();
        if (len < EPSILON) {
            throw std::invalid_argument("Cannot normalize a zero-length vector");
        }
        return *this / len;
    }

    void Vector3D::normalize() {
        double len = length();
        if (len < EPSILON) {
            throw std::invalid_argument("Cannot normalize a zero-length vector");
        }
        components[0] /= len;
        components[1] /= len;
        components[2] /= len;
    }

    double Vector3D::distance(const Vector3D& other) const {
        return std::sqrt(squaredDistance(other));
    }

    double Vector3D::squaredDistance(const Vector3D& other) const {
        Vector3D diff = *this - other;
        return diff.lengthSquared();
    }

    bool Vector3D::parallel(const Vector3D& other) const {
        Vector3D crossProd = this->cross(other);
        return crossProd.zero();
    }

    double Vector3D::angle(const Vector3D& other) const {
        double dotProd = this->dot(other);
        double magnitudes = this->length() * other.length();
        if (magnitudes < EPSILON) {
            throw std::invalid_argument("Cannot calculate angle with zero-length vector");
        }
        // Clamp to valid range for acos to handle floating point errors
        double cosAngle = dotProd / magnitudes;
        cosAngle = std::max(-1.0, std::min(1.0, cosAngle));
        return std::acos(cosAngle);
    }

    bool Vector3D::zero() const {
        return std::abs(components[0]) < EPSILON &&
               std::abs(components[1]) < EPSILON &&
               std::abs(components[2]) < EPSILON;
    }

    math::Vector<double> Vector3D::toVector() const {
        math::Vector<double> result(3);
        result[0] = components[0];
        result[1] = components[1];
        result[2] = components[2];
        return result;
    }

    // Non-member functions
    Vector3D operator*(double scalar, const Vector3D& vector) {
        return vector * scalar;
    }

    std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
        os << "[" << v.components[0] << ", " << v.components[1] << ", " << v.components[2] << "]";
        return os;
    }

} // namespace geometry