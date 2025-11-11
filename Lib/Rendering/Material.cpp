//
// Created by villerot on 06/11/2025.
//

#include "Material.h"
#include <algorithm>
#include <cmath>

namespace rendering {

    // Default constructor
    Material::Material() {
        // Initialize with default white albedo
        albedo = std::make_unique<RGBA_Color>(1.0, 1.0, 1.0, 1.0);
    }

    // Constructor with albedo only
    Material::Material(const RGBA_Color& albedo) {
        this->albedo = std::make_unique<RGBA_Color>(albedo);
    }

    // Constructor with albedo and specular
    Material::Material(const RGBA_Color& albedo, const RGBA_Color& specular) {
        this->albedo = std::make_unique<RGBA_Color>(albedo);
        this->specular = std::make_unique<RGBA_Color>(specular);
    }

    // Constructor with albedo, specular, and emissive
    Material::Material(const RGBA_Color& albedo, const RGBA_Color& specular, const RGBA_Color& emissive) {
        this->albedo = std::make_unique<RGBA_Color>(albedo);
        this->specular = std::make_unique<RGBA_Color>(specular);
        this->emissive = std::make_unique<RGBA_Color>(emissive);
    }

    // Copy constructor
    Material::Material(const Material& other) 
        : absorption(other.absorption),
          roughness(other.roughness),
          metalness(other.metalness),
          refractiveIndex(other.refractiveIndex),
          transmission(other.transmission) {
        
        albedo = copyColor(other.albedo);
        specular = copyColor(other.specular);
        emissive = copyColor(other.emissive);
    }

    // Copy assignment operator
    Material& Material::operator=(const Material& other) {
        if (this != &other) {
            albedo = copyColor(other.albedo);
            specular = copyColor(other.specular);
            emissive = copyColor(other.emissive);
            
            absorption = other.absorption;
            roughness = other.roughness;
            metalness = other.metalness;
            refractiveIndex = other.refractiveIndex;
            transmission = other.transmission;
        }
        return *this;
    }

    // Move constructor
    Material::Material(Material&& other) noexcept
        : albedo(std::move(other.albedo)),
          specular(std::move(other.specular)),
          emissive(std::move(other.emissive)),
          absorption(other.absorption),
          roughness(other.roughness),
          metalness(other.metalness),
          refractiveIndex(other.refractiveIndex),
          transmission(other.transmission) {
    }

    // Move assignment operator
    Material& Material::operator=(Material&& other) noexcept {
        if (this != &other) {
            albedo = std::move(other.albedo);
            specular = std::move(other.specular);
            emissive = std::move(other.emissive);
            
            absorption = other.absorption;
            roughness = other.roughness;
            metalness = other.metalness;
            refractiveIndex = other.refractiveIndex;
            transmission = other.transmission;
        }
        return *this;
    }

    // Albedo methods
    void Material::setAlbedo(const RGBA_Color& color) {
        albedo = std::make_unique<RGBA_Color>(color);
    }

    const RGBA_Color& Material::getAlbedo() const {
        if (!albedo) {
            throw std::runtime_error("Material has no albedo color");
        }
        return *albedo;
    }

    bool Material::hasAlbedo() const {
        return albedo != nullptr;
    }

    void Material::clearAlbedo() {
        albedo.reset();
    }

    // Specular methods
    void Material::setSpecular(const RGBA_Color& color) {
        specular = std::make_unique<RGBA_Color>(color);
    }

    const RGBA_Color& Material::getSpecular() const {
        if (!specular) {
            throw std::runtime_error("Material has no specular color");
        }
        return *specular;
    }

    bool Material::hasSpecular() const {
        return specular != nullptr;
    }

    void Material::clearSpecular() {
        specular.reset();
    }

    // Emissive methods
    void Material::setEmissive(const RGBA_Color& color) {
        emissive = std::make_unique<RGBA_Color>(color);
    }

    const RGBA_Color& Material::getEmissive() const {
        if (!emissive) {
            throw std::runtime_error("Material has no emissive color");
        }
        return *emissive;
    }

    bool Material::hasEmissive() const {
        return emissive != nullptr;
    }

    void Material::clearEmissive() {
        emissive.reset();
    }

    void Material::setEmissiveIntensity(double intensity) {
        if (intensity < 0.0) {
            throw std::invalid_argument("Emissive intensity must be non-negative");
        }
        emissiveIntensity = intensity;
    }

    double Material::getEmissiveIntensity() const {
        return emissiveIntensity;
    }

    // Property setters with validation
    void Material::setAbsorption(double absorption) {
        validateProperty(absorption, "absorption");
        this->absorption = std::clamp(absorption, 0.0, 1.0);
    }

    void Material::setRoughness(double roughness) {
        validateProperty(roughness, "roughness");
        this->roughness = std::clamp(roughness, 0.0, 1.0);
    }

    void Material::setMetalness(double metalness) {
        validateProperty(metalness, "metalness");
        this->metalness = std::clamp(metalness, 0.0, 1.0);
    }

    void Material::setRefractiveIndex(double ior) {
        if (ior < 1.0) {
            throw std::invalid_argument("Refractive index must be >= 1.0");
        }
        this->refractiveIndex = ior;
    }

    void Material::setTransmission(double transmission) {
        validateProperty(transmission, "transmission");
        this->transmission = std::clamp(transmission, 0.0, 1.0);
    }

    // Material type queries
    bool Material::isEmissive() const {
        return hasEmissive() && (emissive->r() > 0.0 || emissive->g() > 0.0 || emissive->b() > 0.0);
    }

    bool Material::isTransparent() const {
        return transmission > 0.0 || (hasAlbedo() && albedo->a() < 1.0);
    }

    bool Material::isMetallic() const {
        return metalness > 0.5;
    }

    bool Material::isReflective() const {
        return hasSpecular() || isMetallic();
    }

    // Utility methods
    RGBA_Color Material::calculateDiffuseColor(const RGBA_Color& lightColor) const {
        if (!hasAlbedo()) {
            return RGBA_Color(0, 0, 0, 1);
        }
        
        // Simple diffuse calculation: albedo * lightColor
        const RGBA_Color& baseColor = getAlbedo();
        return RGBA_Color(
            baseColor.r() * lightColor.r(),
            baseColor.g() * lightColor.g(),
            baseColor.b() * lightColor.b(),
            baseColor.a()
        );
    }

    RGBA_Color Material::calculateSpecularColor(const RGBA_Color& lightColor, double intensity) const {
        if (!hasSpecular()) {
            return RGBA_Color(0, 0, 0, 1);
        }
        
        // Specular calculation with intensity and roughness consideration
        double specularIntensity = intensity * (1.0 - roughness);
        const RGBA_Color& specColor = getSpecular();
        
        return RGBA_Color(
            specColor.r() * lightColor.r() * specularIntensity,
            specColor.g() * lightColor.g() * specularIntensity,
            specColor.b() * lightColor.b() * specularIntensity,
            specColor.a()
        );
    }

    // Factory methods
    Material Material::createDiffuse(const RGBA_Color& color) {
        Material material(color);
        material.setRoughness(1.0);  // Fully rough for diffuse
        material.setMetalness(0.0);  // Non-metallic
        return material;
    }

    Material Material::createMetal(const RGBA_Color& color, double roughness) {
        Material material(color);
        material.setSpecular(color);  // Metals have colored specular
        material.setRoughness(roughness);
        material.setMetalness(1.0);   // Fully metallic
        return material;
    }

    Material Material::createGlass(const RGBA_Color& color, double ior) {
        Material material(color);
        material.setRefractiveIndex(ior);
        material.setTransmission(0.9);  // High transmission
        material.setRoughness(0.0);     // Smooth surface
        material.setMetalness(0.0);     // Non-metallic
        return material;
    }

    Material Material::createEmissive(const RGBA_Color& color, double intensity) {
        Material material;
        RGBA_Color emissiveColor(
            color.r(),
            color.g(),
            color.b(),
            color.a()
        );
        material.setEmissive(emissiveColor);
        material.setEmissiveIntensity(intensity);
        material.setAlbedo(RGBA_Color(0, 0, 0, 1)); // Black albedo for pure emission
        return material;
    }

    Material Material::createPlastic(const RGBA_Color& color, double roughness) {
        Material material(color);
        material.setSpecular(RGBA_Color(1, 1, 1, 1)); // White specular for plastic
        material.setRoughness(roughness);
        material.setMetalness(0.0);    // Non-metallic
        return material;
    }

    // Equality operators
    bool Material::operator==(const Material& other) const {
        // Compare properties
        if (std::abs(absorption - other.absorption) > 1e-6 ||
            std::abs(roughness - other.roughness) > 1e-6 ||
            std::abs(metalness - other.metalness) > 1e-6 ||
            std::abs(refractiveIndex - other.refractiveIndex) > 1e-6 ||
            std::abs(transmission - other.transmission) > 1e-6) {
            return false;
        }

        // Compare colors
        if (hasAlbedo() != other.hasAlbedo() || 
            (hasAlbedo() && getAlbedo() != other.getAlbedo())) {
            return false;
        }

        if (hasSpecular() != other.hasSpecular() || 
            (hasSpecular() && getSpecular() != other.getSpecular())) {
            return false;
        }

        if (hasEmissive() != other.hasEmissive() || 
            (hasEmissive() && getEmissive() != other.getEmissive())) {
            return false;
        }

        return true;
    }

    bool Material::operator!=(const Material& other) const {
        return !(*this == other);
    }

    // Helper methods
    void Material::validateProperty(double value, const std::string& propertyName) const {
        if (std::isnan(value) || std::isinf(value)) {
            throw std::invalid_argument("Material property '" + propertyName + "' cannot be NaN or infinite");
        }
        if (value < 0.0 || value > 1.0) {
            throw std::invalid_argument("Material property '" + propertyName + "' must be in range [0.0, 1.0]");
        }
    }

    std::unique_ptr<RGBA_Color> Material::copyColor(const std::unique_ptr<RGBA_Color>& color) const {
        if (!color) {
            return nullptr;
        }
        return std::make_unique<RGBA_Color>(*color);
    }

}