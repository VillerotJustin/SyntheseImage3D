//
// Created by villerot on 13/11/2025.
//

#include "CameraHelper.h"
#include "Camera.h"
#include <omp.h>

namespace rendering {

    Ray Camera::generateRayForPixel(size_t pixelX, size_t pixelY, size_t imageWidth, size_t imageHeight, bool is3D) const {
        double u = (static_cast<double>(pixelX)) / static_cast<double>(imageWidth);
        double v = (static_cast<double>(pixelY)) / static_cast<double>(imageHeight);
        Vector3D pixelPosition = viewport.getPointAt(u, v);

        if (is3D) {
            // For 3D, adjust pixel position based on FOV origin
            Vector3D fovOrigin = getFOVOrigin();
            Vector3D direction = (pixelPosition - fovOrigin).normal();
            return Ray(fovOrigin, direction);
        }
        return generateRay(pixelPosition);
    }

    Ray Camera::generateRandomRayForPixel(size_t pixelX, size_t pixelY, size_t imageWidth, size_t imageHeight, bool is3D) const {
        // Get pixel coordinates with random sub-pixel offset for anti-aliasing        
        double u = (static_cast<double>(pixelX)) / static_cast<double>(imageWidth);
        double v = (static_cast<double>(pixelY)) / static_cast<double>(imageHeight);
        double width_margin = 1.0 / static_cast<double>(imageWidth);
        double height_margin = 1.0 / static_cast<double>(imageHeight);
        u = u + (width_margin/2) * math::randomDouble(-1.0, 1.0);
        v = v + (height_margin/2) * math::randomDouble(-1.0, 1.0);

        Vector3D pixelPosition = viewport.getPointAt(u, v);

        if (is3D) {
            // For 3D, adjust pixel position based on FOV origin
            Vector3D fovOrigin = getFOVOrigin();
            Vector3D direction = (pixelPosition - fovOrigin).normal();
            return Ray(fovOrigin, direction);
        }
        return generateRay(pixelPosition);
    }


    void applyDepthShadingToPixel(Image& image, size_t x, size_t y, double depth, double max_depth) {
        double intensity = std::max(0.0, 1.2 - (depth / max_depth));
        image.setPixel(x, y, image.getPixel(x, y)*intensity);
    }

    void applyDepthShadingToImage(Image& image, const math::Matrix<double>& depthBuffer, double max_depth) {
        size_t width = image.getWidth();
        size_t height = image.getHeight();

        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                double depth = depthBuffer(x, y);
                if (depth < std::numeric_limits<double>::infinity()) {
                    applyDepthShadingToPixel(image, x, y, depth, max_depth);
                }
            }
        }
    }

    void shapeProcessSimple(Ray& ray, math::Vector<rendering::Camera::ShapeVariant>& shapes, RGBA_Color& pixelColor, double& closestDistance, bool& hitFound) {
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

                    pixelColor = shape.getMaterial() ? shape.getMaterial()->getAlbedo() : RGBA_Color(1, 0, 1, 1); // Default to black if no color

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

            }, shapes[i]);
        }
    }

    Image SSAADownScaling(Image& image_in, size_t samplesPerPixel) {
        size_t imageWidth = image_in.getWidth() / (samplesPerPixel / 2);
        size_t imageHeight = image_in.getHeight() / (samplesPerPixel / 2);
        Image image_out(imageWidth, imageHeight);

        const double exposure = 0.5;  // Adjustable exposure control
        const double gamma = 2.2;     // Standard gamma correction value
        
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                double accR = 0.0, accG = 0.0, accB = 0.0, accA = 0.0;
                
                for (size_t ay = 0; ay < samplesPerPixel / 2; ++ay) {
                    for (size_t ax = 0; ax < samplesPerPixel / 2; ++ax) {
                        size_t sampleX = x * (samplesPerPixel / 2) + ax;
                        size_t sampleY = y * (samplesPerPixel / 2) + ay;
                        RGBA_Color sampleColor = image_in.getPixel(sampleX, sampleY);

                        // Convert from sRGB to linear space
                        double r = std::pow(sampleColor.r(), gamma);
                        double g = std::pow(sampleColor.g(), gamma);
                        double b = std::pow(sampleColor.b(), gamma);

                        accR += r;
                        accG += g;
                        accB += b;
                        accA += sampleColor.a();
                    }
                }
                double numSamples = static_cast<double>(samplesPerPixel);

                // Average in Linear space
                double avgR = accR / numSamples;
                double avgG = accG / numSamples;
                double avgB = accB / numSamples;
                double avgA = accA / numSamples;

                // Apply exposure adjustment
                avgR = 1.0 - std::exp(-avgR * exposure);
                avgG = 1.0 - std::exp(-avgG * exposure);
                avgB = 1.0 - std::exp(-avgB * exposure);


                // Convert back to sRGB space
                double finalR = std::pow(avgR, 1.0 / gamma);
                double finalG = std::pow(avgG, 1.0 / gamma);
                double finalB = std::pow(avgB, 1.0 / gamma);

                RGBA_Color finalColor(finalR, finalG, finalB, avgA);
                image_out.setPixel(x, y, finalColor.clamp());
            }
        }
        return image_out;
    }

} // namespace rendering