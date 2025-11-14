//
// Created by villerot on 13/11/2025.
//

#include "Camera.h"
#include "CameraHelper.h"
#include <omp.h>
#include <stdexcept>
#include <limits>

namespace rendering {

    Image Camera::renderScene2DColor(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes) const {
        Image image(imageWidth, imageHeight);

        if (shapes.size() == 0) {
            return image; // Return empty image if no shapes
        }

        // For each pixel in the image, generate a ray through the corresponding point on the viewport
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                Ray ray = generateRayForPixel(x, y, imageWidth, imageHeight, false);

                double closestDistance = std::numeric_limits<double>::infinity();
                RGBA_Color pixelColor(0, 0, 0, 1); // Default to black
                bool hitFound = false;

                shapeProcessSimple(ray, shapes, pixelColor, closestDistance, hitFound);

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

        // Use a matrix of pointers to doubles for depth buffer
        math::Matrix<double> depthBuffer(imageWidth, imageHeight, std::numeric_limits<double>::infinity());

        // Apply depth-based shading
        double max_depth = -1.0;

        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                Ray ray = generateRayForPixel(x, y, imageWidth, imageHeight, false);
                
                double closestDistance = std::numeric_limits<double>::infinity();
                RGBA_Color pixelColor(0, 0, 0, 1); // Default to black
                bool hitFound = false;

                shapeProcessSimple(ray, shapes, pixelColor, closestDistance, hitFound);

                if (hitFound) {
                    // set max depth
                    if (closestDistance > max_depth) {
                        max_depth = closestDistance;
                    }
                    // Store depth
                    depthBuffer(x, y) = closestDistance;
                    // Store color
                    image.setPixel(x, y, pixelColor);
                }
            }
        }

        applyDepthShadingToImage(image, depthBuffer, max_depth);

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

        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                Ray ray = generateRayForPixel(x, y, imageWidth, imageHeight, true);

                double closestDistance = std::numeric_limits<double>::infinity();
                bool hitFound = false;
                RGBA_Color pixelColor(0, 0, 0, 1); // Default to black

                shapeProcessSimple(ray, shapes, pixelColor, closestDistance, hitFound);
                
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

        // Use a matrix of pointers to doubles for depth buffer
        math::Matrix<double> depthBuffer(imageWidth, imageHeight, std::numeric_limits<double>::infinity());

        // Apply depth-based shading
        double max_depth = -1.0;

        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                Ray ray = generateRayForPixel(x, y, imageWidth, imageHeight, true);

                double closestDistance = std::numeric_limits<double>::infinity();
                bool hitFound = false;
                RGBA_Color pixelColor(0, 0, 0, 1); // Default to black

                shapeProcessSimple(ray, shapes, pixelColor, closestDistance, hitFound);
                
                // Store the depth and color for this pixel
                if (hitFound) {
                    // set max depth
                    if (closestDistance > max_depth) {
                        max_depth = closestDistance;
                    }
                    depthBuffer(x, y) = closestDistance;
                    Image3D.setPixel(x, y, pixelColor);
                }
            }
        }

        applyDepthShadingToImage(Image3D, depthBuffer, max_depth);

        return Image3D;
    }

    Image Camera::renderScene3DLight(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes, math::Vector<Light> lights) const {
        Image Image3D(imageWidth, imageHeight);

        if (shapes.size() == 0 || lights.size() == 0) {
            return Image3D; // Return empty image if no shapes or lights
        }

        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                Ray ray = generateRayForPixel(x, y, imageWidth, imageHeight, true);

                math::Vector<Hit> hits;

                // #pragma omp parallel for schedule(dynamic)
                for (size_t i = 0; i < shapes.size(); ++i) {
                    std::visit([&](auto&& shape) {
                        if (shape.getGeometry()) {
                            if (auto d = shape.getGeometry()->rayIntersectDepth(ray)) {
                                // only accept hits in front of the origin
                                if (*d > 1e-9) {
                                    hits.append(Hit{*d, i});
                                }
                            }
                        }
                    }, shapes[i]);
                }

                if (!hits.empty()) {
                    RGBA_Color finalColor = Camera::processRayHitOld(hits, ray, shapes, lights);
                    Image3D.setPixel(x, y, finalColor.clamp());
                }
            }
        }

        return Image3D;
    }

    Image Camera::renderScene3DLight_MSAA(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes, math::Vector<Light> lights, size_t samplesPerPixel) const {
        Image Image3D(imageWidth, imageHeight);

        if (shapes.size() == 0 || lights.size() == 0) {
            return Image3D; // Return empty image if no shapes or lights
        }

        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                math::Vector<RGBA_Color> sampleColors;
                #pragma omp parallel for schedule(dynamic)
                for (size_t sample_number = 0; sample_number < samplesPerPixel; ++sample_number) {
                    Ray ray = generateRandomRayForPixel(x, y, imageWidth, imageHeight, true);

                    // Collect all hits along the view ray and sort them front-to-back
                    math::Vector<Hit> hits;

                    // #pragma omp parallel for schedule(dynamic)
                    for (size_t i = 0; i < shapes.size(); ++i) {
                        std::visit([&](auto&& shape) {
                            if (shape.getGeometry()) {
                                if (auto d = shape.getGeometry()->rayIntersectDepth(ray)) {
                                    // only accept hits in front of the origin
                                    if (*d > 1e-9) {
                                        hits.append(Hit{*d, i});
                                    }
                                }
                            }
                        }, shapes[i]);
                    }

                    if (!hits.empty()) {
                        RGBA_Color color = Camera::processRayHitOld(hits, ray, shapes, lights);
                        sampleColors.append(color);
                    }                
                }

                // Average samples for this pixel
                if (!sampleColors.empty()) {
                    double accR = 0.0, accG = 0.0, accB = 0.0, accA = 0.0;
                    size_t numColors = sampleColors.size();

                    // Move accumulation outside the parallel region
                    accR = sampleColors[0].r();
                    accG = sampleColors[0].g();
                    accB = sampleColors[0].b();
                    accA = sampleColors[0].a();

                    for (size_t i = 1; i < numColors; i++) {
                        accR += sampleColors[i].r();
                        accG += sampleColors[i].g();
                        accB += sampleColors[i].b();
                        accA += sampleColors[i].a();
                    }

                    double numSamples = static_cast<double>(numColors);
                    RGBA_Color finalColor(accR / numSamples, accG / numSamples, accB / numSamples, accA / numSamples);
                    Image3D.setPixel(x, y, finalColor.clamp());
                }
            }
        }

        return Image3D;
    }

    Image Camera::renderScene3DLight_AA(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes, math::Vector<Light> lights, size_t samplesPerPixel, AntiAliasingMethod method) const {
        if (samplesPerPixel == 0 || samplesPerPixel % 4 != 0) {
            throw std::invalid_argument("samplesPerPixel must be a multiple of 4 not zero");
        }

        Image Image3D(imageWidth, imageHeight);

        if (shapes.size() == 0 || lights.size() == 0) {
            return Image3D; // Return empty image if no shapes or lights
        }

        switch (method)
        {
            case Camera::AntiAliasingMethod::NONE: {
                return renderScene3DLight(imageWidth, imageHeight, shapes, lights);
            }
            case Camera::AntiAliasingMethod::MSAA: {
                // Multi-Sample Anti-Aliasing
                return renderScene3DLight_MSAA(imageWidth, imageHeight, shapes, lights, samplesPerPixel);
            }
            case Camera::AntiAliasingMethod::SSAA: {
                // Super-Sample Anti-Aliasing
                size_t antiAlias_imageHeight = imageHeight * samplesPerPixel / 2;
                size_t antiAlias_imageWidth = imageWidth * samplesPerPixel / 2;

                Image Image3D_antiAliased = renderScene3DLight(antiAlias_imageWidth, antiAlias_imageHeight, shapes, lights);
                
                // Downsample anti-aliased image to final image
                return SSAADownScaling(Image3D_antiAliased, samplesPerPixel);
            }
            case Camera::AntiAliasingMethod::FXAA: {
                // Fast Approximate Anti-Aliasing
                throw std::invalid_argument("FXAA not implemented yet");
            }
            default: {
                throw std::invalid_argument("Unknown AntiAliasingMethod");
            }
        }
    }

    Image Camera::renderScene3DLight_Advanced(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes, math::Vector<Light> lights) const {
        Image Image3D(imageWidth, imageHeight);

        if (shapes.size() == 0 || lights.size() == 0) {
            return Image3D; // Return empty image if no shapes or lights
        }

        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                Ray ray = generateRayForPixel(x, y, imageWidth, imageHeight, true);

                Hit hit;
                double closestDistance = std::numeric_limits<double>::infinity();

                // #pragma omp parallel for schedule(dynamic)
                for (size_t i = 0; i < shapes.size(); ++i) {
                    std::visit([&](auto&& shape) {
                        if (shape.getGeometry()) {
                            if (auto d = shape.getGeometry()->rayIntersectDepth(ray, closestDistance)) {
                                // only accept hits in front of the origin
                                if (*d > 1e-9) {
                                    hit = Hit{*d, i};
                                    closestDistance = *d;
                                }
                            }
                        }
                    }, shapes[i]);
                }

                if (closestDistance < std::numeric_limits<double>::infinity()) {
                    RGBA_Color finalColor = *Camera::processRayHitAdvanced(hit, ray, shapes, lights);
                    Image3D.setPixel(x, y, finalColor.clamp());
                }
            }
        }

        return Image3D;
    }

    Image Camera::renderScene3DLight_Advanced_MSAA(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes, math::Vector<Light> lights, size_t samplesPerPixel) const {
        Image Image3D(imageWidth, imageHeight);

        if (shapes.size() == 0 || lights.size() == 0) {
            return Image3D; // Return empty image if no shapes or lights
        }

        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (size_t y = 0; y < imageHeight; ++y) {
            for (size_t x = 0; x < imageWidth; ++x) {
                math::Vector<RGBA_Color> sampleColors;

                #pragma omp parallel for schedule(dynamic)
                for (size_t sample_number = 0; sample_number < samplesPerPixel; ++sample_number) {
                    Ray ray = generateRandomRayForPixel(x, y, imageWidth, imageHeight, true);

                    Hit hit;
                    double closestDistance = std::numeric_limits<double>::infinity();

                    // #pragma omp parallel for schedule(dynamic)
                    for (size_t i = 0; i < shapes.size(); ++i) {
                        std::visit([&](auto&& shape) {
                            if (shape.getGeometry()) {
                                if (auto d = shape.getGeometry()->rayIntersectDepth(ray, closestDistance)) {
                                    // only accept hits in front of the origin
                                    if (*d > 1e-9) {
                                        hit = Hit{*d, i};
                                        closestDistance = *d;
                                    }
                                }
                            }
                        }, shapes[i]);
                    }

                    if (closestDistance < std::numeric_limits<double>::infinity()) {
                        sampleColors.append(*Camera::processRayHitAdvanced(hit, ray, shapes, lights));
                    }              
                }

                // Average samples for this pixel
                if (!sampleColors.empty()) {
                    double accR = 0.0, accG = 0.0, accB = 0.0, accA = 0.0;
                    size_t numColors = sampleColors.size();

                    // Move accumulation outside the parallel region
                    accR = sampleColors[0].r();
                    accG = sampleColors[0].g();
                    accB = sampleColors[0].b();
                    accA = sampleColors[0].a();

                    for (size_t i = 1; i < numColors; i++) {
                        accR += sampleColors[i].r();
                        accG += sampleColors[i].g();
                        accB += sampleColors[i].b();
                        accA += sampleColors[i].a();
                    }

                    double numSamples = static_cast<double>(numColors);
                    RGBA_Color finalColor(accR / numSamples, accG / numSamples, accB / numSamples, accA / numSamples);
                    Image3D.setPixel(x, y, finalColor.clamp());
                }
            }
        }

        return Image3D;
    }

    Image Camera::renderScene3DLight_Advanced_AA(size_t imageWidth, size_t imageHeight, math::Vector<ShapeVariant> shapes, math::Vector<Light> lights, size_t samplesPerPixel, AntiAliasingMethod method) const {
        if (samplesPerPixel == 0 || samplesPerPixel % 4 != 0) {
            throw std::invalid_argument("samplesPerPixel must be a multiple of 4 not zero");
        }

        Image Image3D(imageWidth, imageHeight);

        if (shapes.size() == 0 || lights.size() == 0) {
            return Image3D; // Return empty image if no shapes or lights
        }

        switch (method)
        {
            case Camera::AntiAliasingMethod::NONE: {
                return renderScene3DLight_Advanced(imageWidth, imageHeight, shapes, lights);
            }
            case Camera::AntiAliasingMethod::MSAA: {
                // Multi-Sample Anti-Aliasing
                return renderScene3DLight_Advanced_MSAA(imageWidth, imageHeight, shapes, lights, samplesPerPixel);
            }
            case Camera::AntiAliasingMethod::SSAA: {
                // Super-Sample Anti-Aliasing
                size_t antiAlias_imageHeight = imageHeight * samplesPerPixel / 2;
                size_t antiAlias_imageWidth = imageWidth * samplesPerPixel / 2;

                Image Image3D_antiAliased = renderScene3DLight_Advanced(antiAlias_imageWidth, antiAlias_imageHeight, shapes, lights);
                
                // Downsample anti-aliased image to final image
                return SSAADownScaling(Image3D_antiAliased, samplesPerPixel);
            }
            case Camera::AntiAliasingMethod::FXAA: {
                // Fast Approximate Anti-Aliasing
                throw std::invalid_argument("FXAA not implemented yet");
            }
            default: {
                throw std::invalid_argument("Unknown AntiAliasingMethod");
            }
        }
    }

} // namespace rendering