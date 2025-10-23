//
// Created by villerot on 10/03/25.
//

#include "./Rectangle.h"
#include "./Quaternion.h"
#include "../Math/math_common.h"
#include <iostream>

namespace geometry {

    // Constructor
    // Rectangle::Rectangle(const Vector3D& origin, double l, double w, const Vector3D& normal)
    //     : origin(origin), l(l), w(w), normal(normal.normal()) {
    //     // Ensure the normal is normal and dimensions are positive for a valid rectangle
    //     if (normal.length() == 0) {
    //         throw std::invalid_argument("Normal vector cannot be zero");
    //     }
    //     if (l <= 0 || w <= 0) {
    //         throw std::invalid_argument("Length and width must be positive");
    //     }
    // }

    Rectangle::Rectangle(const Vector3D& TopLeft, const Vector3D& TopRight, const Vector3D& BottomLeft) {
        origin = TopLeft;
        lengthDir = (TopRight - TopLeft);
        widthDir = (BottomLeft - TopLeft);

        l = lengthDir.length();
        w = widthDir.length();

        if (l > 0) lengthDir = lengthDir.normal();
        if (w > 0) widthDir = widthDir.normal();

        normal = lengthDir.cross(widthDir).normal();
    }

    double Rectangle::getArea() const {
        return l * w;
    }

    double Rectangle::getPerimeter() const {
        return 2.0 * (l + w);
    }

    Vector3D Rectangle::getLengthVec() const {
        return lengthDir;
    }

    Vector3D Rectangle::getWidthVec() const {
        return widthDir;
    }

    Vector3D Rectangle::getCenter() const {
        return origin + (l / 2.0) * lengthDir + (w / 2.0) * widthDir;
    }

    void Rectangle::getCorners(Vector3D corners[4]) const {
        corners[0] = origin;                                    // Origin corner
        corners[1] = origin + l * lengthDir;                   // Length corner
        corners[2] = origin + l * lengthDir + w * widthDir;    // Opposite corner
        corners[3] = origin + w * widthDir;                    // Width corner
    }

    bool Rectangle::containsPoint(const Vector3D& point) const {
        // Use an internal tolerance for floating-point comparisons
        const double tolerance = 1e-6;

        // First check if point is in the plane of the rectangle
        Vector3D toPoint = point - origin;
        double distToPlane = std::abs(toPoint.dot(normal));
        if (distToPlane > tolerance) {
            // Debug: point is off the plane
            std::cerr << "[Rectangle::containsPoint] rejected: off-plane dist=" << distToPlane
                      << " tol=" << tolerance << " point=" << point << " origin=" << origin << " normal=" << normal << std::endl;
            return false; // Point is not in the plane of the rectangle
        }

        // Project point onto rectangle plane and check if within bounds
        Vector3D projectedPoint = projectPointToPlane(point);
        Vector3D fromOrigin = projectedPoint - origin;

        // Get coordinates in rectangle's local coordinate system
        double lengthCoord = fromOrigin.dot(lengthDir);
        double widthCoord = fromOrigin.dot(widthDir);

        // Check if within rectangle bounds using internal tolerance
        bool inside = (lengthCoord >= -tolerance && lengthCoord <= l + tolerance &&
                       widthCoord >= -tolerance && widthCoord <= w + tolerance);
        // if (!inside) {
        //     // Debug: coordinates outside bounds
        //     std::cerr << "[Rectangle::containsPoint] rejected: local coords (len, wid)=(" << lengthCoord << ", " << widthCoord << ")"
        //               << " ranges [0," << l << "] [0," << w << "] point=" << point << " origin=" << origin << std::endl;
        // }
        return inside;
    }

    bool Rectangle::isPointOnEdge(const Vector3D& point) const {
        if (!containsPoint(point)) {
            return false; // Point must be within rectangle bounds
        }
        
        Vector3D projectedPoint = projectPointToPlane(point);
        Vector3D fromOrigin = projectedPoint - origin;

        double lengthCoord = fromOrigin.dot(lengthDir);
        double widthCoord = fromOrigin.dot(widthDir);

        // Check if on any edge
        bool onLengthEdge = (std::abs(lengthCoord) <= 1e-9 || std::abs(lengthCoord - l) <= 1e-9);
        bool onWidthEdge = (std::abs(widthCoord) <= 1e-9 || std::abs(widthCoord - w) <= 1e-9);

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
        
        // Get coordinates in rectangle's local coordinate system
        // Use dot with the normalized direction vectors to obtain local coordinates in world units
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
        
        return origin + (u * l) * lengthDir + (v * w) * widthDir;
    }

    Rectangle Rectangle::translate(const Vector3D& offset) const {
        return Rectangle(origin + offset, origin + offset + l * lengthDir, origin + offset + w * widthDir);
    }

    Rectangle Rectangle::scale(double lengthScale, double widthScale) const {
        if (lengthScale <= 0 || widthScale <= 0) {
            return *this; // Return unchanged if invalid scale factors
        }
        return Rectangle(origin, origin + l * lengthDir * lengthScale, origin + w * widthDir * widthScale);
    }

    Rectangle Rectangle::rotate(Quaternion rotation) const {
        Vector3D newOrigin = rotation * origin;
        Vector3D newLengthDir = rotation * lengthDir;
        Vector3D newWidthDir = rotation * widthDir;
        return Rectangle(newOrigin, newOrigin + l * newLengthDir, newOrigin + w * newWidthDir);
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

    Vector3D Rectangle::getNormalAt(const Vector3D& point) const {
        if (!containsPoint(point)) {
            throw std::runtime_error("Point is not on the surface of the rectangle");
        }
        return normal;
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
        
        // Calculate raw intersection point
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
        if (this->containsPoint(intersectionPoint)) {
            return t; // Return intersection depth
        } else {
            return std::nullopt; // No intersection with rectangle bounds
        }
    }

} // namespace geometry