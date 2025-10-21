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
        Camera(Rectangle viewport);

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
         * Get the position of the camera (origin of the viewport)
         * @return Vector3D The position of the camera
         */
        Vector3D getPosition() const;

        /**
         * Get the origin point of the camera's field of view (FOV)
         * @return Vector3D The FOV origin point
         */
        Vector3D getFOVOrigin() const;

        /**
         * Render the scene from the camera's perspective
         * @tparam T The geometry type of the shapes to render
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @param shapes The vector of shapes in the scene
         * @return Image The rendered image
         */
        using ShapeVariant = std::variant<Shape<Box>, Shape<Circle>, Shape<Plane>, Shape<Rectangle>, Shape<Sphere>>;
        Image renderScene2DColor(int imageWidth, int imageHeight, math::Vector<ShapeVariant> shapes) const;

        /**
         * Render the depth map of the scene from the camera's perspective
         * @tparam T The geometry type of the shapes to render
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @param shapes The vector of shapes in the scene
         * @return Image The rendered depth map image
         */
        Image renderScene3DDepth(int imageWidth, int imageHeight, math::Vector<ShapeVariant> shapes) const;

        /**
         * Generate a ray using a point on the viewport and the normal vector
         * @param pointOnViewport A point on the viewport rectangle
         * @return Ray The generated ray from the camera through the point
         */
        Ray generateRay(const Vector3D& pointOnViewport) const;
    private:
        Rectangle viewport;
        float FOV_Angle = 65.0f; // Field of View angle degrees
    };

}

#endif // CAMERA_H