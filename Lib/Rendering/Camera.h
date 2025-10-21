//
// Created by villerot on 03/10/2025.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "../Geometry/Vector3D.h"
#include "../Geometry/Quaternion.h"
#include "../Geometry/Ray.h"
#include "../Geometry/Rectangle.h"
#include "../Geometry/Box.h"
#include "../Geometry/Circle.h"
#include "../Geometry/Plane.h"
#include "../Geometry/Sphere.h"

#include "../Math/Vector.hpp"

#include "./Image.h"
#include "./Shape.hpp"


using namespace geometry;

namespace rendering {

    class Camera {
    public:
        // Type alias for shape variants
        using ShapeVariant = std::variant<Shape<Box>, Shape<Circle>, Shape<Plane>, Shape<Rectangle>, Shape<Sphere>>;

        /**
         * Constructor for Camera
         * @param position The center position of the camera in world space
         * @param viewportWidth The width of the camera's viewport
         * @param viewportLength The length of the camera's viewport
         * @param direction The direction vector the camera is facing (will be normalized)
         */
        Camera(const Vector3D& position, double viewportWidth, double viewportLength, const Vector3D& direction);

        /**
         * Get the viewport rectangle of the camera
         * @return Rectangle The viewport rectangle
         */
        const Rectangle& getViewport() const;

        /**
         * Set the viewport rectangle of the camera
         * @param viewport The new viewport rectangle
         */
        void setViewport(const Rectangle& viewport);

        /**
         * Get the camera position
         * @return Vector3D The camera center position
         */
        const Vector3D& getPosition() const;

        /**
         * Set the camera position
         * @param position The new camera center position
         */
        void setPosition(const Vector3D& position);

        /**
         * Get the camera direction
         * @return Vector3D The normalized direction vector
         */
        const Vector3D& getDirection() const;

        /**
         * Set the camera direction
         * @param direction The new direction vector (will be normalized)
         */
        void setDirection(const Vector3D& direction);

        /**
         * Get the viewport width
         * @return double The viewport width
         */
        double getViewportWidth() const;

        /**
         * Set the viewport width
         * @param width The new viewport width
         */
        void setViewportWidth(double width);

        /**
         * Get the viewport length
         * @return double The viewport length
         */
        double getViewportLength() const;

        /**
         * Set the viewport length
         * @param length The new viewport length
         */
        void setViewportLength(double length);

        /**
        * Rotate the camera around its origin by a given quaternion
        * @param rotation The quaternion representing the rotation
        */
        void rotate(Quaternion rotation);

        /**
         * Translate the camera by a given vector
         * @param translation The vector to translate the camera
         */
        void translate(const Vector3D& translation);

        /**
         * Render the scene from the camera's perspective
         * @tparam T The geometry type of the shapes to render
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @param shapes The vector of shapes in the scene
         * @return Image The rendered image
         */
        Image renderScene2DColor(int imageWidth, int imageHeight, math::Vector<ShapeVariant> shapes) const;

        /**
         * Render the scene to a depth map from the camera's perspective
         * @tparam T The geometry type of the shapes to render
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @param shapes The vector of shapes in the scene
         * @return Image The rendered depth map image
         */
        Image renderScene2DDepth(int imageWidth, int imageHeight, math::Vector<ShapeVariant> shapes) const;

        /**
         * Generate a ray using a point on the viewport and the normal vector
         * @param pointOnViewport A point on the viewport rectangle
         * @return Ray The generated ray from the camera through the point
         */
        Ray generateRay(const Vector3D& pointOnViewport) const;
    private:
        Vector3D position;   // Camera center position in world space
        double viewportWidth;  // Width of the viewport
        double viewportLength; // Length of the viewport 
        Vector3D direction;  // Direction the camera is facing (normalized)
        Rectangle viewport;  // Generated from camera parameters
        double focalLength = 1.0; // Focal length for perspective projection
        
        /**
         * Update the internal viewport rectangle based on current camera parameters
         */
        void updateViewport();
    };

}

#endif // CAMERA_H