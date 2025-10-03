//
// Created by villerot on 10/03/25.
//

#ifndef CIRCLE_H
#define CIRCLE_H

#include "./Vector3D.h"

namespace geometry {

    class Circle {
    public:
        /**
         * Constructor for Circle
         * @param Vector3D center Center of the circle
         * @param double radius Radius of the circle
         * @param Vector3D normal Normal vector to the plane containing the circle
         */
        Circle(const Vector3D& center, double radius, const Vector3D& normal);

        /**
         * Get the center of the circle
         * @return Vector3D Center of the circle
         */
        const Vector3D& getCenter() const { return center; }

        /**
         * Get the radius of the circle
         * @return double Radius of the circle
         */
        double getRadius() const { return radius; }

        /**
         * Get the normal vector of the circle's plane
         * @return Vector3D Normal vector to the plane containing the circle
         */
        const Vector3D& getNormal() const { return normal; }

        /**
         * Get the circumference of the circle
         * @return double Circumference of the circle
         */
        double getCircumference() const;

        /**
         * Get the area of the circle
         * @return double Area of the circle
         */
        double getArea() const;

        /**
         * Check if a point lies on the circle
         * @param point The point to check
         * @param tolerance Tolerance for floating point comparison
         * @return bool True if the point is on the circle
         */
        bool containsPoint(const Vector3D& point, double tolerance = 1e-9) const;

        /**
         * Get a point on the circle at a given angle
         * @param angle Angle in radians (0 to 2π)
         * @return Vector3D Point on the circle at the specified angle
         */
        Vector3D getPointAtAngle(double angle) const;

        /**
         * Set the normal vector (will be normalized)
         * @param newNormal The new normal vector
         */
        void setNormal(const Vector3D& newNormal);

        /**
         * Set the center of the circle
         * @param newCenter The new center position
         */
        void setCenter(const Vector3D& newCenter);

        /**
         * Set the radius of the circle
         * @param newRadius The new radius (must be positive)
         */
        void setRadius(double newRadius);

        /**
         * Check if the circle is valid (positive radius and non-zero normal)
         * @return bool True if the circle is valid
         */
        bool isValid() const;

    private:
        Vector3D center;
        double radius;
        Vector3D normal;  // Normal vector to the plane containing the circle
        
        /**
         * Generate two orthogonal vectors in the plane of the circle
         * Used for parametric representation of points on the circle
         */
        void generateBasisVectors(Vector3D& u, Vector3D& v) const;
    };

} // namespace geometry
#endif // CIRCLE_H
