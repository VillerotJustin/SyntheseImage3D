//
// Created by villerot on 03/10/2025.
//

#include "Camera.h"
#include <limits>

namespace rendering {

    Camera::Camera(const Vector3D& position, double viewportWidth, double viewportLength, const Vector3D& direction) 
        : position(position), viewportWidth(viewportWidth), viewportLength(viewportLength), direction(direction.normal()),
          viewport(position, viewportLength, viewportWidth, direction.normal()) {
        // Constructor initializes all members including viewport
    }

    const Rectangle& Camera::getViewport() const {
        return viewport;
    }

    void Camera::setViewport(const Rectangle& viewport) {
        this->viewport = viewport;
    }

    void Camera::updateViewport() {
        // Create a viewport rectangle centered at the camera position
        // The viewport is positioned at the camera position facing in the camera direction
        viewport = Rectangle(position, viewportLength, viewportWidth, direction);
    }

    const Vector3D& Camera::getPosition() const {
        return position;
    }

    void Camera::setPosition(const Vector3D& position) {
        this->position = position;
        updateViewport();
    }

    const Vector3D& Camera::getDirection() const {
        return direction;
    }

    void Camera::setDirection(const Vector3D& direction) {
        this->direction = direction.normal();
        updateViewport();
    }

    double Camera::getViewportWidth() const {
        return viewportWidth;
    }

    void Camera::setViewportWidth(double width) {
        this->viewportWidth = width;
        updateViewport();
    }

    double Camera::getViewportLength() const {
        return viewportLength;
    }

    void Camera::setViewportLength(double length) {
        this->viewportLength = length;
        updateViewport();
    }

    void Camera::rotate(Quaternion rotation) {
        // Apply rotation to the camera's direction vector
        direction = rotation * direction;
        direction = direction.normal(); // Ensure it stays normalized
        
        // Update the viewport with the new direction
        updateViewport();
    }

    void Camera::translate(const Vector3D& translation) {
        // Update camera position
        position = position + translation;
        
        // Update the viewport with the new position
        updateViewport();
    }

    Ray Camera::generateRay(const Vector3D& pointOnViewport) const {
        // Get the closest point on the viewport plane to the given point
        Vector3D closestPoint = viewport.closestPointOnRectangle(pointOnViewport);
        
        // Create and return the ray
        return Ray(closestPoint, viewport.getNormal());
    }

    // TODO fix later shape overlap issue
    Image Camera::renderScene2DColor(int imageWidth, int imageHeight, math::Vector<ShapeVariant> shapes) const {
        Image image(imageWidth, imageHeight);

        if (shapes.size() == 0) {
            return image; // Return empty image if no shapes
        }

        Vector3D viewportLengthVec, viewportWidthVec;

        // Use Rectangle's robust basis vector generation
        viewport.generateBasisVectors(viewportLengthVec, viewportWidthVec);
        
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
                            if (sphere && sphere->rayIntersect(ray)) {
                                intersects = true;
                                Vector3D shapeCenter = sphere->getCenter();
                                intersectionDistance = (ray.getOrigin() - shapeCenter).length();
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

    Image Camera::renderScene2DDepth(int imageWidth, int imageHeight, math::Vector<ShapeVariant> shapes) const {
        Image image(imageWidth, imageHeight);

        if (shapes.size() == 0) {
            return image; // Return empty image if no shapes
        }

        Vector3D viewportLengthVec, viewportWidthVec;

        // Use Rectangle's robust basis vector generation
        viewport.generateBasisVectors(viewportLengthVec, viewportWidthVec);
        
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
                        
                        std::optional<double> distance = std::nullopt;
                        if (shape.getGeometry()) {
                            if constexpr (std::is_same_v<T, Shape<Sphere>>) {
                                distance = shape.getGeometry()->rayIntersectDepth(ray);
                            } else {
                                distance = shape.getGeometry()->rayIntersectDepth(ray);
                            }
                        }
                        
                        // Update pixel color if this intersection is closer
                        if (distance && *distance < closestDistance) {
                            closestDistance = *distance;
                            hitFound = true;

                            pixelColor = shape.getColor() ? *shape.getColor() : RGBA_Color(0, 0, 0, 1); // Default to black if no color

                            if (pixelColor != RGBA_Color(0, 0, 0, 1)) {
                                // Scale color intensity based on depth (closer = brighter)
                                double intensity = std::max(0.0, 1.0 - (closestDistance / 100.0)); // Adjust scaling factor as needed
                                pixelColor.setR(pixelColor.r() * intensity);
                                pixelColor.setG(pixelColor.g() * intensity);
                                pixelColor.setB(pixelColor.b() * intensity);
                            } else {
                                // Set appropriate color based on shape type
                                if constexpr (std::is_same_v<T, Shape<Box>>) {
                                    pixelColor = RGBA_Color(1, 0, 0, 1); // Red
                                } else if constexpr (std::is_same_v<T, Shape<Circle>>) {
                                    pixelColor = RGBA_Color(0, 1, 0, 1); // Green
                                } else if constexpr (std::is_same_v<T, Shape<Plane>>) {
                                    pixelColor = RGBA_Color(0.5, 0.5, 0.5, 1); // Gray
                                } else if constexpr (std::is_same_v<T, Shape<Rectangle>>) {
                                    pixelColor = RGBA_Color(0, 0, 1, 1); // Blue
                                } else if constexpr (std::is_same_v<T, Shape<Sphere>>) {
                                    pixelColor = RGBA_Color(1, 1, 1, 1); // White
                                }
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

}
