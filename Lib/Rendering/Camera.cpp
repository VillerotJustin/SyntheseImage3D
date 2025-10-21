//
// Created by villerot on 03/10/2025.
//

#include "Camera.h"
#include "../Math/Matrix.hpp"
#include <limits>

namespace rendering {

    Camera::Camera(Rectangle viewport) : viewport(viewport) {
    }

    const Rectangle& Camera::getViewport() const {
        return viewport;
    }

    void Camera::setViewport(const Rectangle& viewport) {
        this->viewport = viewport;
    }

    void Camera::rotate(Quaternion rotation) {
        viewport = viewport.rotate(rotation);
    }

    void Camera::translate(const Vector3D& translation) {
        viewport = viewport.translate(translation);
    }

    Vector3D Camera::getPosition() const {
        return viewport.getOrigin();
    }

    Vector3D Camera::getFOVOrigin() const {
        // Compute the FOV origin using the viewport center and FOV_Angle (in degrees)
        Vector3D center = viewport.getCenter();
        Vector3D normal = viewport.getNormal();
        // Calculate the distance from the center to the FOV origin using the FOV angle
        // Assume FOV_Angle is the vertical field of view in degrees
        double fovAngleRad = FOV_Angle * M_PI / 180.0;
        double halfHeight = viewport.getWidth() / 2.0;
        double distance = halfHeight / tan(fovAngleRad / 2.0);
        Vector3D fovOrigin = center - normal * distance;
        return fovOrigin;
    }

    Ray Camera::generateRay(const Vector3D& pointOnViewport) const {
        if (!viewport.containsPoint(pointOnViewport)) {
            throw std::invalid_argument("Point is not on the viewport rectangle");
        }
        
        // Create and return the ray
        return Ray(pointOnViewport, viewport.getNormal());
    }

    Image Camera::renderScene2DColor(int imageWidth, int imageHeight, math::Vector<ShapeVariant> shapes) const {
        Image image(imageWidth, imageHeight);

        if (shapes.size() == 0) {
            return image; // Return empty image if no shapes
        }

        Vector3D viewportLengthVec, viewportWidthVec;

        viewportLengthVec = viewport.getLengthVec();
        viewportWidthVec = viewport.getWidthVec();

        // Scale by viewport dimensions
        viewportLengthVec = viewportLengthVec * viewport.getLength();
        viewportWidthVec = viewportWidthVec * viewport.getWidth();

        // For each pixel in the image, generate a ray through the corresponding point on the viewport
        for (int y = 0; y < imageHeight; ++y) {
            for (int x = 0; x < imageWidth; ++x) {
                // Map pixel coordinates to viewport coordinates
                // Use (x + 0.5) / imageWidth to center pixels and ensure values are in [0,1]
                double u = (static_cast<double>(x) + 0.5) / imageWidth;
                double v = (static_cast<double>(y) + 0.5) / imageHeight;
                
                // Map from [0,1] to [0, 1] range for viewport coordinates
                Vector3D pixelPosition = viewport.getOrigin() + 
                                         (u) * viewportLengthVec + 
                                         (v) * viewportWidthVec;
                Ray ray = generateRay(pixelPosition);

                // Track the closest intersection distance and color
                double closestDistance = std::numeric_limits<double>::infinity();
                RGBA_Color pixelColor(0, 0, 0, 1); // Default to black
                bool hitFound = false;

                // Check for intersections with all shapes
                for (int i = 0; i < shapes.size(); ++i) {
                    std::visit([&](auto&& shape) {
                        using T = std::decay_t<decltype(shape)>;
                        double intersectionDistance = std::numeric_limits<double>::infinity();
                        bool intersects = false;
                        
                        if constexpr (std::is_same_v<T, Shape<Box>>) {
                            const Box* box = shape.getGeometry();
                            if (box && box->rayIntersect(ray)) {
                                intersects = true;
                                // Calculate distance from ray origin to intersection
                                // For now, use a simple heuristic based on shape center
                                Vector3D shapeCenter = box->getCenter();
                                intersectionDistance = (ray.getOrigin() - shapeCenter).length();
                            }
                        } else if constexpr (std::is_same_v<T, Shape<Circle>>) {
                            const Circle* circle = shape.getGeometry();
                            if (circle && circle->rayIntersect(ray)) {
                                intersects = true;
                                Vector3D shapeCenter = circle->getCenter();
                                intersectionDistance = (ray.getOrigin() - shapeCenter).length();
                            }
                        } else if constexpr (std::is_same_v<T, Shape<Plane>>) {
                            const Plane* plane = shape.getGeometry();
                            if (plane && plane->rayIntersect(ray)) {
                                intersects = true;
                                Vector3D shapeOrigin = plane->getOrigin();
                                intersectionDistance = (ray.getOrigin() - shapeOrigin).length();
                            }
                        } else if constexpr (std::is_same_v<T, Shape<Rectangle>>) {
                            const Rectangle* rectangle = shape.getGeometry();
                            if (rectangle && rectangle->rayIntersect(ray)) {
                                intersects = true;
                                Vector3D shapeCenter = rectangle->getCenter();
                                intersectionDistance = (ray.getOrigin() - shapeCenter).length();
                            }
                        } else if constexpr (std::is_same_v<T, Shape<Sphere>>) {
                            const Sphere* sphere = shape.getGeometry();
                            if (sphere) {
                                if (auto hitPoint = sphere->rayIntersectionHit(ray)) {
                                    intersects = true;
                                    intersectionDistance = (ray.getOrigin() - *hitPoint).length();
                                }
                            }
                        }
                        
                        // Update pixel color if this intersection is closer
                        if (intersects && intersectionDistance < closestDistance) {
                            closestDistance = intersectionDistance;
                            hitFound = true;
                            
                            // Set appropriate color based on shape type
                            if constexpr (std::is_same_v<T, Shape<Box>>) {
                                pixelColor = shape.getColor() ? *shape.getColor() : RGBA_Color(1, 0, 0, 1); // Red
                            } else if constexpr (std::is_same_v<T, Shape<Circle>>) {
                                pixelColor = shape.getColor() ? *shape.getColor() : RGBA_Color(0, 1, 0, 1); // Green
                            } else if constexpr (std::is_same_v<T, Shape<Plane>>) {
                                pixelColor = shape.getColor() ? *shape.getColor() : RGBA_Color(0.5, 0.5, 0.5, 1); // Gray
                            } else if constexpr (std::is_same_v<T, Shape<Rectangle>>) {
                                pixelColor = shape.getColor() ? *shape.getColor() : RGBA_Color(0, 0, 1, 1); // Blue
                            } else if constexpr (std::is_same_v<T, Shape<Sphere>>) {
                                pixelColor = shape.getColor() ? *shape.getColor() : RGBA_Color(1, 1, 1, 1); // White
                            }
                        }

                    }, *shapes[i]);
                }
                
                // Set the final pixel color (only if we found a hit)
                if (hitFound) {
                    image.setPixel(x, y, pixelColor);
                }
            }
        }
        return image;
    }



//     Image Camera::renderScene3DDepth(const int imageWidth, const int imageHeight, const math::Vector<ShapeVariant>& shapes) const {
//         Vector3D FOV_Origin = this->getFOVOrigin();

//         Rectangle viewport = this->getViewport();

//         Image newImage(imageWidth, imageHeight);
//         math::Matrix<double> depthBuffer(imageWidth, imageHeight);
//         for (size_t i = 0; i < imageWidth; i++)
//         {
//             for (size_t j = 0; j < imageHeight; j++)
//             {
//                 depthBuffer(i, j) = std::numeric_limits<double>::max();
//             }
            
//         }
        

//         Vector3D topLeft = this->getTopLeftCorner();
//         Vector3D lengthVec = this->getLengthVector();

//         for (int y = 0; y < imageHeight; y++) {
//             for (int x = 0; x < imageWidth; x++) {
//                 double u = static_cast<double>(x) / (imageWidth - 1);
//                 double v = static_cast<double>(y) / (imageHeight - 1);

//                 Vector3 viewportPoint = topLeft + u * lengthVec - v * heightVec;
//                 Ray ray(FOV_Origin, (viewportPoint - FOV_Origin).normalize());

//                 RGBA_Color color;
//                 double closestHit = std::numeric_limits<double>::max();

//                 for (const auto& shape : world.getShapes()) {
//                     // Compute intersections and shading here

//                     double hitDistance;

//                     if (shape->intersect(ray, hitDistance)) {
//                         if (hitDistance < closestHit) {
//                             closestHit = hitDistance;
//                             color = shape->GetColor();
//                         }
//                     }
//                 }

//                 if (closestHit < std::numeric_limits<double>::max() && color.isValid()) {
//                     newImage.setPixel(x, y, color);
//                     depthBuffer.set(x, y, closestHit);
//                 } else {
//                     newImage.setPixel(x, y, world.getBackgroundColor());
//                 }

//                 newImage.setPixel(x, y, color);
//             }
//         }

//         double max_depth = 0.0;;
//         for (int y = 0; y < imageHeight; y++) {
//             for (int x = 0; x < imageWidth; x++) {
//                 double depth = depthBuffer.get(x, y);
//                 if (depth < std::numeric_limits<double>::max()) {
//                     max_depth = std::max(max_depth, depth);
//                 }
//             }
//         }

//         for (int y = 0; y < imageHeight; y++) {
//             for (int x = 0; x < imageWidth; x++) {
//                 double depth = depthBuffer.get(x, y);
//                 if (depth < std::numeric_limits<double>::max()) {
//                     newImage.setPixel(x, y, newImage.getPixel(x, y).applyDepthShading(depth, max_depth));
//                 }
//             }
//         }

//         return newImage;
// }


}
