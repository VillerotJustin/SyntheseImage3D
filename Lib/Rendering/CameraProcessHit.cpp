// Import necessary headers

// Internal libraries
#include "Camera.h"

// External libraries
#include <optional>
#include <algorithm>
#include <limits>

namespace rendering {

    // Constants for better maintainability
    static constexpr double EPSILON = 1e-9;
    static constexpr double EPSILON_REMAINING = 1e-6;
    static constexpr double SHADOW_EPSILON = 1e-6;
    static constexpr double TRANSMISSION_THRESHOLD = 1e-12;

    std::optional<Hit> Camera::findNextHit(const Ray& ray, const math::Vector<rendering::Camera::ShapeVariant>& shapes, const math::Vector<size_t>& excluded_indexes) {
        Hit next_hit;
        next_hit.t = std::numeric_limits<double>::infinity();
        next_hit.shapeIndex = size_t(-1);

        for (size_t idx = 0; idx < shapes.size(); ++idx) {
            if (excluded_indexes.contains(idx)) continue;
            std::visit([&](auto&& otherShape) {
                if (otherShape.getGeometry()) {
                    if (auto d = otherShape.getGeometry()->rayIntersectDepth(ray, next_hit.t)) {
                        // only accept hits in front of the origin
                        if (*d > EPSILON) {
                            next_hit = Hit{*d, idx};
                        }
                    }
                }
            }, shapes[idx]);
        }
        if (next_hit.t == std::numeric_limits<double>::infinity()) {
            return std::nullopt;
        }
        return next_hit;
    }

    std::optional<Hit> Camera::findClosestHit(const Ray& ray, const math::Vector<rendering::Camera::ShapeVariant>& shapes, int excludeIndex) {
        Hit closest_hit;
        closest_hit.t = std::numeric_limits<double>::infinity();

        for (size_t idx = 0; idx < shapes.size(); ++idx) {
            if (int(idx) == excludeIndex) continue;
            std::visit([&](auto&& otherShape) {
                if (otherShape.getGeometry()) {
                    if (auto d = otherShape.getGeometry()->rayIntersectDepth(ray, closest_hit.t)) {
                        // only accept hits in front of the origin
                        if (*d > EPSILON) {
                            closest_hit = Hit{*d, idx};
                        }
                    }
                }
            }, shapes[idx]);
        }

        if (closest_hit.t == std::numeric_limits<double>::infinity()) {
            return std::nullopt;
        }
        return closest_hit;
    }

    RGBA_Color Camera::processRayHitOld(math::Vector<Hit>& hits, const Ray& hitRay, const math::Vector<ShapeVariant>& shapes, const math::Vector<Light>& lights){
        if (hits.empty()) return RGBA_Color(1,0,1,1); // Magenta for no hit
        
        std::sort(hits.begin(), hits.end(), [](const Hit a, const Hit b){
            return a.t < b.t;
        });

        // Front-to-back compositing using remaining transmittance
        double remaining = 1.0;
        const double epsRemaining = 1e-6;
        // Accumulate premultiplied color
        double accR = 0.0, accG = 0.0, accB = 0.0, accA = 0.0;

        // Pre-allocate accumulated light outside the loop to avoid repeated allocations
        RGBA_Color accumulatedLight;

        for (const Hit h : hits) {
            // Early termination check moved to the beginning
            if (remaining <= epsRemaining) break; // fully opaque already

            // Access the shape
            size_t i = h.shapeIndex;
            std::visit([&](auto&& shape) {
                using T = std::decay_t<decltype(shape)>;

                // Compute lighting at this hit
                const Vector3D hitPoint = hitRay.getPointAt(h.t);
                const Vector3D normal = shape.getNormalAt(hitPoint);

                // #pragma omp parallel for schedule(dynamic)
                accumulatedLight = calculateLighting(hitPoint, normal, lights, shapes, i);

                // Get surface color (avoid repeated comparisons)
                const RGBA_Color* shapeColor = shape.getMaterial() ? &shape.getMaterial()->getAlbedo() : nullptr;
                RGBA_Color surfColor;
                if (shapeColor && *shapeColor != RGBA_Color(0,0,0,1)) {
                    surfColor = *shapeColor;
                } else {
                    // Default colors by shape type
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
                    } else {
                        surfColor = RGBA_Color(1,0,1,1);
                    }
                }

                double srcA = surfColor.a();
                // Calculate lit surface color directly without temporary objects
                double litR = surfColor.r() * accumulatedLight.r();
                double litG = surfColor.g() * accumulatedLight.g();
                double litB = surfColor.b() * accumulatedLight.b();

                // premultiplied source color
                double srcR = litR * srcA;
                double srcG = litG * srcA;
                double srcB = litB * srcA;

                // accumulate front-to-back
                accR += srcR * remaining;
                accG += srcG * remaining;
                accB += srcB * remaining;
                accA += srcA * remaining;

                remaining *= (1.0 - srcA);
            }, shapes[i]);
        }

        // Build final color
        double finalA = 1.0 - remaining;
        RGBA_Color finalColor(accR, accG, accB, finalA);
        return finalColor.clamp();
    }

    RGBA_Color Camera::processRayHitRegression(const Hit& closest_hit, const Ray& hitRay, const math::Vector<ShapeVariant>& shapes, const math::Vector<Light>& lights, math::Vector<size_t> excluded_indexes, double remaining, double accR, double accG, double accB, double accA) {
        if (remaining <= EPSILON_REMAINING) {
            // Fully opaque already
            double finalA = 1.0 - remaining;
            RGBA_Color finalColor(accR, accG, accB, finalA);
            return finalColor.clamp();
        }
        
        // Access the shape
        size_t i = closest_hit.shapeIndex;
        std::visit([&](auto&& shape) {
            // using T = std::decay_t<decltype(shape)>;
            // Compute lighting at this hit
            Vector3D hitPoint = hitRay.getPointAt(closest_hit.t);
            Vector3D normal = shape.getNormalAt(hitPoint);

            RGBA_Color accumulatedLight = calculateLighting(hitPoint, normal, lights, shapes, i);

            // No ambient

            RGBA_Color surfColor = shape.getMaterial() ? shape.getMaterial()->getAlbedo() : RGBA_Color(1,0,1,1);

            RGBA_Color litSurface = surfColor * accumulatedLight;

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
        }, shapes[i]);

        // Find next hit
        excluded_indexes.append(i);

        std::optional<Hit> next_hit = findNextHit(hitRay, shapes, excluded_indexes);
        if (next_hit) {
            return processRayHitRegression(*next_hit, hitRay, shapes, lights, excluded_indexes, remaining,  accR, accG, accB, accA);
        }

        // No more hits, build final color
        double finalA = 1.0 - remaining;
        RGBA_Color finalColor(accR, accG, accB, finalA);
        return finalColor.clamp();
    }

    RGBA_Color Camera::processRayHitAdvanced(const Hit& hit, const Ray& hitRay, const math::Vector<ShapeVariant>& shapes, const math::Vector<Light>& lights, int recursivity_depth){
        // Add recursivity depth check | Moved to later to return local color with no processing
        // if (recursivity_depth <= 0) {
        //     return new RGBA_Color(0,0,0,1); // Black if max depth
        // }

        if (hit.t == std::numeric_limits<double>::infinity()) return RGBA_Color(1,0,1,1); // Magenta for no hit
        
        RGBA_Color Local_color(0,0,0,1);
        RGBA_Color Transparency_color(1,0,1,1);
        RGBA_Color Reflection_color(1,0,1,1);
        RGBA_Color final_color(1,0,1,1);

        // Access the shape
        size_t i = hit.shapeIndex;
        std::visit([&](auto&& shape) {

            // Get material
            const Material* material = shape.getMaterial();

            // Get Color at hit point

            // Compute lighting at this hit
            Vector3D hitPoint = hitRay.getPointAt(hit.t);
            Vector3D normal = shape.getNormalAt(hitPoint);

            RGBA_Color accumulatedLight = calculateLighting(hitPoint, normal, lights, shapes, i);

            // No ambient

            RGBA_Color surfColor = material ? material->getAlbedo() : RGBA_Color(1,0,1,1);
            Local_color = (surfColor * accumulatedLight).clamp();

            if (recursivity_depth <= 0) {
                final_color = Local_color.clamp();
                return;
            }

            // If no material, skip advanced processing but continue to blending
            if (material != nullptr) {
                Vector3D rayDir = hitRay.getDirection();
                
                // Handle transparency with recursion
                if (material->isTransparent()) {
                    Vector3D refractDir = material->getRefractedDirection(rayDir, normal);
                    Ray refractRay(hitPoint + refractDir * 1e-4, refractDir);
                    
                    std::optional<Hit> next_hit = findClosestHit(refractRay, shapes, i);

                    if (next_hit) {
                        RGBA_Color behindColor = processRayHitAdvanced(*next_hit, refractRay, shapes, lights, recursivity_depth - 1);
                        // Apply material color as a filter to the light passing through
                        RGBA_Color materialFilter = material->getAlbedo();
                        Transparency_color = RGBA_Color(
                            behindColor.r() * materialFilter.r(),
                            behindColor.g() * materialFilter.g(), 
                            behindColor.b() * materialFilter.b(),
                            behindColor.a()
                        );
                    } else {
                        Transparency_color = RGBA_Color(1,0,1,1); // Debug color
                    }
                }

                // Handle reflection/refraction here if needed
                if (material->isReflective()) {
                    Vector3D reflectDir = rayDir - normal * 2.0 * rayDir.dot(normal);
                    Ray reflectRay(hitPoint + reflectDir * 1e-4, reflectDir);

                    std::optional<Hit> next_hit = findClosestHit(reflectRay, shapes, i);

                    if (next_hit) {
                        Reflection_color = processRayHitAdvanced(*next_hit, reflectRay, shapes, lights, recursivity_depth - 1);
                    } else {
                        Reflection_color = RGBA_Color(1,0,1,1); // Debug color
                    }
                }
            } // End of material processing

            // Combine colors based on material properties
            // Blend local color with reflection and transmission based on material properties
            final_color = Local_color;
            
            if (material) {
                double metalness = material->getMetalness();
                double transmission = material->getTransmission();
                double roughness = material->getRoughness();
                
                // For transparent materials, blend with transmitted light
                if (material->isTransparent() && Transparency_color!= RGBA_Color(1,0,1,1)) {
                    // Transmission strength based on material transmission property or alpha channel
                    double transmissionStrength = transmission;
                    if (transmissionStrength == 0.0 && material->hasAlbedo()) {
                        // Use alpha channel for transparency if no explicit transmission
                        transmissionStrength = 1.0 - material->getAlbedo().a();
                    }
                    transmissionStrength *= (1.0 - metalness); // Metals don't transmit light
                    // Manual alpha blending for transparency
                    final_color = final_color * (1.0 - transmissionStrength) + Transparency_color * transmissionStrength;
                }

                // For metallic materials, blend more reflection
                if (material->isReflective() && Reflection_color!= RGBA_Color(1,0,1,1)) {
                    // Fresnel-like reflection mixing based on metalness and roughness
                    double reflectionStrength = metalness * (1.0 - roughness * 0.8);
                    // Manual alpha blending: result = src * (1-alpha) + dst * alpha
                    final_color = final_color * (1.0 - reflectionStrength) + Reflection_color * reflectionStrength;
                }
                
                // Add emissive contribution if material is emissive
                if (material->isEmissive()) {
                    RGBA_Color emissiveContrib = material->getEmissive() * material->getEmissiveIntensity();
                    final_color = final_color + emissiveContrib;
                }
            }
            
            // Apply clamping to final color
            final_color = final_color.clamp();

        }, shapes[i]);
        return final_color;
        
        // Fallback return (should not reach here)
        return RGBA_Color(1, 0, 1, 1); // Magenta for error
    }

    RGBA_Color Camera::calculateLighting(const Vector3D& hitPoint, const Vector3D& normal, const math::Vector<Light>& lights, const math::Vector<ShapeVariant>& shapes, size_t selfIndex){
        RGBA_Color accumulatedLight(0.0, 0.0, 0.0, 1.0);
        
        // #pragma omp parallel for schedule(dynamic)
        for (const Light &light : lights) {
            Vector3D hitToLight = (light.getPosition() - hitPoint);
            double distanceToLight = hitToLight.length();
            Vector3D lightDir = hitToLight.normal();

            Ray lightRay(hitPoint + lightDir * SHADOW_EPSILON, lightDir);
            double transmission = 1.0;

            // Check for occlusions
            // #pragma omp parallel for schedule(dynamic)
            for (size_t j = 0; j < shapes.size(); ++j) {
                if (selfIndex != j && transmission > TRANSMISSION_THRESHOLD) {
                    std::visit([&](auto&& otherShape) {
                        if (otherShape.getGeometry()) {
                            auto shadowDist = otherShape.getGeometry()->rayIntersectDepth(lightRay, std::numeric_limits<double>::infinity());
                            if (shadowDist && *shadowDist < distanceToLight) {
                                const RGBA_Color* occColor = otherShape.getMaterial() ? &otherShape.getMaterial()->getAlbedo() : nullptr;
                                double occAlpha = occColor ? occColor->a() : 1.0;
                                if (occAlpha >= 1.0 - TRANSMISSION_THRESHOLD) {
                                    transmission = 0.0;
                                } else {
                                    transmission *= (1.0 - occAlpha);
                                }
                            }
                        }
                    }, shapes[j]);
                }
            }

            if (transmission > TRANSMISSION_THRESHOLD) {
                double nDotL = std::max(0.0, normal.dot(lightDir));
                RGBA_Color lightCol = light.getColor() * light.getIntensity();
                double distanceAtten = 1.0 / (1.0 + 0.03 * distanceToLight * distanceToLight);
                RGBA_Color contrib = lightCol * (transmission * nDotL * distanceAtten);
                accumulatedLight = accumulatedLight + contrib;
            }
        }
        return accumulatedLight;
    }
}