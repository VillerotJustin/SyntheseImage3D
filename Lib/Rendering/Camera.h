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
#include "./Light.h"


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
        Camera(const Rectangle& viewport, float FOV_Angle = 65.0f);

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
         * Get the viewport aspect ratio (width / length)
         * @return double The viewport aspect ratio
         */
        double getViewportAspectRatio() const;

        /**
         * Get the camera's field of view (FOV) angle
         * @return double The FOV angle in degrees
         */
        double getFOVAngle() const;

        /**
         * Set the camera's field of view (FOV) angle
         * @param angle The new FOV angle in degrees
         */
        void setFOVAngle(double angle);

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
         * Get the origin point of the camera's field of view (FOV)
         * @return Vector3D The FOV origin point
         */
        Vector3D getFOVOrigin() const;

        /**
         * Generate a ray using a point on the viewport and the normal vector
         * @param pointOnViewport A point on the viewport rectangle
         * @return Ray The generated ray from the camera through the point
         */
        Ray generateRay(const Vector3D& pointOnViewport) const;

        Ray generateRayForPixel(size_t pixelX, size_t pixelY, size_t imageWidth, size_t imageHeight, bool is3D) const;

        /**
         * Render the scene from the camera's perspective
         * @tparam T The geometry type of the shapes to render
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @param shapes The vector of shapes in the scene
         * @return Image The rendered image
         */
        Image renderScene2DColor(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes) const;

        /**
         * Render the scene to a depth map from the camera's perspective
         * @tparam T The geometry type of the shapes to render
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @param shapes The vector of shapes in the scene
         * @return Image The rendered depth map image
         */
        Image renderScene2DDepth(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes) const;

        /**
         * Render the depth map of the scene from the camera's perspective
         * @tparam T The geometry type of the shapes to render
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @param shapes The vector of shapes in the scene
         * @return Image The rendered depth map image
         */
        Image renderScene3DColor(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes) const;

        /**
         * Render the depth map of the scene from the camera's perspective
         * @tparam T The geometry type of the shapes to render
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @param shapes The vector of shapes in the scene
         * @return Image The rendered depth map image
         */
        Image renderScene3DDepth(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes) const;

        /**
         * Render the depth map of the scene from the camera's perspective
         * @tparam T The geometry type of the shapes to render
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @param shapes The vector of shapes in the scene
         * @param lights The vector of lights in the scene
         * @return Image The rendered depth map image
         */
        Image renderScene3DLight(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes, math::Vector<Light> lights) const;

        // Enum for anti-aliasing methods
        enum class AntiAliasingMethod {
            NONE,
            SSAA,
            MSAA,
            FXAA
        };

        Image renderScene3DLight_AA(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes, math::Vector<Light> lights, size_t samplesPerPixel = 8, AntiAliasingMethod method = AntiAliasingMethod::NONE) const;

    private:
        Rectangle viewport;
        double FOV_Angle = 65.0f; // Field of View angle degrees
    };

}

#endif // CAMERA_H