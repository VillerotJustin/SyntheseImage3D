//
// Created by villerot on 03/10/2025.
//

#include "Camera.h"
#include "../Math/Matrix.hpp"
#include "../Math/Vector.hpp"
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

                // Collect all hits along the view ray and sort them front-to-back
                struct Hit { double t; size_t idx; };
                math::Vector<Hit> hits;

                for (size_t i = 0; i < shapes.size(); ++i) {
                    std::visit([&](auto&& shape) {
                        if (shape.getGeometry()) {
                            if (auto d = shape.getGeometry()->rayIntersectDepth(ray)) {
                                // only accept hits in front of the origin
                                if (*d > 1e-9) {
                                    hits.append(new Hit{*d, i});
                                }
                            }
                        }
                    }, *shapes[i]);
                }

                if (!hits.empty()) {
                    std::sort(hits.begin(), hits.end(), [](const Hit *a, const Hit *b){ return a->t < b->t; });

                    // Front-to-back compositing using remaining transmittance
                    double remaining = 1.0;
                    double epsRemaining = 1e-6;
                    // Accumulate premultiplied color
                    double accR = 0.0, accG = 0.0, accB = 0.0;
                    double accA = 0.0;

                    for (const Hit *h : hits) {
                        if (remaining <= epsRemaining) break; // fully opaque already

                        // Access the shape
                        size_t i = h->idx;
                        std::visit([&](auto&& shape) {
                            using T = std::decay_t<decltype(shape)>;

                            // Compute lighting at this hit
                            Vector3D hitPoint = ray.getPointAt(h->t);
                            Vector3D normal = shape.getNormalAt(hitPoint);

                            RGBA_Color accumulatedLight(0.0, 0.0, 0.0, 1.0);

                            for (const Light* light : lights) {
                                Vector3D hitToLight = (light->getPosition() - hitPoint);
                                double distanceToLight = hitToLight.length();
                                Vector3D lightDir = hitToLight.normal();

                                const double epsilon = 1e-4;
                                Ray lightRay(hitPoint + lightDir * epsilon, lightDir);

                                double transmission = 1.0;

                                for (size_t j = 0; j < shapes.size(); ++j) {
                                    if (i == j) continue;
                                    std::visit([&](auto&& otherShape) {
                                        if (otherShape.getGeometry()) {
                                            auto shadowDist = otherShape.getGeometry()->rayIntersectDepth(lightRay);
                                            if (shadowDist && *shadowDist < distanceToLight) {
                                                const RGBA_Color* occColor = otherShape.getColor();
                                                double occAlpha = occColor ? occColor->a() : 1.0;
                                                if (occAlpha >= 1.0 - 1e-12) {
                                                    transmission = 0.0;
                                                } else {
                                                    transmission *= (1.0 - occAlpha);
                                                }
                                            }
                                        }
                                    }, *shapes[j]);

                                    if (transmission <= 1e-12) break;
                                }

                                if (transmission > 1e-12) {
                                    double nDotL = std::max(0.0, normal.dot(lightDir));
                                    RGBA_Color lightCol = light->getColor() * light->getIntensity();
                                    double distanceAtten = 1.0 / (1.0 + 0.03 * distanceToLight * distanceToLight);
                                    RGBA_Color contrib = lightCol * (transmission * nDotL * distanceAtten);
                                    accumulatedLight = accumulatedLight + contrib;
                                }
                            }

                            // Ambient
                            RGBA_Color ambient(0.05, 0.05, 0.05, 1.0);
                            RGBA_Color lightEffect = (accumulatedLight + ambient).clamp();

                            RGBA_Color surfColor = shape.getColor() ? *shape.getColor() : RGBA_Color(0,0,0,1);
                            if (surfColor == RGBA_Color(0,0,0,1)) {
                                if constexpr (std::is_same_v<T, Shape<Box>>) {
                                    surfColor = RGBA_Color(1,0,0,1);
                                } else if constexpr (std::is_same_v<T, Shape<Circle>>) {
                                    surfColor = RGBA_Color(0,1,0,1);
                                } else if constexpr (std::is_same_v<T, Shape<Plane>>) {
                                    surfColor = RGBA_Color(0.5,0.5,0.5,1);
                                } else if constexpr (std::is_same_v<T, Shape<Rectangle>>) {
                                    surfColor = RGBA_Color(0,0,1,1);
                                } else if constexpr (std::is_same_v<T, Shape<Sphere>>) {
                                    surfColor = RGBA_Color(1,1,1,1);
                                }
                            }

                            RGBA_Color litSurface = surfColor * lightEffect; // component-wise

                            double srcA = surfColor.a();
                            // premultiplied source color
                            double srcR = litSurface.r() * srcA;
                            double srcG = litSurface.g() * srcA;
                            double srcB = litSurface.b() * srcA;

                            // accumulate front-to-back
                            accR += srcR * remaining;
                            accG += srcG * remaining;
                            accB += srcB * remaining;
                            accA += srcA * remaining;

                            remaining *= (1.0 - srcA);
                        }, *shapes[i]);
                    }

                    // Build final color
                    double finalA = 1.0 - remaining;
                    RGBA_Color finalColor(accR, accG, accB, finalA);
                    Image3D.setPixel(x, y, finalColor.clamp());
                }
            }
        }

        return Image3D;
    }

    Image Camera::renderScene3DLightAntiAliasing(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes, math::Vector<Light> lights, size_t samplesPerPixel) const {
        if (samplesPerPixel == 0 || samplesPerPixel % 4 != 0) {
            throw std::invalid_argument("samplesPerPixel must be a multiple of 4 not zero");
        }

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

        size_t antiAlias_imageHeight = imageHeight * samplesPerPixel / 2;
        size_t antiAlias_imageWidth = imageWidth * samplesPerPixel / 2;

        Image Image3D_antiAliased(antiAlias_imageWidth, antiAlias_imageHeight);

        for (size_t y = 0; y < antiAlias_imageHeight; ++y) {
            for (size_t x = 0; x < antiAlias_imageWidth; ++x) {
                double u = (static_cast<double>(x)) / static_cast<double>(antiAlias_imageWidth);
                double v = (static_cast<double>(y)) / static_cast<double>(antiAlias_imageHeight);

                // Use Rectangle's parametric point method to avoid coordinate/ordering issues
                Vector3D pixelPosition = viewport.getPointAt(u, v);
                Ray ray(fovOrigin, (pixelPosition - fovOrigin).normal());

                // Collect all hits along the view ray and sort them front-to-back
                struct Hit { double t; size_t idx; };
                math::Vector<Hit> hits;

                for (size_t i = 0; i < shapes.size(); ++i) {
                    std::visit([&](auto&& shape) {
                        if (shape.getGeometry()) {
                            if (auto d = shape.getGeometry()->rayIntersectDepth(ray)) {
                                // only accept hits in front of the origin
                                if (*d > 1e-9) {
                                    hits.append(new Hit{*d, i});
                                }
                            }
                        }
                    }, *shapes[i]);
                }

                if (!hits.empty()) {
                    std::sort(hits.begin(), hits.end(), [](const Hit *a, const Hit *b){ return a->t < b->t; });

                    // Front-to-back compositing using remaining transmittance
                    double remaining = 1.0;
                    double epsRemaining = 1e-6;
                    // Accumulate premultiplied color
                    double accR = 0.0, accG = 0.0, accB = 0.0;
                    double accA = 0.0;

                    for (const Hit *h : hits) {
                        if (remaining <= epsRemaining) break; // fully opaque already

                        // Access the shape
                        size_t i = h->idx;
                        std::visit([&](auto&& shape) {
                            using T = std::decay_t<decltype(shape)>;

                            // Compute lighting at this hit
                            Vector3D hitPoint = ray.getPointAt(h->t);
                            Vector3D normal = shape.getNormalAt(hitPoint);

                            RGBA_Color accumulatedLight(0.0, 0.0, 0.0, 1.0);

                            for (const Light* light : lights) {
                                Vector3D hitToLight = (light->getPosition() - hitPoint);
                                double distanceToLight = hitToLight.length();
                                Vector3D lightDir = hitToLight.normal();

                                const double epsilon = 1e-4;
                                Ray lightRay(hitPoint + lightDir * epsilon, lightDir);

                                double transmission = 1.0;

                                for (size_t j = 0; j < shapes.size(); ++j) {
                                    if (i == j) continue;
                                    std::visit([&](auto&& otherShape) {
                                        if (otherShape.getGeometry()) {
                                            auto shadowDist = otherShape.getGeometry()->rayIntersectDepth(lightRay);
                                            if (shadowDist && *shadowDist < distanceToLight) {
                                                const RGBA_Color* occColor = otherShape.getColor();
                                                double occAlpha = occColor ? occColor->a() : 1.0;
                                                if (occAlpha >= 1.0 - 1e-12) {
                                                    transmission = 0.0;
                                                } else {
                                                    transmission *= (1.0 - occAlpha);
                                                }
                                            }
                                        }
                                    }, *shapes[j]);

                                    if (transmission <= 1e-12) break;
                                }

                                if (transmission > 1e-12) {
                                    double nDotL = std::max(0.0, normal.dot(lightDir));
                                    RGBA_Color lightCol = light->getColor() * light->getIntensity();
                                    double distanceAtten = 1.0 / (1.0 + 0.03 * distanceToLight * distanceToLight);
                                    RGBA_Color contrib = lightCol * (transmission * nDotL * distanceAtten);
                                    accumulatedLight = accumulatedLight + contrib;
                                }
                            }

                            // Ambient
                            RGBA_Color ambient(0.05, 0.05, 0.05, 1.0);
                            RGBA_Color lightEffect = (accumulatedLight + ambient).clamp();

                            RGBA_Color surfColor = shape.getColor() ? *shape.getColor() : RGBA_Color(0,0,0,1);
                            if (surfColor == RGBA_Color(0,0,0,1)) {
                                if constexpr (std::is_same_v<T, Shape<Box>>) {
                                    surfColor = RGBA_Color(1,0,0,1);
                                } else if constexpr (std::is_same_v<T, Shape<Circle>>) {
                                    surfColor = RGBA_Color(0,1,0,1);
                                } else if constexpr (std::is_same_v<T, Shape<Plane>>) {
                                    surfColor = RGBA_Color(0.5,0.5,0.5,1);
                                } else if constexpr (std::is_same_v<T, Shape<Rectangle>>) {
                                    surfColor = RGBA_Color(0,0,1,1);
                                } else if constexpr (std::is_same_v<T, Shape<Sphere>>) {
                                    surfColor = RGBA_Color(1,1,1,1);
                                }
                            }

                            RGBA_Color litSurface = surfColor * lightEffect; // component-wise

                            double srcA = surfColor.a();
                            // premultiplied source color
                            double srcR = litSurface.r() * srcA;
                            double srcG = litSurface.g() * srcA;
                            double srcB = litSurface.b() * srcA;

                            // accumulate front-to-back
                            accR += srcR * remaining;
                            accG += srcG * remaining;
                            accB += srcB * remaining;
                            accA += srcA * remaining;

                            remaining *= (1.0 - srcA);
                        }, *shapes[i]);
                    }

                    // Build final color
                    double finalA = 1.0 - remaining;
                    RGBA_Color finalColor(accR, accG, accB, finalA);
                    Image3D_antiAliased.setPixel(x, y, finalColor.clamp());
                }
            }
        }

        // Downsample anti-aliased image to final image
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                double accR = 0.0, accG = 0.0, accB = 0.0, accA = 0.0;
                for (size_t ay = 0; ay < samplesPerPixel / 2; ++ay) {
                    for (size_t ax = 0; ax < samplesPerPixel / 2; ++ax) {
                        size_t sampleX = x * (samplesPerPixel / 2) + ax;
                        size_t sampleY = y * (samplesPerPixel / 2) + ay;
                        RGBA_Color sampleColor = *Image3D_antiAliased.getPixel(sampleX, sampleY);
                        accR += sampleColor.r();
                        accG += sampleColor.g();
                        accB += sampleColor.b();
                        accA += sampleColor.a();
                    }
                }

                // TODO fix light bleeding (overexposure) by using a better tonemapping operator

                double numSamples = static_cast<double>(samplesPerPixel);
                RGBA_Color finalColor(accR / numSamples, accG / numSamples, accB / numSamples, accA / numSamples);
                Image3D.setPixel(x, y, finalColor.clamp());
            }
        }

        return Image3D;
    }

}
