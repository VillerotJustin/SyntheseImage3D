//
// Created by villerot on 10/03/25.
//

#include "./Ray.h"
#include "../math_common.h"

namespace geometry {

// Constructor
Ray::Ray(const Vector3D& origin, const Vector3D& direction)
    : origin(origin), direction(direction.normalized()) {
    // Ensure direction is normalized
}

Vector3D Ray::getPointAt(double t) const {
    return origin + t * direction;
}

Vector3D Ray::closestPointTo(const Vector3D& point) const {
    double t = closestParameterTo(point);
    return getPointAt(t);
}

double Ray::closestParameterTo(const Vector3D& point) const {
    Vector3D toPoint = point - origin;
    double t = toPoint.dot(direction);
    // Clamp t to be non-negative since it's a ray
    return std::max(0.0, t);
}

double Ray::distanceToPoint(const Vector3D& point) const {
    Vector3D closestPoint = closestPointTo(point);
    return (point - closestPoint).length();
}

bool Ray::containsPoint(const Vector3D& point, double tolerance) const {
    return distanceToPoint(point) <= tolerance;
}

Ray Ray::translate(const Vector3D& offset) const {
    return Ray(origin + offset, direction);
}

void Ray::setOrigin(const Vector3D& newOrigin) {
    origin = newOrigin;
}

void Ray::setDirection(const Vector3D& newDirection) {
    direction = newDirection.normalized();
}

bool Ray::isValid() const {
    return direction.length() > 0;
}

bool Ray::isParallelTo(const Ray& other, double tolerance) const {
    // Two rays are parallel if their direction vectors are parallel
    Vector3D cross = direction.cross(other.direction);
    return cross.length() <= tolerance;
}

double Ray::closestPointsBetweenRays(const Ray& other, double& t1, double& t2) const {
    Vector3D w0 = origin - other.origin;
    double a = direction.dot(direction);
    double b = direction.dot(other.direction);
    double c = other.direction.dot(other.direction);
    double d = direction.dot(w0);
    double e = other.direction.dot(w0);
    
    double denominator = a * c - b * b;
    
    if (std::abs(denominator) < 1e-9) {
        // Rays are parallel
        t1 = 0.0;
        t2 = d / b;  // If b is also near zero, rays are coincident
        t2 = std::max(0.0, t2);  // Clamp for ray
    } else {
        t1 = (b * e - c * d) / denominator;
        t2 = (a * e - b * d) / denominator;
        
        // Clamp parameters to be non-negative for rays
        t1 = std::max(0.0, t1);
        t2 = std::max(0.0, t2);
    }
    
    Vector3D point1 = getPointAt(t1);
    Vector3D point2 = other.getPointAt(t2);
    
    return (point1 - point2).length();
}

} // namespace geometry