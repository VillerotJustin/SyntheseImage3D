//
// Created by villerot on 10/03/25.
//

#ifndef RAY_H
#define RAY_H

#include "./Vector3D.h"

namespace geometry {

    class Ray {
    public:
        /**
         * Constructor for Ray
         * @param origin Starting point of the ray
         * @param direction Direction vector of the ray (will be normalized)
         */
        Ray(const Vector3D& origin, const Vector3D& direction);

        /**
         * Get the origin point of the ray
         * @return Vector3D Origin point of the ray
         */
        const Vector3D& getOrigin() const { return origin; }

        /**
         * Get the direction vector of the ray
         * @return Vector3D Normalized direction vector of the ray
         */
        const Vector3D& getDirection() const { return direction; }

        /**
         * Get a point along the ray at parameter t
         * @param t Parameter value (t >= 0 for valid ray points)
         * @return Vector3D Point at origin + t * direction
         */
        Vector3D getPointAt(double t) const;

        /**
         * Get the closest point on the ray to a given point
         * @param point The point to find closest ray point to
         * @return Vector3D Closest point on the ray
         */
        Vector3D closestPointTo(const Vector3D& point) const;

        /**
         * Get the parameter t for the closest point on ray to a given point
         * @param point The point to find closest ray point to
         * @return double Parameter t (clamped to t >= 0)
         */
        double closestParameterTo(const Vector3D& point) const;

        /**
         * Calculate distance from ray to a point
         * @param point The point to calculate distance to
         * @return double Distance from ray to the point
         */
        double distanceToPoint(const Vector3D& point) const;

        /**
         * Check if a point lies on the ray
         * @param point The point to check
         * @param tolerance Tolerance for floating point comparison
         * @return bool True if the point is on the ray
         */
        bool containsPoint(const Vector3D& point, double tolerance = 1e-9) const;

        /**
         * Transform the ray by moving its origin
         * @param offset Vector to translate the ray origin
         * @return Ray New ray with translated origin
         */
        Ray translate(const Vector3D& offset) const;

        /**
         * Set the origin of the ray
         * @param newOrigin The new origin point
         */
        void setOrigin(const Vector3D& newOrigin);

        /**
         * Set the direction of the ray (will be normalized)
         * @param newDirection The new direction vector
         */
        void setDirection(const Vector3D& newDirection);

        /**
         * Check if the ray is valid (non-zero direction)
         * @return bool True if the ray is valid
         */
        bool isValid() const;

        /**
         * Check if two rays are parallel
         * @param other The other ray to compare with
         * @param tolerance Tolerance for floating point comparison
         * @return bool True if rays are parallel
         */
        bool isParallelTo(const Ray& other, double tolerance = 1e-9) const;

        /**
         * Find the closest points between two rays
         * @param other The other ray
         * @param t1 Reference to store parameter for this ray
         * @param t2 Reference to store parameter for other ray
         * @return double Distance between the closest points
         */
        double closestPointsBetweenRays(const Ray& other, double& t1, double& t2) const;

    private:
        Vector3D origin;
        Vector3D direction;  // Always normalized
    };

} // namespace geometry

#endif // RAY_H
