//
// Created by villerot on 10/03/25.
//

#include "./Box.h"
#include "../Math/math_common.h"

#include <stdexcept>
#include <limits>

namespace geometry {

    // Constructor
    Box::Box(const Vector3D& origin, double w, double h, double p, const Vector3D& normal)
        : origin(origin), w(w), h(h), p(p), normal(normal.normal()) {
        // All dimensions should be positive for a valid box
        // Normal vector is automatically normalized
    }

    double Box::getVolume() const {
        return w * h * p;
    }

    double Box::getSurfaceArea() const {
        return 2.0 * (w * h + w * p + h * p);
    }

    bool Box::containsPoint(const Vector3D& point) const {
        Vector3D relativePoint = point - origin;
        return (relativePoint.x() >= 0 && relativePoint.x() <= w &&
                relativePoint.y() >= 0 && relativePoint.y() <= h &&
                relativePoint.z() >= 0 && relativePoint.z() <= p);
    }

    bool Box::isPointOnSurface(const Vector3D& point, double tolerance) const {
        Vector3D relativePoint = point - origin;
        
        // Check if point is within the box bounds
        if (relativePoint.x() < -tolerance || relativePoint.x() > w + tolerance ||
            relativePoint.y() < -tolerance || relativePoint.y() > h + tolerance ||
            relativePoint.z() < -tolerance || relativePoint.z() > p + tolerance) {
            return false;
        }
        
        // Check if point is on any face
        bool onXFace = (std::abs(relativePoint.x()) <= tolerance || std::abs(relativePoint.x() - w) <= tolerance);
        bool onYFace = (std::abs(relativePoint.y()) <= tolerance || std::abs(relativePoint.y() - h) <= tolerance);
        bool onZFace = (std::abs(relativePoint.z()) <= tolerance || std::abs(relativePoint.z() - p) <= tolerance);

        return onXFace || onYFace || onZFace;
    }

    Vector3D Box::getCenter() const {
        return origin + Vector3D(w / 2.0, h / 2.0, p / 2.0);
    }

    Vector3D Box::getMinCorner() const {
        return origin;
    }

    Vector3D Box::getMaxCorner() const {
        return origin + Vector3D(w, h, p);
    }

    void Box::getCorners(Vector3D corners[8]) const {
        corners[0] = origin;                                    // (0,0,0)
        corners[1] = origin + Vector3D(w, 0, 0);               // (w,0,0)
        corners[2] = origin + Vector3D(w, h, 0);               // (w,h,0)
        corners[3] = origin + Vector3D(0, h, 0);               // (0,h,0)
        corners[4] = origin + Vector3D(0, 0, p);               // (0,0,p)
        corners[5] = origin + Vector3D(w, 0, p);               // (w,0,p)
        corners[6] = origin + Vector3D(w, h, p);               // (w,h,p)
        corners[7] = origin + Vector3D(0, h, p);               // (0,h,p)
    }

    bool Box::intersects(const Box& other) const {
        Vector3D thisMin = getMinCorner();
        Vector3D thisMax = getMaxCorner();
        Vector3D otherMin = other.getMinCorner();
        Vector3D otherMax = other.getMaxCorner();
        
        return (thisMin.x() <= otherMax.x() && thisMax.x() >= otherMin.x() &&
                thisMin.y() <= otherMax.y() && thisMax.y() >= otherMin.y() &&
                thisMin.z() <= otherMax.z() && thisMax.z() >= otherMin.z());
    }

    std::optional<std::variant<Vector3D, Edge, Rectangle, Box>> Box::intersectionPoints(const Box& other) const {
        (void)other; // Suppress unused parameter warning
        throw std::runtime_error("Box::intersection not implemented yet");
        // TODO : Implement intersection logic with proper handling of non-intersecting boxes & different return types
        // If no intersection, return nullpointer
        // If intersection is a point, return Vector3D
        // If intersection is a line, return Line
        // If intersection is a rectangle, return Rectangle
        // If intersection is a box, return Box
    }

    Box Box::expand(double amount) const {
        return Box(
            origin - Vector3D(amount, amount, amount),
            w + 2 * amount,
            h + 2 * amount,
            p + 2 * amount,
            normal
        );
    }

    Box Box::translate(const Vector3D& offset) const {
        return Box(origin + offset, w, h, p, normal);
    }

    void Box::setOrigin(const Vector3D& newOrigin) {
        origin = newOrigin;
    }

    void Box::setDimensions(double newW, double newH, double newP) {
        w = newW;
        h = newH;
        p = newP;
    }

    void Box::setNormal(const Vector3D& newNormal) {
        normal = newNormal.normal();
    }

    bool Box::isValid() const {
        return w > 0 && h > 0 && p > 0 && normal.length() > 0;
    }

    bool Box::rayIntersect(const Ray& ray) const {
        // Using the "slab" method for ray-box intersection
        Vector3D rayDir = ray.getDirection();
        Vector3D rayOrigin = ray.getOrigin();
        Vector3D minCorner = getMinCorner();
        Vector3D maxCorner = getMaxCorner();
        
        double tmin = -std::numeric_limits<double>::infinity();
        double tmax = std::numeric_limits<double>::infinity();
        
        // Check intersection with each pair of parallel planes (slabs)
        for (int i = 0; i < 3; ++i) {
            double rayDirComponent = (i == 0) ? rayDir.x() : (i == 1) ? rayDir.y() : rayDir.z();
            double rayOriginComponent = (i == 0) ? rayOrigin.x() : (i == 1) ? rayOrigin.y() : rayOrigin.z();
            double minComponent = (i == 0) ? minCorner.x() : (i == 1) ? minCorner.y() : minCorner.z();
            double maxComponent = (i == 0) ? maxCorner.x() : (i == 1) ? maxCorner.y() : maxCorner.z();
            
            if (std::abs(rayDirComponent) < 1e-9) {
                // Ray is parallel to this pair of planes
                if (rayOriginComponent < minComponent || rayOriginComponent > maxComponent) {
                    return false; // Ray is outside the slab
                }
            } else {
                // Calculate intersection parameters
                double t1 = (minComponent - rayOriginComponent) / rayDirComponent;
                double t2 = (maxComponent - rayOriginComponent) / rayDirComponent;
                
                // Ensure t1 <= t2
                if (t1 > t2) {
                    std::swap(t1, t2);
                }
                
                // Update tmin and tmax
                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);
                
                // Check if intersection interval is empty
                if (tmin > tmax) {
                    return false;
                }
            }
        }
        
        // Ray intersects box if tmax >= 0 (intersection is in front of ray origin)
        return tmax >= 0;
    }

    std::optional<double> Box::rayIntersectDepth(const Ray& ray) const {
        // Using the "slab" method for ray-box intersection
        Vector3D rayDir = ray.getDirection();
        Vector3D rayOrigin = ray.getOrigin();
        Vector3D minCorner = getMinCorner();
        Vector3D maxCorner = getMaxCorner();
        
        double tmin = -std::numeric_limits<double>::infinity();
        double tmax = std::numeric_limits<double>::infinity();
        
        // Check intersection with each pair of parallel planes (slabs)
        for (int i = 0; i < 3; ++i) {
            double rayDirComponent = (i == 0) ? rayDir.x() : (i == 1) ? rayDir.y() : rayDir.z();
            double rayOriginComponent = (i == 0) ? rayOrigin.x() : (i == 1) ? rayOrigin.y() : rayOrigin.z();
            double minComponent = (i == 0) ? minCorner.x() : (i == 1) ? minCorner.y() : minCorner.z();
            double maxComponent = (i == 0) ? maxCorner.x() : (i == 1) ? maxCorner.y() : maxCorner.z();
            
            if (std::abs(rayDirComponent) < 1e-9) {
                // Ray is parallel to this pair of planes
                if (rayOriginComponent < minComponent || rayOriginComponent > maxComponent) {
                    return std::nullopt; // Ray is outside the slab
                }
            } else {
                // Calculate intersection parameters
                double t1 = (minComponent - rayOriginComponent) / rayDirComponent;
                double t2 = (maxComponent - rayOriginComponent) / rayDirComponent;
                
                // Ensure t1 <= t2
                if (t1 > t2) {
                    std::swap(t1, t2);
                }
                
                // Update tmin and tmax
                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);
                
                // Check if intersection interval is empty
                if (tmin > tmax) {
                    return std::nullopt;
                }
            }
        }
        
        // Ray intersects box if tmax >= 0 (intersection is in front of ray origin)
        if (tmax >= 0) {
            return tmin >= 0 ? tmin : tmax; // Return nearest positive intersection
        } else {
            return std::nullopt; // Intersection is behind the ray origin
        }
    }

} // namespace geometry