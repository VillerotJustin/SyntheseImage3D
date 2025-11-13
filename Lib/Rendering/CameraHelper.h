//
// Created by villerot on 13/11/2025.
//

#ifndef CAMERA_HELPER_HPP
#define CAMERA_HELPER_HPP

#include "Camera.h"
#include "Image.h"
#include "RGBA_Color.h"
#include "../Geometry/Ray.h"
#include "../Math/Vector.hpp"
#include "../Math/Matrix.hpp"
#include "../Math/math_common.h"
#include "Shape.hpp"

#include <limits>
#include <algorithm>
#include <cmath>

namespace rendering {

    /**
     * Apply depth shading to a specific pixel based on depth and maximum depth
     * @param image The image to modify
     * @param x The x-coordinate of the pixel
     * @param y The y-coordinate of the pixel  
     * @param depth The depth value at this pixel
     * @param max_depth The maximum depth in the scene
     */
    void applyDepthShadingToPixel(Image& image, size_t x, size_t y, double depth, double max_depth);

    /**
     * Apply depth shading to an entire image based on a depth buffer
     * @param image The image to modify
     * @param depthBuffer The depth buffer containing depth values
     * @param max_depth The maximum depth in the scene
     */
    void applyDepthShadingToImage(Image& image, const math::Matrix<double>& depthBuffer, double max_depth);

    /**
     * Simple shape processing for ray intersection testing
     * @param ray The ray to test intersections with
     * @param shapes The vector of shapes to test against
     * @param pixelColor Output parameter for the resulting color
     * @param closestDistance Output parameter for the closest intersection distance
     * @param hitFound Output parameter indicating if any hit was found
     */
    void shapeProcessSimple(Ray& ray, math::Vector<rendering::Camera::ShapeVariant>& shapes, RGBA_Color& pixelColor, double& closestDistance, bool& hitFound);

    /**
     * Super-Sample Anti-Aliasing downscaling function
     * @param image_in The high-resolution input image
     * @param samplesPerPixel The number of samples per pixel used
     * @return Image The downscaled image with anti-aliasing applied
     */
    Image SSAADownScaling(Image& image_in, size_t samplesPerPixel);

} // namespace rendering

#endif // CAMERA_HELPER_HPP
