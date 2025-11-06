//
// Created by villerot on 10/03/25.
//

#ifndef SPHERE_H
#define SPHERE_H


#include <iostream>

#include "./Vector3D.h"
#include "./Box.h"
#include "./Ray.h"
#include "./Plane.h"
#include "./Circle.h"

#include "../Math/math_common.h"

#include <optional>
#include <variant>

namespace geometry {

    class Sphere {
    public:
        // Constructors
        /**
         * Constructor for Sphere
         * @param Vector3D center Center of the sphere
         * @param double radius Radius of the sphere
         */
        Sphere(const Vector3D& center, double radius);

        /**
         * Destructor for Sphere
         */
        ~Sphere() = default;

        // Accessors
        /**
         * Get the center of the sphere
         * @return Vector3D Center of the sphere
         */
        const Vector3D& getCenter() const { return center; }

        /**
         * Get the radius of the sphere
         * @return double Radius of the sphere
         */
        double getRadius() const { return radius; }

        /**
         * Get the surface area of the sphere
         * @return double Surface area of the sphere
         */
        double getSurfaceArea() const;

        /**
         * Get the volume of the sphere
         * @return double Volume of the sphere
         */
        double getVolume() const;

        // Core Geometry Methods

        /**
         * Check if a point is inside the sphere
         * @param Vector3D point Point to check
         * @return bool True if the point is inside the sphere, false otherwise
         */
        bool containsPoint(const Vector3D& point) const;

        /**
         * Check if a point is on the surface of the sphere
         * @param Vector3D point Point to check
         * @return bool True if the point is on the surface of the sphere, false otherwise
         */
        bool surfacePoint(const Vector3D& point) const;

        /**
         * Get the distance from the center of the sphere to a point
         * @param Vector3D point Point to check
         * @return double Distance from the center of the sphere to the point
         */
        double distanceToPoint(const Vector3D& point) const;

        /**
         * Get the closest point on the surface of the sphere to a given point
         * @param Vector3D point Point to check
         * @return Vector3D Closest point on the surface of the sphere
         */
        Vector3D closestPointOnSurface(const Vector3D& point) const;

        // Intersections

        /**
         * Check if two spheres intersect
         * @param Sphere other Other sphere to check
         * @return bool True if the spheres intersect, false otherwise
         */
        bool intersects(const Sphere& other) const;
        
        /**
         * Get the intersection points of two spheres (if they intersect)
         * @param Sphere other Other sphere to check
         * @return std::optional<std::variant<Circle, Vector3D, Sphere>> point if tangent, Circle if intersecting in a circle, sphere if equal, nullopt if no intersection
         */
        std::optional<std::variant<Circle, Vector3D, Sphere>> intersectionPoints(const Sphere& other) const;

        /**
         * Get the normal vector at a point on the surface of the sphere
         * @param Vector3D point Point on the surface
         * @return Vector3D Normal vector at the point
         */
        Vector3D getNormalAt(const Vector3D& point) const;

        /**
         * Check if a ray intersects the sphere
         * @param Ray ray The ray to check
         * @param double t step while advancing the ray (default 0.01)
         * @return bool True if the ray intersects the sphere, false otherwise
         */
        bool rayMarch(const Ray& ray, double t = 0.01) const;


        /**
         * Get the distance from the ray origin to the intersection point with the sphere
         * @param Ray ray The ray to check
         * @param double t step while advancing the ray (default 0.01)
         * @return double Distance to intersection point, or -1 if no intersection
         */
        std::optional<double> rayMarchDistance(const Ray& ray, double t = 0.01) const;

        /**
         * Check if a ray intersects the sphere
         * @param Ray ray The ray to check
         * @return bool True if the ray intersects the sphere, false otherwise
         */
        bool rayIntersect(const Ray& ray) const;

        /**
         * Get the distance from the ray origin to the intersection point with the sphere
         * @param Ray ray The ray to check
         * @return std::optional<double> Distance to intersection point, or nullopt if no intersection
         */
        std::optional<double> rayIntersectDepth(const Ray& ray, double tmax = std::numeric_limits<double>::max()) const;

        /**
         * Get the distance from the ray origin to the intersection point with the sphere
         * @param Ray ray The ray to check
         * @return std::optional<double> Distance to intersection point, or nullopt if no intersection
         */
        std::optional<Vector3D> rayIntersectionHit(const Ray& ray) const;

        /**
         * Check if a line intersects the sphere
         */
        bool lineIntersects(const Vector3D& linePoint, const Vector3D& lineDirection) const;

        // Utility Methods

        /**
         * Translate the sphere by a given vector
         * @param Vector3D translation Vector to translate the sphere
         */
        void translate(const Vector3D& translation);

        /**
         * Scale the sphere by a given factor
         * @param double factor Factor to scale the sphere
         */
        void scale(double factor);

        /**
         * Get the axis-aligned bounding box of the sphere
         * @return Box Axis-aligned bounding box of the sphere
         */
        Box getBoundingBox() const;

        /**
         * Generate a random point on the surface of the sphere
         * @return Vector3D Random point on the surface of the sphere
         */
        Vector3D randomPointOnSurface() const;

        /**
         * Generate a random point inside the sphere
         * @return Vector3D Random point inside the sphere
         */
        Vector3D randomPointInside() const;

        // Advanced Methods

        /**
         * Get the tangent plane at a given point on the surface of the sphere
         * @param Vector3D point Point on the surface of the sphere
         * @return Plane Tangent plane at the given point
         * @throws std::invalid_argument if the point is not on the surface of the sphere
         */
        Plane tangentPlaneAtPoint(const Vector3D& point) const;

        /**
         * Get the normal vector at a given point on the surface of the sphere
         * @param Vector3D point Point on the surface of the sphere
         * @return Vector3D Normal vector at the given point
         * @throws std::invalid_argument if the point is not on the surface of the sphere
         */
        Vector3D normalAt(const Vector3D& point) const;

        /**
         * Project a point onto the surface of the sphere
         * @param Vector3D point Point to project
         * @return Vector3D Projected point on the surface of the sphere
         */
        Vector3D projectPointOntoSurface(const Vector3D& point) const;

        // Comparison Operators
        bool operator==(const Sphere& other) const;
        bool operator!=(const Sphere& other) const;

        bool equals(const Sphere& other, double epsilon = 1e-6) const;

        bool isValid() const;

        // ToString

        /**
         * @brief Output stream operator for debugging and display purposes.
         * @param os The output stream.
         * @param s The sphere to output.
         * @return The output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const Sphere& s);


    private:
        Vector3D center;
        double radius;
    };

} // namespace geometry

#endif // SPHERE_H