//
// Created by villerot on 10/03/25.
//

#ifndef PLANE_H
#define PLANE_H

#include "./Vector3D.h"
#include "./Ray.h"

#include <optional>
namespace geometry {

    class Plane {
    public:
        /**
         * Constructor for Plane from a point and normal vector
         * @param origin A point on the plane
         * @param normal Normal vector to the plane (will be normalized)
         */
        Plane(const Vector3D& origin, const Vector3D& normal);

        /**
         * Constructor for Plane from three points
         * @param point1 First point on the plane
         * @param point2 Second point on the plane
         * @param point3 Third point on the plane
         */
        Plane(const Vector3D& point1, const Vector3D& point2, const Vector3D& point3);

        /**
         * Get the origin point of the plane
         * @return Vector3D Origin point on the plane
         */
        const Vector3D& getOrigin() const { return origin; }

        /**
         * Get the normal vector of the plane
         * @return Vector3D Normal vector to the plane
         */
        const Vector3D& getNormal() const { return normal; }

        /**
         * Calculate the distance from a point to the plane
         * @param point The point to calculate distance from
         * @return double Signed distance to the plane (positive if on normal side)
         */
        double distanceToPoint(const Vector3D& point) const;

        /**
         * Check if a point lies on the plane
         * @param point The point to check
         * @param tolerance Tolerance for floating point comparison
         * @return bool True if the point is on the plane
         */
        bool containsPoint(const Vector3D& point, double tolerance = 1e-9) const;

        /**
         * Project a point onto the plane
         * @param point The point to project
         * @return Vector3D The projected point on the plane
         */
        Vector3D projectPoint(const Vector3D& point) const;

        /**
         * Get the intersection point of a line with the plane
         * @param linePoint A point on the line
         * @param lineDirection Direction vector of the line
         * @return Vector3D Intersection point (if line is not parallel to plane)
         */
        Vector3D lineIntersection(const Vector3D& linePoint, const Vector3D& lineDirection) const;

        /**
         * Check if a line is parallel to the plane
         * @param lineDirection Direction vector of the line
         * @param tolerance Tolerance for floating point comparison
         * @return bool True if the line is parallel to the plane
         */
        bool isLineParallel(const Vector3D& lineDirection, double tolerance = 1e-9) const;

        /**
         * Set the origin point of the plane
         * @param newOrigin The new origin point
         */
        void setOrigin(const Vector3D& newOrigin);

        /**
         * Set the normal vector of the plane (will be normalized)
         * @param newNormal The new normal vector
         */
        void setNormal(const Vector3D& newNormal);

        /**
         * Check if the plane is valid (non-zero normal)
         * @return bool True if the plane is valid
         */
        bool isValid() const;

        /**
         * Get the plane equation coefficients (ax + by + cz + d = 0)
         * @param a Reference to store the x coefficient
         * @param b Reference to store the y coefficient
         * @param c Reference to store the z coefficient
         * @param d Reference to store the constant term
         */
        void getPlaneEquation(double& a, double& b, double& c, double& d) const;

        /**
         * Get the normal vector at a point on the plane
         * @param point The point on the plane
         * @return Vector3D The normal vector at the point
         */
        Vector3D getNormalAt(const Vector3D& point) const;

        /**
         * Check if a ray intersects with the box
         * @param ray The ray to check intersection with
         * @return bool True if the ray intersects the box
         */
        bool rayIntersect(const Ray& ray) const;

        std::optional<double> rayIntersectDepth(const Ray& ray, double tmax = std::numeric_limits<double>::max()) const;


    private:
        Vector3D normal;
        Vector3D origin;
    };

} // namespace geometry

#endif // PLANE_H
