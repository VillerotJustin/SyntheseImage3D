#include "./Circle.h"
#include "../math_common.h"

namespace geometry {

// Constructor
Circle::Circle(const Vector3D& center, double radius, const Vector3D& normal)
    : center(center), radius(radius), normal(normal.normalized()) {
    // Ensure the normal is normalized
}

double Circle::getCircumference() const {
    return 2 * math::pi * radius;
}

double Circle::getArea() const {
    return math::pi * radius * radius;
}

bool Circle::containsPoint(const Vector3D& point, double tolerance) const {
    // Check if the point lies in the plane of the circle
    Vector3D toPoint = point - center;
    double distToPlane = std::abs(toPoint.dot(normal));
    if (distToPlane > tolerance) {
        return false; // Point is not in the plane of the circle
    }

    // Check if the point is within the radius of the circle
    double distToCenter = (toPoint - toPoint.dot(normal) * normal).length();
    return std::abs(distToCenter - radius) <= tolerance;
}

Vector3D Circle::getPointAtAngle(double angle) const {
    Vector3D u, v;
    generateBasisVectors(u, v);
    return center + radius * (std::cos(angle) * u + std::sin(angle) * v);
}

void Circle::setNormal(const Vector3D& newNormal) {
    normal = newNormal.normalized();
}

void Circle::setCenter(const Vector3D& newCenter) {
    center = newCenter;
}

void Circle::setRadius(double newRadius) {
    if (newRadius > 0) {
        radius = newRadius;
    }
}

bool Circle::isValid() const {
    return radius > 0 && normal.length() > 0;
}

void Circle::generateBasisVectors(Vector3D& u, Vector3D& v) const {
    // Generate an arbitrary vector that is not parallel to the normal
    Vector3D arbitrary = (std::abs(normal.x) < 0.9) ? Vector3D(1, 0, 0) : Vector3D(0, 1, 0);
    u = normal.cross(arbitrary).normalized();
    v = normal.cross(u).normalized();
}

} // namespace geometry

