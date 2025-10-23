/**
 * 3D Box class
 *

 * 
 * Created by villerot on 10/03/25.
 */

#ifndef BOX_H
#define BOX_H

#include "./Vector3D.h"
#include "./Ray.h"
#include "./Edge.h"
#include "./Rectangle.h"

#include <optional>
#include <variant>
#include <stdexcept>

namespace geometry {

    /**
     * @class Box
     * @brief A class representing a box in 3D space.
     * 
     * Why make long explanations when a simple diagram does the job ?
     * Represents box in 3D space.
     *       .---------.
     *      /|  ^     /|
     *    d/ |  n    / |
     *    /  |      /  |
     *   O---------.   |
     *   |   |     |   |
     *   |   |     |   |
     *   |   |     |   |
     * h |   |     |   |
     *   |   .-----|---.
     *   |  /      |  /
     *   | /       | /
     *   |/        |/
     *   .---------.
     *       w
     * 
     * O -> Origin (minimum corner)
     * w -> Width
     * h -> Height
     * d -> Depth 
     * n -> Normal vector of top face
     */
    class Box {
    public:
        /**
         * Constructor for Box
         * @param origin Origin point of the box (corner)
         * @param w Width of the box (x-axis)
         * @param h Height of the box (y-axis)
         * @param p Depth of the box (z-axis)
         * @param normal Normal vector for orientation (will be normalized)
         */
        Box(const Vector3D& origin, double w, double h, double p, const Vector3D& normal);

        /**
         * Get the origin point of the box
         * @return Vector3D Origin point of the box
         */
        const Vector3D& getOrigin() const { return origin; }

        /**
         * Get the width of the box
         * @return double Width of the box
         */
        double getWidth() const { return w; }

        /**
         * Get the height of the box
         * @return double Height of the box
         */
        double getHeight() const { return h; }

        /**
         * Get the depth of the box
         * @return double Depth of the box
         */
        double getDepth() const { return p; }

        /**
         * Get the normal vector of the box
         * @return Vector3D Normal vector of the box
         */
        const Vector3D& getNormal() const { return normal; }

        /**
         * Get the volume of the box
         * @return double Volume of the box
         */
        double getVolume() const;

        /**
         * Get the surface area of the box
         * @return double Surface area of the box
         */
        double getSurfaceArea() const;

        /**
         * Check if a point is inside the box
         * @param point The point to check
         * @return bool True if the point is inside the box
         */
        bool containsPoint(const Vector3D& point) const;

        /**
         * Check if a point is on the surface of the box
         * @param point The point to check
         * @param tolerance Tolerance for floating point comparison
         * @return bool True if the point is on the surface
         */
        bool isPointOnSurface(const Vector3D& point) const;

        /**
         * Get the center point of the box
         * @return Vector3D Center point of the box
         */
        Vector3D getCenter() const;

        /**
         * Get the minimum corner of the box (origin)
         * @return Vector3D Minimum corner coordinates
         */
        Vector3D getMinCorner() const;

        /**
         * Get the maximum corner of the box
         * @return Vector3D Maximum corner coordinates
         */
        Vector3D getMaxCorner() const;

        /**
         * Get all 8 corners of the box
         * @param corners Array to store the 8 corner points
         */
        void getCorners(Vector3D corners[8]) const;

        /**
         * Check if this box intersects with another box
         * @param other The other box to check intersection with
         * @return bool True if boxes intersect
         */
        bool intersects(const Box& other) const;

        /**
         * Get the intersection box with another box
         * @param other The other box
         * @return Vector3D, Edge, Rectangle or Box representing the intersection area
         *         std::nullopt if there is no intersection
         * @throws std::runtime_error if intersection logic is not implemented yet
         */
        std::optional<std::variant<Vector3D, Edge, Rectangle, Box>> intersectionPoints(const Box& other) const;

        /**
         * Expand the box by a given amount in all directions
         * @param amount Amount to expand
         * @return Box New expanded box
         */
        Box expand(double amount) const;

        /**
         * Translate the box by an offset
         * @param offset Vector to translate the box
         * @return Box New translated box
         */
        Box translate(const Vector3D& offset) const;

        /**
         * Set the origin of the box
         * @param newOrigin The new origin point
         */
        void setOrigin(const Vector3D& newOrigin);

        /**
         * Set the dimensions of the box
         * @param newW New width
         * @param newH New height
         * @param newP New depth
         */
        void setDimensions(double newW, double newH, double newP);

        /**
         * Set the normal vector of the box (will be normalized)
         * @param newNormal The new normal vector
         */
        void setNormal(const Vector3D& newNormal);

        /**
         * Check if the box is valid (positive dimensions)
         * @return bool True if the box is valid
         */
        bool isValid() const;

        /**
         * Get the normal vector at a given point on the box surface
         * @param point The point on the box surface
         * @return Vector3D Normal vector at the point
         * @throws std::runtime_error if the point is not on the surface
         */
        Vector3D getNormalAt(const Vector3D& point) const;

        /**
         * Check if a ray intersects with the box
         * @param ray The ray to check intersection with
         * @return bool True if the ray intersects the box
         */
        bool rayIntersect(const Ray& ray) const;

        /**
         * Get the intersection depth of a ray with the box
         * @param ray The ray to check intersection with
         * @return std::optional<double> Depth of intersection if it exists, std::nullopt otherwise
         */
        std::optional<double> rayIntersectDepth(const Ray& ray) const;

    private:
        Vector3D origin;  // Origin point (minimum corner)
        double w;         // Width (x-axis)
        double h;         // Height (y-axis)
        double p;         // Depth (z-axis)
        Vector3D normal;  // Normal vector for orientation
    };

} // namespace geometry

#endif // BOX_H
