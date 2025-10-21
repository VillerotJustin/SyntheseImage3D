//
// Created by villerot on 10/03/25.
//

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "./Vector3D.h"
#include "./Quaternion.h"
#include "./Ray.h"

namespace geometry {

    /**
     * @class Rectangle
     * @brief A class representing a rectangle in 3D space.
     * 
     * Rectangle layout:
     *      l
     * O----------.
     * |     ^    |
     * |     n    | w
     * |           |
     * .-----------.
     * 
     * O -> Origin (corner point)
     * l -> Length 
     * w -> Width
     * n -> Normal vector (perpendicular to rectangle plane)
     */
    class Rectangle {
    public:
        /**
         * Constructor for Rectangle
         * @param origin Origin point of the rectangle (corner)
         * @param l Length of the rectangle
         * @param w Width of the rectangle
         * @param normal Normal vector perpendicular to the rectangle plane (will be normalized)
         * deprecated Use the constructor with three corner points instead.
         */
        //Rectangle(const Vector3D& origin, double l, double w, const Vector3D& normal);

        /**
         * Constructor for Rectangle using three corner points
         * @param TopLeft Top-left corner point of the rectangle
         * @param TopRight Top-right corner point of the rectangle
         * @param BottomLeft Bottom-left corner point of the rectangle
         */
        Rectangle(const Vector3D& TopLeft, const Vector3D& TopRight, const Vector3D& BottomLeft);

        /**
         * Get the origin point of the rectangle
         * @return Vector3D Origin point of the rectangle
         */
        const Vector3D& getOrigin() const { return origin; }

        /**
         * Get the length of the rectangle
         * @return double Length of the rectangle
         */
        double getLength() const { return l; }

        /**
         * Get the width of the rectangle
         * @return double Width of the rectangle
         */
        double getWidth() const { return w; }

        /**
         * Get the normal vector of the rectangle
         * @return Vector3D Normal vector perpendicular to the rectangle plane
         */
        const Vector3D& getNormal() const { return normal; }

        /**
         * Get the area of the rectangle
         * @return double Area of the rectangle (l * w)
         */
        double getArea() const;

        /**
         * Get the perimeter of the rectangle
         * @return double Perimeter of the rectangle (2 * (l + w))
         */
        double getPerimeter() const;

        /**
         * Get the center point of the rectangle
         * @return Vector3D Center point of the rectangle
         */
        Vector3D getCenter() const;

        /**
         * Get all 4 corners of the rectangle
         * @param corners Array to store the 4 corner points
         */
        void getCorners(Vector3D corners[4]) const;

        /**
         * Get the length direction vector (unit vector)
         * @return Vector3D Length direction vector
         */
        Vector3D getLengthVec() const;

        /**
         * Get the width direction vector (unit vector)
         * @return Vector3D Width direction vector
         */
        Vector3D getWidthVec() const;

        /**
         * Check if a point lies within the rectangle
         * @param point The point to check
         * @param tolerance Tolerance for floating point comparison
         * @return bool True if the point is within the rectangle
         */
        bool containsPoint(const Vector3D& point, double tolerance = 1e-9) const;

        /**
         * Check if a point is on the edge of the rectangle
         * @param point The point to check
         * @param tolerance Tolerance for floating point comparison
         * @return bool True if the point is on the rectangle edge
         */
        bool isPointOnEdge(const Vector3D& point, double tolerance = 1e-9) const;

        /**
         * Project a point onto the rectangle plane
         * @param point The point to project
         * @return Vector3D The projected point on the rectangle plane
         */
        Vector3D projectPointToPlane(const Vector3D& point) const;

        /**
         * Get the closest point on the rectangle to a given point
         * @param point The point to find closest rectangle point to
         * @return Vector3D Closest point on the rectangle
         */
        Vector3D closestPointOnRectangle(const Vector3D& point) const;

        /**
         * Calculate distance from a point to the rectangle
         * @param point The point to calculate distance from
         * @return double Distance from point to the rectangle
         */
        double distanceToPoint(const Vector3D& point) const;

        /**
         * Get a point on the rectangle using parametric coordinates
         * @param u Parameter in range [0,1] along length direction
         * @param v Parameter in range [0,1] along width direction
         * @return Vector3D Point on the rectangle at (u,v)
         */
        Vector3D getPointAt(double u, double v) const;

        /**
         * Translate the rectangle by an offset
         * @param offset Vector to translate the rectangle
         * @return Rectangle New translated rectangle
         */
        Rectangle translate(const Vector3D& offset) const;

        /**
         * Scale the rectangle dimensions
         * @param lengthScale Scale factor for length
         * @param widthScale Scale factor for width
         * @return Rectangle New scaled rectangle
         */
        Rectangle scale(double lengthScale, double widthScale) const;

        /**
         * Rotate the rectangle by a quaternion
         * @param rotation The quaternion representing the rotation
         * @return Rectangle New rotated rectangle
         */
        Rectangle rotate(Quaternion rotation) const;

        /**
         * Set the origin of the rectangle
         * @param newOrigin The new origin point
         */
        void setOrigin(const Vector3D& newOrigin);

        /**
         * Set the dimensions of the rectangle
         * @param newL New length
         * @param newW New width
         */
        void setDimensions(double newL, double newW);

        /**
         * Set the normal vector of the rectangle (will be normalized)
         * @param newNormal The new normal vector
         */
        void setNormal(const Vector3D& newNormal);

        /**
         * Check if the rectangle is valid (positive dimensions and non-zero normal)
         * @return bool True if the rectangle is valid
         */
        bool isValid() const;

        /**
         * Check if a ray intersects with the box
         * @param ray The ray to check intersection with
         * @return bool True if the ray intersects the box
         */
        bool rayIntersect(const Ray& ray) const;

    private:
        Vector3D origin;  // Origin point (corner)
        Vector3D lengthDir; // Length direction unit vector
        Vector3D widthDir;  // Width direction unit vector
        double l;         // Length
        double w;         // Width  
        Vector3D normal;  // Normal vector (perpendicular to rectangle plane)
    };

} // namespace geometry

#endif // RECTANGLE_H
