//
// Created by villerot on 10/03/25.
//

#include "./Rectangle.h"
#include "../Math/math_common.h"

namespace geometry {

    // Constructor
    Rectangle::Rectangle(const Vector3D& origin, double l, double w, const Vector3D& normal)
        : origin(origin), l(l), w(w), normal(normal.normal()) {
        // Ensure the normal is normal and dimensions are positive for a valid rectangle
        if (normal.length() == 0) {
            throw std::invalid_argument("Normal vector cannot be zero");
        }
        if (l <= 0 || w <= 0) {
            throw std::invalid_argument("Length and width must be positive");
        }
    }

    double Rectangle::getArea() const {
        return l * w;
    }

    double Rectangle::getPerimeter() const {
        return 2.0 * (l + w);
    }

    Vector3D Rectangle::getCenter() const {
        Vector3D lengthDir, widthDir;
        generateBasisVectors(lengthDir, widthDir);
        return origin + (l / 2.0) * lengthDir + (w / 2.0) * widthDir;
    }

    void Rectangle::getCorners(Vector3D corners[4]) const {
        Vector3D lengthDir, widthDir;
        generateBasisVectors(lengthDir, widthDir);
        
        corners[0] = origin;                                    // Origin corner
        corners[1] = origin + l * lengthDir;                   // Length corner
        corners[2] = origin + l * lengthDir + w * widthDir;    // Opposite corner
        corners[3] = origin + w * widthDir;                    // Width corner
    }

    bool Rectangle::containsPoint(const Vector3D& point, double tolerance) const {
        // First check if point is in the plane of the rectangle
        Vector3D toPoint = point - origin;
        double distToPlane = std::abs(toPoint.dot(normal));
        if (distToPlane > tolerance) {
            return false; // Point is not in the plane of the rectangle
        }
        
        // Project point onto rectangle plane and check if within bounds
        Vector3D projectedPoint = projectPointToPlane(point);
        Vector3D fromOrigin = projectedPoint - origin;
        
        Vector3D lengthDir, widthDir;
        generateBasisVectors(lengthDir, widthDir);
        
        // Get coordinates in rectangle's local coordinate system
        double lengthCoord = fromOrigin.dot(lengthDir);
        double widthCoord = fromOrigin.dot(widthDir);
        
        // Check if within rectangle bounds
        return (lengthCoord >= -tolerance && lengthCoord <= l + tolerance &&
                widthCoord >= -tolerance && widthCoord <= w + tolerance);
    }

    bool Rectangle::isPointOnEdge(const Vector3D& point, double tolerance) const {
        if (!containsPoint(point, tolerance)) {
            return false; // Point must be within rectangle bounds
        }
        
        Vector3D projectedPoint = projectPointToPlane(point);
        Vector3D fromOrigin = projectedPoint - origin;
        
        Vector3D lengthDir, widthDir;
        generateBasisVectors(lengthDir, widthDir);
        
        double lengthCoord = fromOrigin.dot(lengthDir);
        double widthCoord = fromOrigin.dot(widthDir);
        
        // Check if on any edge
        bool onLengthEdge = (std::abs(lengthCoord) <= tolerance || std::abs(lengthCoord - l) <= tolerance);
        bool onWidthEdge = (std::abs(widthCoord) <= tolerance || std::abs(widthCoord - w) <= tolerance);
        
        return onLengthEdge || onWidthEdge;
    }

    Vector3D Rectangle::projectPointToPlane(const Vector3D& point) const {
        Vector3D toPoint = point - origin;
        double distance = toPoint.dot(normal);
        return point - distance * normal;
    }

    Vector3D Rectangle::closestPointOnRectangle(const Vector3D& point) const {
        // First project point onto rectangle plane
        Vector3D projectedPoint = projectPointToPlane(point);
        Vector3D fromOrigin = projectedPoint - origin;
        
        Vector3D lengthDir, widthDir;
        generateBasisVectors(lengthDir, widthDir);
        
        // Get coordinates in rectangle's local coordinate system
        double lengthCoord = fromOrigin.dot(lengthDir);
        double widthCoord = fromOrigin.dot(widthDir);
        
        // Clamp coordinates to rectangle bounds
        lengthCoord = std::max(0.0, std::min(l, lengthCoord));
        widthCoord = std::max(0.0, std::min(w, widthCoord));
        
        // Convert back to world coordinates
        return origin + lengthCoord * lengthDir + widthCoord * widthDir;
    }

    double Rectangle::distanceToPoint(const Vector3D& point) const {
        Vector3D closestPoint = closestPointOnRectangle(point);
        return (point - closestPoint).length();
    }

    Vector3D Rectangle::getPointAt(double u, double v) const {
        // Clamp parameters to [0,1] range
        u = std::max(0.0, std::min(1.0, u));
        v = std::max(0.0, std::min(1.0, v));
        
        Vector3D lengthDir, widthDir;
        generateBasisVectors(lengthDir, widthDir);
        
        return origin + (u * l) * lengthDir + (v * w) * widthDir;
    }

    Rectangle Rectangle::translate(const Vector3D& offset) const {
        return Rectangle(origin + offset, l, w, normal);
    }

    Rectangle Rectangle::scale(double lengthScale, double widthScale) const {
        if (lengthScale <= 0 || widthScale <= 0) {
            return *this; // Return unchanged if invalid scale factors
        }
        return Rectangle(origin, l * lengthScale, w * widthScale, normal);
    }

    void Rectangle::setOrigin(const Vector3D& newOrigin) {
        origin = newOrigin;
    }

    void Rectangle::setDimensions(double newL, double newW) {
        if (newL > 0 && newW > 0) {
            l = newL;
            w = newW;
        } else {
            throw std::invalid_argument("Length and width must be positive");
        }
    }

    void Rectangle::setNormal(const Vector3D& newNormal) {
        normal = newNormal.normal();
    }

    bool Rectangle::isValid() const {
        return l > 0 && w > 0 && normal.length() > 0;
    }

    void Rectangle::generateBasisVectors(Vector3D& lengthDir, Vector3D& widthDir) const {
        // Generate two orthogonal vectors in the plane perpendicular to the normal
        // This method is mathematically robust and handles all edge cases
        
        Vector3D n = normal; // Ensure we're working with the normalized normal
        
        // Choose an initial reference vector that's not parallel to the normal
        Vector3D reference;
        if (std::abs(n.x()) < 0.9) {
            reference = Vector3D(1, 0, 0);  // Use X-axis if normal is not mostly along X
        } else if (std::abs(n.y()) < 0.9) {
            reference = Vector3D(0, 1, 0);  // Use Y-axis if normal is not mostly along Y  
        } else {
            reference = Vector3D(0, 0, 1);  // Use Z-axis if normal is mostly along X or Y
        }
        
        // Generate the first basis vector using Gram-Schmidt process
        lengthDir = reference - n * reference.dot(n);
        
        // Check if the result is too small (parallel case)
        if (lengthDir.length() < 1e-9) {
            // Try a different reference vector
            if (std::abs(n.z()) < 0.9) {
                reference = Vector3D(0, 0, 1);
            } else {
                reference = Vector3D(1, 0, 0);
            }
            lengthDir = reference - n * reference.dot(n);
        }
        
        // Normalize the first basis vector
        lengthDir = lengthDir.normal();
        
        // Generate the second basis vector using cross product
        // This guarantees orthogonality to both normal and lengthDir
        widthDir = n.cross(lengthDir);
        
        // The cross product should already be normalized since both inputs are normalized
        // and orthogonal, but normalize for safety
        widthDir = widthDir.normal();
    }

    bool Rectangle::rayIntersect(const Ray& ray) const {
        // First, check if ray intersects the plane containing the rectangle
        Vector3D rayDir = ray.getDirection();
        Vector3D rayOrigin = ray.getOrigin();
        
        // Check if ray is parallel to the plane
        double denominator = rayDir.dot(normal);
        if (std::abs(denominator) < 1e-9) {
            // Ray is parallel to the plane
            return false;
        }
        
        // Calculate intersection parameter t
        Vector3D toOrigin = origin - rayOrigin;
        double t = toOrigin.dot(normal) / denominator;
        
        // Check if intersection point is behind the ray origin
        if (t < 0) {
            return false;
        }
        
        // Calculate intersection point
        Vector3D intersectionPoint = ray.getPointAt(t);
        
        // Check if intersection point is within the rectangle bounds
        return containsPoint(intersectionPoint);
    }

    std::optional<double> Rectangle::rayIntersectDepth(const Ray& ray) const {
        // First, check if ray intersects the plane containing the rectangle
        Vector3D rayDir = ray.getDirection();
        Vector3D rayOrigin = ray.getOrigin();
        
        // Check if ray is parallel to the plane
        double denominator = rayDir.dot(normal);
        if (std::abs(denominator) < 1e-9) {
            // Ray is parallel to the plane
            return std::nullopt;
        }
        
        // Calculate intersection parameter t
        Vector3D toOrigin = origin - rayOrigin;
        double t = toOrigin.dot(normal) / denominator;
        
        // Check if intersection point is behind the ray origin
        if (t < 0) {
            return std::nullopt;
        }
        
        // Calculate intersection point
        Vector3D intersectionPoint = ray.getPointAt(t);
        
        // Check if intersection point is within the rectangle bounds
        if (containsPoint(intersectionPoint)) {
            return t; // Return intersection depth
        } else {
            return std::nullopt; // No intersection with rectangle bounds
        }
    }

} // namespace geometry