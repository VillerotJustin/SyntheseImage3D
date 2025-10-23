//
// Created by villerot on 03/10/2025.
//

#include "Camera.h"
#include "../Math/Matrix.hpp"
#include <limits>
#include <cmath>

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

    Image Camera::renderScene2DColor(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes) const {
        Image image(imageWidth, imageHeight);

        if (shapes.size() == 0) {
            return image; // Return empty image if no shapes
        }

        Vector3D viewportLengthVec = viewport.getLengthVec();
        Vector3D viewportWidthVec = viewport.getWidthVec();

        // Scale by viewport dimensions
        viewportLengthVec = viewportLengthVec * viewport.getLength();
        viewportWidthVec = viewportWidthVec * viewport.getWidth();

        // For each pixel in the image, generate a ray through the corresponding point on the viewport
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                double u = (static_cast<double>(x) + 0.5) / static_cast<double>(imageWidth);
                double v = (static_cast<double>(y) + 0.5) / static_cast<double>(imageHeight);

                // Use Rectangle's parametric point method to avoid coordinate/ordering issues
                Vector3D pixelPosition = viewport.getPointAt(u, v);
                Ray ray = generateRay(pixelPosition);

                double closestDistance = std::numeric_limits<double>::infinity();
                RGBA_Color pixelColor(0, 0, 0, 1); // Default to black
                bool hitFound = false;

                for (size_t i = 0; i < shapes.size(); ++i) {
                    std::visit([&](auto&& shape) {
                        using T = std::decay_t<decltype(shape)>;
                        double intersectionDistance = std::numeric_limits<double>::infinity();
                        bool intersects = false;

                        if constexpr (std::is_same_v<T, Shape<Box>>) {
                            const Box* box = shape.getGeometry();
                            if (box && box->rayIntersect(ray)) {
                                intersects = true;
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

                        if (intersects && intersectionDistance < closestDistance) {
                            closestDistance = intersectionDistance;
                            hitFound = true;

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

                if (hitFound) {
                    image.setPixel(x, y, pixelColor);
                }
            }
        }
        return image;
    }

    Image Camera::renderScene2DDepth(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes) const {
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

        // Use a matrix of pointers to doubles for depth buffer
        math::Matrix<double> depthBuffer(imageWidth, imageHeight);
        
        // Initialize depth buffer with pointers to infinity values
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                depthBuffer(x, y) = new double(std::numeric_limits<double>::infinity());
            }
        }

        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                double u = (static_cast<double>(x)) / static_cast<double>(imageWidth);
                double v = (static_cast<double>(y)) / static_cast<double>(imageHeight);

                // Use Rectangle's parametric point method to avoid coordinate/ordering issues
                Vector3D pixelPosition = viewport.getPointAt(u, v);
                Ray ray = generateRay(pixelPosition);

                double closestDistance = std::numeric_limits<double>::infinity();
                RGBA_Color pixelColor(0, 0, 0, 1); // Default to black
                bool hitFound = false;

                for (size_t i = 0; i < shapes.size(); ++i) {
                    std::visit([&](auto&& shape) {
                        using T = std::decay_t<decltype(shape)>;

                        std::optional<double> distance = std::nullopt;
                        if (shape.getGeometry()) {
                            distance = shape.getGeometry()->rayIntersectDepth(ray);
                        }

                        if (distance && *distance < closestDistance) {
                            closestDistance = *distance;
                            hitFound = true;

                            pixelColor = shape.getColor() ? *shape.getColor() : RGBA_Color(0, 0, 0, 1); // Default to black if no color

                            if (pixelColor == RGBA_Color(0, 0, 0, 1)) {
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

                if (hitFound) {
                    // Store depth
                    *depthBuffer(x, y) = closestDistance;
                    // Store color
                    image.setPixel(x, y, pixelColor);
                }
            }
        }

        // Apply depth-based shading
        double max_depth = -1.0;

        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                double depth = *depthBuffer(x, y);
                if (depth > max_depth && depth < std::numeric_limits<double>::infinity()) {
                    max_depth = depth;
                }
            }
        }

        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                double depth = *depthBuffer(x, y);
                if (depth < std::numeric_limits<double>::infinity()) {
                    RGBA_Color pixelColor = *image.getPixel(x, y);
                    double intensity = std::max(0.0, 1.2 - (depth / max_depth));
                    pixelColor.setR(pixelColor.r() * intensity);
                    pixelColor.setG(pixelColor.g() * intensity);
                    pixelColor.setB(pixelColor.b() * intensity);
                    image.setPixel(x, y, pixelColor);
                }
            }
        }

        // Clean up depth buffer
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                delete depthBuffer(x, y);
            }
        }

        // TODO fix janky depth color

        return image;
    }

    Image Camera::renderScene3DColor(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes) const {
        // Check Image aspect ratio
        if (static_cast<double>(imageWidth) / static_cast<double>(imageHeight) != getViewportAspectRatio()) {
            throw std::invalid_argument("Image aspect ratio does not match camera viewport aspect ratio");
        }

        Image Image3D(imageWidth, imageHeight);

        if (shapes.size() == 0) {
            return Image3D; // Return empty image if no shapes
        }

        Vector3D viewportLengthVec = viewport.getLengthVec();
        Vector3D viewportWidthVec = viewport.getWidthVec();

        // Scale by viewport dimensions
        viewportLengthVec = viewportLengthVec * viewport.getLength();
        viewportWidthVec = viewportWidthVec * viewport.getWidth();

        Vector3D fovOrigin = getFOVOrigin();

        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                double u = (static_cast<double>(x)) / static_cast<double>(imageWidth);
                double v = (static_cast<double>(y)) / static_cast<double>(imageHeight);

                // Use Rectangle's parametric point method to avoid coordinate/ordering issues
                Vector3D pixelPosition = viewport.getPointAt(u, v);
                Ray ray(fovOrigin, (pixelPosition - fovOrigin).normal());

                double closestDistance = std::numeric_limits<double>::infinity();
                bool hitFound = false;
                RGBA_Color pixelColor(0, 0, 0, 1); // Default to black

                for (size_t i = 0; i < shapes.size(); ++i) {
                    std::visit([&](auto&& shape) {
                        using T = std::decay_t<decltype(shape)>;

                        std::optional<double> distance = std::nullopt;
                        if (shape.getGeometry()) {
                            distance = shape.getGeometry()->rayIntersectDepth(ray);
                        }

                        if (distance && *distance < closestDistance) {
                            closestDistance = *distance;
                            hitFound = true;

                            pixelColor = shape.getColor() ? *shape.getColor() : RGBA_Color(0, 0, 0, 1); // Default to black if no color

                            if (pixelColor == RGBA_Color(0, 0, 0, 1)) {
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
                
                // Store the depth and color for this pixel
                if (hitFound) {
                    Image3D.setPixel(x, y, pixelColor);
                }
            }
        }

        return Image3D;
    }
    
    Image Camera::renderScene3DDepth(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes) const {
        // Check Image aspect ratio
        double aspectRatio = static_cast<double>(imageWidth) / static_cast<double>(imageHeight);
        double precision = 1e-6;
        if (std::abs(aspectRatio - getViewportAspectRatio()) > precision) {
            throw std::invalid_argument("Image aspect ratio does not match camera viewport aspect ratio");
        }

        Image Image3D(imageWidth, imageHeight);

        if (shapes.size() == 0) {
            return Image3D; // Return empty image if no shapes
        }

        Vector3D viewportLengthVec = viewport.getLengthVec();
        Vector3D viewportWidthVec = viewport.getWidthVec();

        // Scale by viewport dimensions
        viewportLengthVec = viewportLengthVec * viewport.getLength();
        viewportWidthVec = viewportWidthVec * viewport.getWidth();

        Vector3D fovOrigin = getFOVOrigin();

        // Use a matrix of pointers to doubles for depth buffer
        math::Matrix<double> depthBuffer(imageWidth, imageHeight);
        
        // Initialize depth buffer with pointers to infinity values
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                depthBuffer(x, y) = new double(std::numeric_limits<double>::infinity());
            }
        }

        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                double u = (static_cast<double>(x)) / static_cast<double>(imageWidth);
                double v = (static_cast<double>(y)) / static_cast<double>(imageHeight);

                // Use Rectangle's parametric point method to avoid coordinate/ordering issues
                Vector3D pixelPosition = viewport.getPointAt(u, v);
                Ray ray(fovOrigin, (pixelPosition - fovOrigin).normal());

                double closestDistance = std::numeric_limits<double>::infinity();
                bool hitFound = false;
                RGBA_Color pixelColor(0, 0, 0, 1); // Default to black

                for (size_t i = 0; i < shapes.size(); ++i) {
                    std::visit([&](auto&& shape) {
                        using T = std::decay_t<decltype(shape)>;

                        std::optional<double> distance = std::nullopt;
                        if (shape.getGeometry()) {
                            distance = shape.getGeometry()->rayIntersectDepth(ray);
                        }

                        if (distance && *distance < closestDistance) {
                            closestDistance = *distance;
                            hitFound = true;

                            pixelColor = shape.getColor() ? *shape.getColor() : RGBA_Color(0, 0, 0, 1); // Default to black if no color

                            if (pixelColor == RGBA_Color(0, 0, 0, 1)) {
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
                
                // Store the depth and color for this pixel
                if (hitFound) {
                    *depthBuffer(x, y) = closestDistance;
                    Image3D.setPixel(x, y, pixelColor);
                }
            }
        }

        // Apply depth-based shading
        double max_depth = -1.0;

        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                double depth = *depthBuffer(x, y);
                if (depth > max_depth && depth < std::numeric_limits<double>::infinity()) {
                    max_depth = depth;
                }
            }
        }

        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                double depth = *depthBuffer(x, y);
                if (depth < std::numeric_limits<double>::infinity()) {
                    RGBA_Color pixelColor = *Image3D.getPixel(x, y);
                    double intensity = std::max(0.0, 1.2 - (depth / max_depth));
                    pixelColor.setR(pixelColor.r() * intensity);
                    pixelColor.setG(pixelColor.g() * intensity);
                    pixelColor.setB(pixelColor.b() * intensity);
                    Image3D.setPixel(x, y, pixelColor);
                }
            }
        }

        // Clean up depth buffer
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                delete depthBuffer(x, y);
            }
        }

        return Image3D;
    }
    
    Image Camera::renderScene3DLight(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes, math::Vector<Light> lights) const {
        Image Image3D(imageWidth, imageHeight);

        if (shapes.size() == 0 || lights.size() == 0) {
            return Image3D; // Return empty image if no shapes or lights
        }

        Vector3D viewportLengthVec = viewport.getLengthVec();
        Vector3D viewportWidthVec = viewport.getWidthVec();

        // Scale by viewport dimensions
        viewportLengthVec = viewportLengthVec * viewport.getLength();
        viewportWidthVec = viewportWidthVec * viewport.getWidth();

        Vector3D fovOrigin = getFOVOrigin();

        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                double u = (static_cast<double>(x)) / static_cast<double>(imageWidth);
                double v = (static_cast<double>(y)) / static_cast<double>(imageHeight);

                // Use Rectangle's parametric point method to avoid coordinate/ordering issues
                Vector3D pixelPosition = viewport.getPointAt(u, v);
                Ray ray(fovOrigin, (pixelPosition - fovOrigin).normal());

                double closestDistance = std::numeric_limits<double>::infinity();
                // Accumulated colored light for this pixel's hit (initialized to zero light)
                RGBA_Color accumulatedLight(0.0, 0.0, 0.0, 1.0);
                bool hitFound = false;
                RGBA_Color pixelColor(0, 0, 0, 1); // Default to black

                for (size_t i = 0; i < shapes.size(); ++i) {
                    std::visit([&](auto&& shape) {
                        using T = std::decay_t<decltype(shape)>;

                        std::optional<double> distance = std::nullopt;
                        if (shape.getGeometry()) {
                            distance = shape.getGeometry()->rayIntersectDepth(ray);
                        }

                        if (distance && *distance < closestDistance) {
                            closestDistance = *distance;
                            hitFound = true;

                            // Calculate light intensity at the hit point
                            Vector3D hitPoint = ray.getPointAt(*distance);
                            // reset accumulated light for this hit
                            accumulatedLight = RGBA_Color(0.0, 0.0, 0.0, 1.0);

                            // Calculate normal at hit point if possible
                            Vector3D normal = shape.getNormalAt(hitPoint);


                            // Calculate light intensity and color contribution from each light source
                            for (const Light* light : lights) {
                                Vector3D hitToLight = (light->getPosition() - hitPoint);
                                double distanceToLight = hitToLight.length();
                                Vector3D lightDir = hitToLight.normal();

                                // Offset the shadow ray origin slightly to avoid self-intersection
                                const double epsilon = 1e-4;
                                Ray lightRay(hitPoint + lightDir * epsilon, lightDir);

                                // Track transmission through potentially transparent occluders
                                double transmission = 1.0;

                                for (size_t j = 0; j < shapes.size(); ++j) {
                                    if (i == j) continue; // Skip self
                                    std::visit([&](auto&& otherShape) {
                                        if (otherShape.getGeometry()) {
                                            auto shadowDist = otherShape.getGeometry()->rayIntersectDepth(lightRay);
                                            // If intersection between hit point and light, attenuate by occluder's alpha
                                            if (shadowDist && *shadowDist < distanceToLight) {
                                                const RGBA_Color* occColor = otherShape.getColor();
                                                double occAlpha = occColor ? occColor->a() : 1.0;
                                                // If fully opaque, block completely
                                                if (occAlpha >= 1.0 - 1e-12) {
                                                    transmission = 0.0;
                                                } else {
                                                    // Multiply transmission by (1 - occluder opacity)
                                                    transmission *= (1.0 - occAlpha);
                                                }
                                            }
                                        }
                                    }, *shapes[j]);

                                    if (transmission <= 1e-12) break; // fully blocked
                                }

                                if (transmission > 1e-12) {
                                    // Lambertian term: dot(normal, lightDir)
                                    double nDotL = std::max(0.0, normal.dot(lightDir));

                                    // Light color scaled by intensity, geometric attenuation (optional inverse-square)
                                    RGBA_Color lightCol = light->getColor() * light->getIntensity();

                                    // Optionally include distance attenuation (simple inverse square)
                                    double distanceAtten = 1.0 / (1.0 + 0.03 * distanceToLight * distanceToLight);

                                    // Contribution from this light factoring transmission, nDotL and distance attenuation
                                    RGBA_Color contrib = lightCol * (transmission * nDotL * distanceAtten);
                                    accumulatedLight = accumulatedLight + contrib;
                                }
                            }

                            // Ensure some ambient minimum lighting
                            // accumulatedLight components may exceed 1.0, clamp later when applying to pixel

                            pixelColor = shape.getColor() ? *shape.getColor() : RGBA_Color(0, 0, 0, 1); // Default to black if no color

                            if (pixelColor == RGBA_Color(0, 0, 0, 1)) {
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
                
                // Store the depth and color for this pixel
                if (hitFound) {
                    // accumulatedLight was computed per hit; if zero use small ambient
                    RGBA_Color ambient(0.05, 0.05, 0.05, 1.0);
                    RGBA_Color lightEffect = (accumulatedLight + ambient).clamp();

                    // Modulate surface color by light color (component-wise)
                    RGBA_Color litSurface = pixelColor * lightEffect;

                    // Handle alpha of the surface: composite over black background
                    double surfAlpha = pixelColor.a();
                    RGBA_Color finalColor;
                    if (surfAlpha >= 1.0 - 1e-12) {
                        finalColor = litSurface.clamp();
                    } else {
                        // Alpha blend over black background (or could read existing background)
                        // premultiplied approach: result = src + (1-src.a)*dst ; dst is black so simplified
                        finalColor = RGBA_Color(
                            litSurface.r() * surfAlpha,
                            litSurface.g() * surfAlpha,
                            litSurface.b() * surfAlpha,
                            surfAlpha
                        ).clamp();
                    }

                    Image3D.setPixel(x, y, finalColor);
                }
            }
        }

        return Image3D;
    }

}
