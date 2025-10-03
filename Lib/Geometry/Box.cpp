//
// Created by villerot on 10/03/25.
//

#include "./Box.h"
#include "../math_common.h"

namespace geometry {

// Constructor
Box::Box(const Vector3D& origin, double w, double h, double p, const Vector3D& normal)
    : origin(origin), w(w), h(h), p(p), normal(normal.normalized()) {
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
    return (relativePoint.x >= 0 && relativePoint.x <= w &&
            relativePoint.y >= 0 && relativePoint.y <= h &&
            relativePoint.z >= 0 && relativePoint.z <= p);
}

bool Box::isPointOnSurface(const Vector3D& point, double tolerance) const {
    Vector3D relativePoint = point - origin;
    
    // Check if point is within the box bounds
    if (relativePoint.x < -tolerance || relativePoint.x > w + tolerance ||
        relativePoint.y < -tolerance || relativePoint.y > h + tolerance ||
        relativePoint.z < -tolerance || relativePoint.z > p + tolerance) {
        return false;
    }
    
    // Check if point is on any face
    bool onXFace = (std::abs(relativePoint.x) <= tolerance || std::abs(relativePoint.x - w) <= tolerance);
    bool onYFace = (std::abs(relativePoint.y) <= tolerance || std::abs(relativePoint.y - h) <= tolerance);
    bool onZFace = (std::abs(relativePoint.z) <= tolerance || std::abs(relativePoint.z - p) <= tolerance);
    
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
    
    return (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x &&
            thisMin.y <= otherMax.y && thisMax.y >= otherMin.y &&
            thisMin.z <= otherMax.z && thisMax.z >= otherMin.z);
}

Box Box::intersection(const Box& other) const {
    Vector3D thisMin = getMinCorner();
    Vector3D thisMax = getMaxCorner();
    Vector3D otherMin = other.getMinCorner();
    Vector3D otherMax = other.getMaxCorner();
    
    Vector3D intersectionMin(
        std::max(thisMin.x, otherMin.x),
        std::max(thisMin.y, otherMin.y),
        std::max(thisMin.z, otherMin.z)
    );
    
    Vector3D intersectionMax(
        std::min(thisMax.x, otherMax.x),
        std::min(thisMax.y, otherMax.y),
        std::min(thisMax.z, otherMax.z)
    );
    
    Vector3D dimensions = intersectionMax - intersectionMin;
    
    // If any dimension is negative, there's no intersection
    if (dimensions.x <= 0 || dimensions.y <= 0 || dimensions.z <= 0) {
        return Box(Vector3D(0, 0, 0), 0, 0, 0);  // Invalid box
    }
    
    return Box(intersectionMin, dimensions.x, dimensions.y, dimensions.z);
}

Box Box::expand(double amount) const {
    return Box(
        origin - Vector3D(amount, amount, amount),
        w + 2 * amount,
        h + 2 * amount,
        p + 2 * amount
    );
}

Box Box::translate(const Vector3D& offset) const {
    return Box(origin + offset, w, h, p);
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
    normal = newNormal.normalized();
}

bool Box::isValid() const {
    return w > 0 && h > 0 && p > 0 && normal.length() > 0;
}

} // namespace geometry