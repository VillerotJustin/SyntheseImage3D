//
// Created by villerot on 06/11/2025.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "./RGBA_Color.h"
#include <stdexcept>
#include <memory>
#include <string>

namespace rendering {

    /**
     * @brief Material class representing physical material properties for rendering
     * 
     * This class encapsulates all the material properties needed for physically-based
     * rendering including albedo, specular reflection, emission, and surface properties.
     */
    class Material {
    public:
        // Constructors
        Material();
        Material(const RGBA_Color& albedo);
        Material(const RGBA_Color& albedo, const RGBA_Color& specular);
        Material(const RGBA_Color& albedo, const RGBA_Color& specular, const RGBA_Color& emissive);
        
        // Copy constructor and assignment operator
        Material(const Material& other);
        Material& operator=(const Material& other);
        
        // Move constructor and assignment operator
        Material(Material&& other) noexcept;
        Material& operator=(Material&& other) noexcept;
        
        // Destructor
        ~Material() = default;

        // Albedo (base color) methods
        void setAlbedo(const RGBA_Color& color);
        const RGBA_Color& getAlbedo() const;
        bool hasAlbedo() const;
        void clearAlbedo();

        // Specular color methods
        void setSpecular(const RGBA_Color& color);
        const RGBA_Color& getSpecular() const;
        bool hasSpecular() const;
        void clearSpecular();

        // Emissive color methods  
        void setEmissive(const RGBA_Color& color);
        const RGBA_Color& getEmissive() const;
        bool hasEmissive() const;
        void clearEmissive();
        void setEmissiveIntensity(double intensity);
        double getEmissiveIntensity() const;

        // Material property setters
        void setAbsorption(double absorption);
        void setRoughness(double roughness);
        void setMetalness(double metalness);
        void setRefractiveIndex(double ior);
        void setTransmission(double transmission);

        // Material property getters
        double getAbsorption() const { return absorption; }
        double getRoughness() const { return roughness; }
        double getMetalness() const { return metalness; }
        double getRefractiveIndex() const { return refractiveIndex; }
        double getTransmission() const { return transmission; }

        // Material type queries
        bool isEmissive() const;
        bool isTransparent() const;
        bool isMetallic() const;
        bool isReflective() const;

        // Utility methods
        RGBA_Color calculateDiffuseColor(const RGBA_Color& lightColor) const;
        RGBA_Color calculateSpecularColor(const RGBA_Color& lightColor, double intensity) const;
        
        // Factory methods for common materials
        static Material createDiffuse(const RGBA_Color& color);
        static Material createMetal(const RGBA_Color& color, double roughness = 0.1);
        static Material createGlass(const RGBA_Color& color, double ior = 1.5);
        static Material createEmissive(const RGBA_Color& color, double intensity = 1.0);
        static Material createPlastic(const RGBA_Color& color, double roughness = 0.5);

        // Equality operators
        bool operator==(const Material& other) const;
        bool operator!=(const Material& other) const;

    private:
        // Color properties
        std::unique_ptr<RGBA_Color> albedo;    // The base color of the material
        std::unique_ptr<RGBA_Color> specular;  // The specular color for reflections
        std::unique_ptr<RGBA_Color> emissive;  // The emissive color for light emission
        double emissiveIntensity = 1.0; // Intensity multiplier for emissive color

        // Physical properties (clamped to valid ranges)
        double absorption = 0.0;       // Light absorption coefficient [0.0, 1.0]
        double roughness = 0.5;        // Surface roughness [0.0, 1.0] (0=mirror, 1=diffuse)
        double metalness = 0.0;        // Metallic property [0.0, 1.0] (0=dielectric, 1=metallic)
        double refractiveIndex = 1.0;  // Index of refraction [1.0, ~2.5] (1.0=air, 1.5=glass)
        double transmission = 0.0;     // Light transmission coefficient [0.0, 1.0]

        // Helper methods
        void validateProperty(double value, const std::string& propertyName) const;
        std::unique_ptr<RGBA_Color> copyColor(const std::unique_ptr<RGBA_Color>& color) const;
    };

}

#endif // MATERIAL_H