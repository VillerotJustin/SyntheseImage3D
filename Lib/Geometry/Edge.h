//
// Created by villerot on 03/10/25.
//

#ifndef EDGE_H
#define EDGE_H

#include "./Vector3D.h"
#include <optional>

namespace geometry {

    /**
     * @class Edge
     * @brief Represents a line segment between two points in 3D space
     * 
     * An Edge is defined by two endpoints and provides functionality for
     * geometric operations like length calculation, midpoint determination,
     * and point containment testing.
     */
    class Edge {
    public:
        #pragma region Constructors

        /**
         * @brief Constructor for Edge
         * @param start Starting point of the edge
         * @param end Ending point of the edge
         */
        Edge(const Vector3D& start, const Vector3D& end);

        /**
         * @brief Default constructor creates an edge from origin to origin
         */
        Edge();

        #pragma endregion

        #pragma region Accessors

        /**
         * @brief Get the starting point of the edge
         * @return Vector3D Starting point of the edge
         */
        const Vector3D& getStart() const { return start; }

        /**
         * @brief Get the ending point of the edge
         * @return Vector3D Ending point of the edge
         */
        const Vector3D& getEnd() const { return end; }

        /**
         * @brief Get the direction vector from start to end
         * @return Vector3D Direction vector (not normalized)
         */
        Vector3D getDirection() const { return end - start; }

        /**
         * @brief Get the normalized direction vector from start to end
         * @return Vector3D Normalized direction vector
         */
        Vector3D getDirectionNormalized() const;

        /**
         * @brief Get the length of the edge
         * @return double Length of the edge
         */
        double getLength() const;

        /**
         * @brief Get the squared length of the edge (more efficient than getLength)
         * @return double Squared length of the edge
         */
        double getLengthSquared() const;

        /**
         * @brief Get the midpoint of the edge
         * @return Vector3D Midpoint between start and end
         */
        Vector3D getMidpoint() const;

        #pragma endregion

        #pragma region Setters

        /**
         * @brief Set the starting point of the edge
         * @param newStart New starting point
         */
        void setStart(const Vector3D& newStart) { start = newStart; }

        /**
         * @brief Set the ending point of the edge
         * @param newEnd New ending point
         */
        void setEnd(const Vector3D& newEnd) { end = newEnd; }

        /**
         * @brief Set both endpoints of the edge
         * @param newStart New starting point
         * @param newEnd New ending point
         */
        void setEndpoints(const Vector3D& newStart, const Vector3D& newEnd);

        #pragma endregion

        #pragma region Geometric Operations

        /**
         * @brief Get a point at parameter t along the edge
         * @param t Parameter value (0 = start, 1 = end)
         * @return Vector3D Point at parameter t
         */
        Vector3D getPointAtParameter(double t) const;

        /**
         * @brief Find the closest point on the edge to a given point
         * @param point The point to find the closest point to
         * @return Vector3D Closest point on the edge
         */
        Vector3D closestPointTo(const Vector3D& point) const;

        /**
         * @brief Calculate the distance from a point to the edge
         * @param point The point to calculate distance to
         * @return double Distance from point to edge
         */
        double distanceTo(const Vector3D& point) const;

        /**
         * @brief Calculate the squared distance from a point to the edge
         * @param point The point to calculate squared distance to
         * @return double Squared distance from point to edge
         */
        double distanceToSquared(const Vector3D& point) const;

        /**
         * @brief Check if a point lies on the edge (within tolerance)
         * @param point The point to check
         * @param tolerance Tolerance for the check (default: 1e-9)
         * @return bool True if point lies on the edge
         */
        bool containsPoint(const Vector3D& point, double tolerance = 1e-9) const;

        /**
         * @brief Check if this edge intersects with another edge
         * @param other The other edge to check intersection with
         * @return bool True if edges intersect
         */
        bool intersects(const Edge& other) const;

        /**
         * @brief Find intersection point with another edge (if it exists)
         * @param other The other edge to find intersection with
         * @return std::optional<Vector3D> Intersection point if it exists
         */
        std::optional<Vector3D> intersectionPoint(const Edge& other) const;

        #pragma endregion

        #pragma region Validation

        /**
         * @brief Check if the edge is valid (start and end are not the same)
         * @return bool True if the edge is valid
         */
        bool isValid() const;

        /**
         * @brief Check if the edge is degenerate (start == end)
         * @return bool True if the edge is degenerate
         */
        bool isDegenerate() const { return start == end; }

        #pragma endregion

        #pragma region Operators

        /**
         * @brief Equality operator
         * @param other Edge to compare with
         * @return bool True if edges are equal
         */
        bool operator==(const Edge& other) const;

        /**
         * @brief Inequality operator
         * @param other Edge to compare with
         * @return bool True if edges are not equal
         */
        bool operator!=(const Edge& other) const { return !(*this == other); }

        #pragma endregion

        #pragma region Transformations

        /**
         * @brief Translate the edge by a vector
         * @param translation Translation vector
         * @return Edge New translated edge
         */
        Edge translate(const Vector3D& translation) const;

        /**
         * @brief Scale the edge from its midpoint
         * @param scale Scale factor
         * @return Edge New scaled edge
         */
        Edge scale(double scale) const;

        /**
         * @brief Reverse the direction of the edge (swap start and end)
         * @return Edge New reversed edge
         */
        Edge reverse() const;

        #pragma endregion

        #pragma region Static Methods

        /**
         * @brief Create an edge along the X-axis
         * @param length Length of the edge
         * @param origin Starting point (default: origin)
         * @return Edge Edge along X-axis
         */
        static Edge createXAxis(double length, const Vector3D& origin = Vector3D::ZERO);

        /**
         * @brief Create an edge along the Y-axis
         * @param length Length of the edge
         * @param origin Starting point (default: origin)
         * @return Edge Edge along Y-axis
         */
        static Edge createYAxis(double length, const Vector3D& origin = Vector3D::ZERO);

        /**
         * @brief Create an edge along the Z-axis
         * @param length Length of the edge
         * @param origin Starting point (default: origin)
         * @return Edge Edge along Z-axis
         */
        static Edge createZAxis(double length, const Vector3D& origin = Vector3D::ZERO);

        #pragma endregion

    private:
        Vector3D start;  ///< Starting point of the edge
        Vector3D end;    ///< Ending point of the edge
    };

} // namespace geometry

#endif // EDGE_H
