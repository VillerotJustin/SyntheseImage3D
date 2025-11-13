// Import necessary headers

// Internal libraries
#include "Camera.h"

// External libraries
#include <optional>
#include <algorithm>
#include <limits>

namespace rendering {

    std::optional<Hit> Camera::findNextHit(const Ray& ray, const math::Vector<rendering::Camera::ShapeVariant>& shapes, const math::Vector<size_t>& index_to_test) {
        Hit closest_hit;
        closest_hit.t = std::numeric_limits<double>::infinity();
        closest_hit.shapeIndex = size_t(-1);

        for (size_t idx : index_to_test) {
            std::visit([&](auto&& shape) {
                
                std::optional<double> distance = std::nullopt;
                if (shape.getGeometry()) {
                    distance = shape.getGeometry()->rayIntersectDepth(ray);
                }

                if (distance && *distance < closest_hit.t) {
                    closest_hit.t = *distance;
                    closest_hit.shapeIndex = idx;
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
        double lightAccR = 0.0, lightAccG = 0.0, lightAccB = 0.0;

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

                // Reset accumulated light for this hit (reuse variables)
                lightAccR = 0.0;
                lightAccG = 0.0;
                lightAccB = 0.0;

                #pragma omp parallel for schedule(dynamic)
                for (const Light &light : lights) {
                    const Vector3D hitToLight = (light.getPosition() - hitPoint);
                    double distanceToLight = hitToLight.length();
                    const Vector3D lightDir = hitToLight.normal();

                    const double epsilon = 1e-4;
                    const Ray lightRay(hitPoint + lightDir * epsilon, lightDir);

                    double transmission = 1.0;

                    // Check for occlusions (sequential for early termination)
                    for (size_t j = 0; j < shapes.size() && transmission > 1e-12; ++j) {
                        if (i != j) {
                            std::visit([&](auto&& otherShape) {
                                if (otherShape.getGeometry() && transmission > 1e-12) {
                                    auto shadowDist = otherShape.getGeometry()->rayIntersectDepth(lightRay);
                                    if (shadowDist && *shadowDist < distanceToLight) {
                                        const RGBA_Color* occColor = otherShape.getMaterial() ? &otherShape.getMaterial()->getAlbedo() : nullptr;
                                        double occAlpha = occColor ? occColor->a() : 1.0;
                                        if (occAlpha >= 1.0 - 1e-12) {
                                            transmission = 0.0; // Early exit when fully occluded
                                        } else {
                                            transmission *= (1.0 - occAlpha);
                                        }
                                    }
                                }
                            }, shapes[j]);
                        }
                    }

                    if (transmission > 1e-12) {
                        double nDotL = std::max(0.0, normal.dot(lightDir));
                        const RGBA_Color& lightColor = light.getColor();
                        double intensity = light.getIntensity();
                        double distanceAtten = 1.0 / (1.0 + 0.03 * distanceToLight * distanceToLight);
                        
                        // Avoid creating temporary RGBA_Color objects
                        double lightContrib = transmission * nDotL * distanceAtten * intensity;
                        lightAccR += lightColor.r() * lightContrib;
                        lightAccG += lightColor.g() * lightContrib;
                        lightAccB += lightColor.b() * lightContrib;
                    }
                }

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
                double litR = surfColor.r() * lightAccR;
                double litG = surfColor.g() * lightAccG;
                double litB = surfColor.b() * lightAccB;

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

    RGBA_Color Camera::processRayHitRegression(const Hit& closest_hit, const Ray& hitRay, const math::Vector<ShapeVariant>& shapes, const math::Vector<Light>& lights, math::Vector<size_t> index_to_test, double remaining, double accR, double accG, double accB, double accA) {
        if (remaining <= 1e-6) {
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

            RGBA_Color accumulatedLight(0.0, 0.0, 0.0, 1.0);
            #pragma omp parallel for schedule(dynamic)
            for (const Light &light : lights) {
                Vector3D hitToLight = (light.getPosition() - hitPoint);
                double distanceToLight = hitToLight.length();
                Vector3D lightDir = hitToLight.normal();

                const double epsilon = 1e-4;
                Ray lightRay(hitPoint + lightDir * epsilon, lightDir);

                double transmission = 1.0;

                // Check for occlusions
                #pragma omp parallel for schedule(dynamic)
                for (size_t j = 0; j < shapes.size(); ++j) {
                    if (i != j && transmission > 1e-12) {
                        std::visit([&](auto&& otherShape) {
                            if (otherShape.getGeometry()) {
                                auto shadowDist = otherShape.getGeometry()->rayIntersectDepth(lightRay);
                                if (shadowDist && *shadowDist < distanceToLight) {
                                    const RGBA_Color* occColor = otherShape.getMaterial() ? &otherShape.getMaterial()->getAlbedo() : nullptr;
                                    double occAlpha = occColor ? occColor->a() : 1.0;
                                    if (occAlpha >= 1.0 - 1e-12) {
                                        transmission = 0.0;
                                    } else {
                                        transmission *= (1.0 - occAlpha);
                                    }
                                }
                            }
                        }, shapes[j]);
                    }
                }

                if (transmission > 1e-12) {
                    double nDotL = std::max(0.0, normal.dot(lightDir));
                    RGBA_Color lightCol = light.getColor() * light.getIntensity();
                    double distanceAtten = 1.0 / (1.0 + 0.03 * distanceToLight * distanceToLight);
                    RGBA_Color contrib = lightCol * (transmission * nDotL * distanceAtten);
                    accumulatedLight = accumulatedLight + contrib;
                }
            }

            // No ambient

            RGBA_Color surfColor = *shape.getColor();

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
        for (size_t idx = 0; idx < index_to_test.size(); ++idx) {
            if (index_to_test[idx] == closest_hit.shapeIndex) {
                index_to_test.erase(idx);
                break;
            }
        }

        std::optional<Hit> next_hit = findNextHit(hitRay, shapes, index_to_test);
        if (next_hit) {
            return processRayHitRegression(*next_hit, hitRay, shapes, lights, index_to_test, remaining,  accR, accG, accB, accA);
        }

        // No more hits, build final color
        double finalA = 1.0 - remaining;
        RGBA_Color finalColor(accR, accG, accB, finalA);
        return finalColor.clamp();
    }

    RGBA_Color* Camera::processRayHitAdvanced(const Hit& hit, const Ray& hitRay, const math::Vector<ShapeVariant>& shapes, const math::Vector<Light>& lights, int recursivity_depth){
        // Add recursivity depth check
        if (recursivity_depth <= 0) {
            return new RGBA_Color(0,0,0,1); // Black if max depth
        }

        if (hit.t == std::numeric_limits<double>::infinity()) return new RGBA_Color(1,0,1,1); // Magenta for no hit
        
        RGBA_Color* Local_color = new RGBA_Color(0,0,0,1);
        RGBA_Color* Transparency_color = new RGBA_Color(0,0,0,1);
        RGBA_Color* Reflection_color = new RGBA_Color(0,0,0,1);
        RGBA_Color *final_color = new RGBA_Color(1,0,1,1);

        // Access the shape
        size_t i = hit.shapeIndex;
        std::visit([&](auto&& shape) {

            // Get material
            const Material* material = shape.getMaterial();

            // Get Color at hit point

            // Compute lighting at this hit
            Vector3D hitPoint = hitRay.getPointAt(hit.t);
            Vector3D normal = shape.getNormalAt(hitPoint);

            RGBA_Color accumulatedLight(0.0, 0.0, 0.0, 1.0);
            #pragma omp parallel for schedule(dynamic)
            for (const Light &light : lights) {
                Vector3D hitToLight = (light.getPosition() - hitPoint);
                double distanceToLight = hitToLight.length();
                Vector3D lightDir = hitToLight.normal();

                const double epsilon = 1e-4;
                Ray lightRay(hitPoint + lightDir * epsilon, lightDir);

                double transmission = 1.0;

                // Check for occlusions
                #pragma omp parallel for schedule(dynamic)
                for (size_t j = 0; j < shapes.size(); ++j) {
                    if (i != j && transmission > 1e-12) {
                        std::visit([&](auto&& otherShape) {
                            if (otherShape.getGeometry()) {
                                auto shadowDist = otherShape.getGeometry()->rayIntersectDepth(lightRay);
                                if (shadowDist && *shadowDist < distanceToLight) {
                                    const RGBA_Color* occColor = otherShape.getMaterial() ? &otherShape.getMaterial()->getAlbedo() : nullptr;
                                    double occAlpha = occColor ? occColor->a() : 1.0;
                                    if (occAlpha >= 1.0 - 1e-12) {
                                        transmission = 0.0;
                                    } else {
                                        transmission *= (1.0 - occAlpha);
                                    }
                                }
                            }
                        }, shapes[j]);
                    }
                }

                if (transmission > 1e-12) {
                    double nDotL = std::max(0.0, normal.dot(lightDir));
                    RGBA_Color lightCol = light.getColor() * light.getIntensity();
                    double distanceAtten = 1.0 / (1.0 + 0.03 * distanceToLight * distanceToLight);
                    RGBA_Color contrib = lightCol * (transmission * nDotL * distanceAtten);
                    accumulatedLight = accumulatedLight + contrib;
                }
            }

            // No ambient

            RGBA_Color surfColor = material ? material->getAlbedo() : RGBA_Color(1,0,1,1);
            *Local_color = (surfColor * accumulatedLight).clamp();

            // If no material, skip advanced processing but continue to blending
            if (material != nullptr) {
                Vector3D rayDir = hitRay.getDirection();
                
                // Handle transparency with recursion
                if (material->isTransparent()) {
                    Ray refractRay(hitPoint + rayDir * 1e-4, material ? material->getRefractedDirection(rayDir, normal) : rayDir);
                    
                    Hit next_hit;
                    double closest_t = std::numeric_limits<double>::infinity();

                    for (size_t idx = 0; idx < shapes.size(); ++idx) {
                        if (idx == i) continue; // Skip current shape
                        std::visit([&](auto&& otherShape) {
                            if (otherShape.getGeometry()) {
                                if (auto d = otherShape.getGeometry()->rayIntersectDepth(refractRay, closest_t)) {
                                    // only accept hits in front of the origin
                                    if (*d > 1e-9) {
                                        next_hit = Hit{*d, idx};
                                        closest_t = *d;
                                    }
                                }
                            }
                        }, shapes[idx]);
                    }

                    if (closest_t < std::numeric_limits<double>::infinity()) {
                        Transparency_color = processRayHitAdvanced(next_hit, refractRay, shapes, lights, recursivity_depth - 1);
                    } else {
                        Transparency_color = new RGBA_Color(1,0,1,1); // Debug color
                    }
                }

                // Handle reflection/refraction here if needed
                if (material->isReflective()) {
                    Vector3D reflectDir = rayDir - normal * 2.0 * rayDir.dot(normal);
                    Ray reflectRay(hitPoint + reflectDir * 1e-4, reflectDir);

                    Hit next_hit;
                    double closest_t = std::numeric_limits<double>::infinity();

                    for (size_t idx = 0; idx < shapes.size(); ++idx) {
                        if (idx == i) continue; // Skip current shape
                        std::visit([&](auto&& otherShape) {
                            if (otherShape.getGeometry()) {
                                if (auto d = otherShape.getGeometry()->rayIntersectDepth(reflectRay, closest_t)) {
                                    // only accept hits in front of the origin
                                    if (*d > 1e-9) {
                                        next_hit = Hit{*d, idx};
                                        closest_t = *d;
                                    }
                                }
                            }
                        }, shapes[idx]);
                    }

                    if (closest_t < std::numeric_limits<double>::infinity()) {
                        Reflection_color = processRayHitAdvanced(next_hit, reflectRay, shapes, lights, recursivity_depth - 1);
                    } else {
                        Reflection_color = new RGBA_Color(1,0,1,1); // Debug color
                    }
                }
            } // End of material processing

            // Combine colors based on material properties
            // Blend local color with reflection and transmission based on material properties
            *final_color = *Local_color;
            
            if (material) {
                double metalness = material->getMetalness();
                double transmission = material->getTransmission();
                double roughness = material->getRoughness();
                
                // For metallic materials, blend more reflection
                if (material->isReflective() && Reflection_color) {
                    // Fresnel-like reflection mixing based on metalness and roughness
                    double reflectionStrength = metalness * (1.0 - roughness * 0.8);
                    // Manual alpha blending: result = src * (1-alpha) + dst * alpha
                    *final_color = (*final_color) * (1.0 - reflectionStrength) + (*Reflection_color) * reflectionStrength;
                }
                
                // For transparent materials, blend with transmitted light
                if (material->isTransparent() && Transparency_color) {
                    // Transmission strength based on material transmission property or alpha channel
                    double transmissionStrength = transmission;
                    if (transmissionStrength == 0.0 && material->hasAlbedo()) {
                        // Use alpha channel for transparency if no explicit transmission
                        transmissionStrength = 1.0 - material->getAlbedo().a();
                    }
                    transmissionStrength *= (1.0 - metalness); // Metals don't transmit light
                    // Manual alpha blending for transparency
                    *final_color = (*final_color) * (1.0 - transmissionStrength) + (*Transparency_color) * transmissionStrength;
                }
                
                // Add emissive contribution if material is emissive
                if (material->isEmissive()) {
                    RGBA_Color emissiveContrib = material->getEmissive() * material->getEmissiveIntensity();
                    *final_color = *final_color + emissiveContrib;
                }
            }
            
            // Apply clamping to final color
            *final_color = final_color->clamp();
            
            // Clean up temporary colors
            if (Transparency_color && Transparency_color != final_color) {
                delete Transparency_color;
            }
            if (Reflection_color && Reflection_color != final_color) {
                delete Reflection_color;
            }
            if (Local_color && Local_color != final_color) {
                delete Local_color;
            }

        }, shapes[i]);
        return final_color;
        
        // Fallback return (should not reach here)
        return new RGBA_Color(1, 0, 1, 1); // Magenta for error
    }
}