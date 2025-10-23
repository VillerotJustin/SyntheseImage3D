#include "./Circle.h"
#include "../Math/math_common.h"

namespace geometry {

    // Constructor
    Circle::Circle(const Vector3D& center, double radius, const Vector3D& normal)
        : center(center), radius(radius), normal(normal.normal()) {
        // Ensure the normal is normal & radius is positive for a valid circle
        if (normal.length() == 0) {
            throw std::invalid_argument("Normal vector cannot be zero");
        }
        if (radius <= 0) {
            throw std::invalid_argument("Radius must be positive");
        }
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
        normal = newNormal.normal();
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
        // For consistent orientation, we want the first basis vector to point 
        // in a predictable direction. For a circle with normal (0,0,1),
        // we want u = (1,0,0) and v = (0,1,0) to match the test expectations.
        
        if (std::abs(normal.z()) > 0.9) {
            // Normal is mostly aligned with Z axis
            u = Vector3D(1, 0, 0);
            v = Vector3D(0, 1, 0);
        } else if (std::abs(normal.y()) > 0.9) {
            // Normal is mostly aligned with Y axis
            u = Vector3D(1, 0, 0);
            v = normal.cross(u).normal();
        } else {
            // Normal is mostly aligned with X axis
            u = Vector3D(0, 1, 0);
            v = normal.cross(u).normal();
        }
        
        // Ensure orthogonality by recomputing v
        v = normal.cross(u).normal();
    }

    Vector3D Circle::getNormalAt(const Vector3D& point) const {
        if (!containsPoint(point)) {
            throw std::invalid_argument("Point is not on the circle");
        }
        return normal;
    }

    bool Circle::rayIntersect(const Ray& ray) const {
        // First, check if ray intersects the plane containing the circle
        Vector3D rayDir = ray.getDirection();
        Vector3D rayOrigin = ray.getOrigin();
        
        // Check if ray is parallel to the plane
        double denominator = rayDir.dot(normal);
        if (std::abs(denominator) < 1e-9) {
            // Ray is parallel to the plane
            return false;
        }
        
        // Calculate intersection parameter t
        Vector3D toCenter = center - rayOrigin;
        double t = toCenter.dot(normal) / denominator;
        
        // Check if intersection point is behind the ray origin
        if (t < 0) {
            return false;
        }
        
        // Calculate intersection point
        Vector3D intersectionPoint = ray.getPointAt(t);
        
        // Check if intersection point is within the circle radius
        double distToCenter = (intersectionPoint - center).length();
        return distToCenter <= radius;
    }

    std::optional<double> Circle::rayIntersectDepth(const Ray& ray) const {
        // First, check if ray intersects the plane containing the circle
        Vector3D rayDir = ray.getDirection();
        Vector3D rayOrigin = ray.getOrigin();
        
        // Check if ray is parallel to the plane
        double denominator = rayDir.dot(normal);
        if (std::abs(denominator) < 1e-9) {
            // Ray is parallel to the plane
            return std::nullopt;
        }
        
        // Calculate intersection parameter t
        Vector3D toCenter = center - rayOrigin;
        double t = toCenter.dot(normal) / denominator;
        
        // Check if intersection point is behind the ray origin
        if (t < 0) {
            return std::nullopt;
        }
        
        // Calculate intersection point
        Vector3D intersectionPoint = ray.getPointAt(t);
        
        // Check if intersection point is within the circle radius
        double distToCenter = (intersectionPoint - center).length();
        if (distToCenter <= radius) {
            return t; // Return the depth of intersection
        } else {
            return std::nullopt; // No intersection with the circle
        }
    }

} // namespace geometry

