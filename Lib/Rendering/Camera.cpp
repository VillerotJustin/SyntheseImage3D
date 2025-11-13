//
// Created by villerot on 03/10/2025.
//

#include "Camera.h"
#include "../Math/Matrix.hpp"
#include "../Math/Vector.hpp"

#include <omp.h>
#include <limits>
#include <cmath>
#include <algorithm>

namespace rendering {

    // Constructor now matches header: takes a Rectangle and optional FOV angle
    Camera::Camera(const Rectangle& viewport, float FOV_Angle)
        : viewport(viewport), FOV_Angle(FOV_Angle) {
    }

    const Rectangle& Camera::getViewport() const {
        return viewport;
    }

    void Camera::setViewport(const Rectangle& viewport) {
        this->viewport = viewport;
    }

    const Vector3D& Camera::getPosition() const {
        return viewport.getOrigin();
    }

    void Camera::setPosition(const Vector3D& position) {
        // Apply translation and store the updated viewport
        viewport = viewport.translate(position - viewport.getOrigin());
    }

    const Vector3D& Camera::getDirection() const {
        return viewport.getNormal();
    }

    void Camera::setDirection(const Vector3D& direction) {
        Quaternion rotationQuat = Quaternion::fromVectorToVector(viewport.getNormal(), direction);
        
        // Store the exact dimensions before rotation to preserve aspect ratio perfectly
        double length = viewport.getLength();
        double width = viewport.getWidth();
        Vector3D origin = viewport.getOrigin();
        
        // Rotate the direction vectors and renormalize to prevent numerical drift
        Vector3D newLengthDir = (rotationQuat * viewport.getLengthVec()).normal();
        Vector3D newWidthDir = (rotationQuat * viewport.getWidthVec()).normal();
        
        // Reconstruct the rectangle
        viewport = Rectangle(origin, origin + length * newLengthDir, origin + width * newWidthDir);
        
        // Force exact dimensions to prevent any floating-point drift
        viewport.setDimensions(length, width);
    }

    double Camera::getViewportWidth() const {
        return viewport.getWidth();
    }

    void Camera::setViewportWidth(double width) {
        // Recreate rectangle with new width
        viewport.scale(1.0, width / viewport.getWidth());
    }

    double Camera::getViewportLength() const {
        return viewport.getLength();
    }

    void Camera::setViewportLength(double length) {
        // Recreate rectangle with new length
        viewport.scale(length / viewport.getLength(), 1.0);
    }

    double Camera::getViewportAspectRatio() const {
        return viewport.getWidth() / viewport.getLength();
    }

    double Camera::getFOVAngle() const {
        return FOV_Angle;
    }

    void Camera::setFOVAngle(double angle) {
        FOV_Angle = angle;
    }

    void Camera::rotate(Quaternion rotation) {
        viewport = viewport.rotate(rotation);
    }

    void Camera::translate(const Vector3D& translation) {
        viewport = viewport.translate(translation);
    }

    Vector3D Camera::getFOVOrigin() const {
        // Compute the FOV origin using the viewport center and FOV_Angle (in degrees)
        Vector3D center = viewport.getCenter();
        Vector3D normal = viewport.getNormal();
        double fovAngleRad = static_cast<double>(FOV_Angle) * M_PI / 180.0;
        double halfHeight = viewport.getWidth() / 2.0;
        double distance = halfHeight / std::tan(fovAngleRad / 2.0);
        Vector3D fovOrigin = center - normal * distance;
        return fovOrigin;
    }

    Ray Camera::generateRay(const Vector3D& pointOnViewport) const {
        if (!viewport.containsPoint(pointOnViewport)) {
            throw std::invalid_argument("Point is not on the viewport rectangle");
        }
        return Ray(pointOnViewport, viewport.getNormal());
    }

    // ========== Helper Methods ==========

    // Function moved to CameraHelper.cpp
    
    // ========== Rendering Methods ==========

    // Function implementations moved to CameraRenderers.cpp

}
