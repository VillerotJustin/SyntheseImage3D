//
// Created by villerot on 03/10/25.
//

#include "Edge.h"
#include "../Math/math_common.h"
#include <algorithm>
#include <optional>

namespace geometry {

#pragma region Constructors

Edge::Edge(const Vector3D& start, const Vector3D& end)
    : start(start), end(end) {
}

Edge::Edge() : start(Vector3D::ZERO), end(Vector3D::ZERO) {
}

#pragma endregion

#pragma region Accessors

Vector3D Edge::getDirectionNormalized() const {
    Vector3D direction = getDirection();
    if (direction.length() == 0) {
        return Vector3D::ZERO;
    }
    return direction.normal();
}

double Edge::getLength() const {
    return getDirection().length();
}

double Edge::getLengthSquared() const {
    return getDirection().lengthSquared();
}

Vector3D Edge::getMidpoint() const {
    return (start + end) * 0.5;
}

#pragma endregion

#pragma region Setters

void Edge::setEndpoints(const Vector3D& newStart, const Vector3D& newEnd) {
    start = newStart;
    end = newEnd;
}

#pragma endregion

#pragma region Geometric Operations

Vector3D Edge::getPointAtParameter(double t) const {
    return start + getDirection() * t;
}

Vector3D Edge::closestPointTo(const Vector3D& point) const {
    Vector3D direction = getDirection();
    double lengthSquared = direction.lengthSquared();
    
    if (lengthSquared == 0) {
        // Degenerate edge, return start point
        return start;
    }
    
    // Project point onto the infinite line containing the edge
    double t = (point - start).dot(direction) / lengthSquared;
    
    // Clamp t to [0, 1] to stay within the edge segment
    t = std::max(0.0, std::min(1.0, t));
    
    return getPointAtParameter(t);
}

double Edge::distanceTo(const Vector3D& point) const {
    return (point - closestPointTo(point)).length();
}

double Edge::distanceToSquared(const Vector3D& point) const {
    return (point - closestPointTo(point)).lengthSquared();
}

bool Edge::containsPoint(const Vector3D& point, double tolerance) const {
    return distanceTo(point) <= tolerance;
}

bool Edge::intersects(const Edge& other) const {
    return intersectionPoint(other).has_value();
}

std::optional<Vector3D> Edge::intersectionPoint(const Edge& other) const {
    // Check for degenerate edges
    if (isDegenerate() || other.isDegenerate()) {
        return std::nullopt;
    }
    
    Vector3D dir1 = getDirection();
    Vector3D dir2 = other.getDirection();
    Vector3D startDiff = other.start - start;
    
    // Check if lines are parallel
    Vector3D cross = dir1.cross(dir2);
    double crossLengthSquared = cross.lengthSquared();
    
    if (crossLengthSquared < 1e-10) {
        // Lines are parallel, check if they are collinear
        Vector3D crossStart = startDiff.cross(dir1);
        if (crossStart.lengthSquared() < 1e-10) {
            // Lines are collinear, check for overlap
            // Project other.start and other.end onto this edge
            double dir1LengthSquared = dir1.lengthSquared();
            double t1 = startDiff.dot(dir1) / dir1LengthSquared;
            double t2 = (other.end - start).dot(dir1) / dir1LengthSquared;
            
            // Ensure t1 <= t2
            if (t1 > t2) std::swap(t1, t2);
            
            // Check for overlap with [0, 1]
            if (t2 >= 0 && t1 <= 1) {
                // There is overlap, return a point in the overlap region
                double overlapStart = std::max(0.0, t1);
                double overlapEnd = std::min(1.0, t2);
                return getPointAtParameter((overlapStart + overlapEnd) * 0.5);
            }
        }
        return std::nullopt;
    }
    
    // Lines are not parallel, find intersection point
    // Solve the system: start + t1 * dir1 = other.start + t2 * dir2
    // This gives us: t1 * dir1 - t2 * dir2 = startDiff
    
    // Use the cross product method for 3D line intersection
    Vector3D cross1 = startDiff.cross(dir2);
    Vector3D cross2 = startDiff.cross(dir1);
    
    double t1 = cross1.dot(cross) / crossLengthSquared;
    double t2 = cross2.dot(cross) / crossLengthSquared;
    
    // Check if intersection point is within both edge segments
    if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
        // Verify that the intersection points are actually the same
        Vector3D point1 = getPointAtParameter(t1);
        Vector3D point2 = other.getPointAtParameter(t2);
        
        if ((point1 - point2).lengthSquared() < 1e-10) {
            return point1;
        }
    }
    
    return std::nullopt;
}

#pragma endregion

#pragma region Validation

bool Edge::isValid() const {
    return !isDegenerate();
}

#pragma endregion

#pragma region Operators

bool Edge::operator==(const Edge& other) const {
    return start == other.start && end == other.end;
}

#pragma endregion

#pragma region Transformations

Edge Edge::translate(const Vector3D& translation) const {
    return Edge(start + translation, end + translation);
}

Edge Edge::scale(double scale) const {
    Vector3D midpoint = getMidpoint();
    Vector3D direction = getDirection() * scale;
    return Edge(midpoint - direction * 0.5, midpoint + direction * 0.5);
}

Edge Edge::reverse() const {
    return Edge(end, start);
}

#pragma endregion

#pragma region Static Methods

Edge Edge::createXAxis(double length, const Vector3D& origin) {
    return Edge(origin, origin + Vector3D(length, 0, 0));
}

Edge Edge::createYAxis(double length, const Vector3D& origin) {
    return Edge(origin, origin + Vector3D(0, length, 0));
}

Edge Edge::createZAxis(double length, const Vector3D& origin) {
    return Edge(origin, origin + Vector3D(0, 0, length));
}

#pragma endregion

} // namespace geometry