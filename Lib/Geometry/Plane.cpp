//
// Created by villerot on 10/03/25.
//

#include "./Plane.h"
#include "../Math/math_common.h"

namespace geometry {

// Constructor from point and normal
Plane::Plane(const Vector3D& origin, const Vector3D& normal)
    : normal(normal.normal()), origin(origin) {
    // Ensure the normal is normal
}

// Constructor from three points
Plane::Plane(const Vector3D& point1, const Vector3D& point2, const Vector3D& point3)
    : origin(point1) {
    // Calculate two vectors in the plane
    Vector3D v1 = point2 - point1;
    Vector3D v2 = point3 - point1;
    
    // Calculate normal using cross product
    normal = v1.cross(v2).normal();
}

double Plane::distanceToPoint(const Vector3D& point) const {
    Vector3D toPoint = point - origin;
    return toPoint.dot(normal);
}

bool Plane::containsPoint(const Vector3D& point, double tolerance) const {
    return std::abs(distanceToPoint(point)) <= tolerance;
}

Vector3D Plane::projectPoint(const Vector3D& point) const {
    double distance = distanceToPoint(point);
    return point - distance * normal;
}

Vector3D Plane::lineIntersection(const Vector3D& linePoint, const Vector3D& lineDirection) const {
    double denominator = lineDirection.dot(normal);
    
    // Check if line is parallel to plane (denominator near zero)
    if (std::abs(denominator) < 1e-9) {
        // Line is parallel to plane, return the line point (or throw exception)
        return linePoint;
    }
    
    double t = (origin - linePoint).dot(normal) / denominator;
    return linePoint + t * lineDirection;
}

bool Plane::isLineParallel(const Vector3D& lineDirection, double tolerance) const {
    return std::abs(lineDirection.dot(normal)) <= tolerance;
}

void Plane::setOrigin(const Vector3D& newOrigin) {
    origin = newOrigin;
}

void Plane::setNormal(const Vector3D& newNormal) {
    normal = newNormal.normal();
}

bool Plane::isValid() const {
    return normal.length() > 0;
}

void Plane::getPlaneEquation(double& a, double& b, double& c, double& d) const {
    // Plane equation: ax + by + cz + d = 0
    // Where (a, b, c) is the normal vector and d = -normal.dot(origin)
    a = normal.x();
    b = normal.y();
    c = normal.z();
    d = -normal.dot(origin);
}

} // namespace geometry