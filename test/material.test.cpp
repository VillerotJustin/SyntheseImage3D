#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include "../Lib/Rendering/Material.h"
#include "../Lib/Rendering/RGBA_Color.h"

using namespace rendering;

// Helper function for floating-point comparison
bool isEqual(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
}

// Test function declarations
void testMaterialConstructors();
void testMaterialProperties();
void testMaterialColors();
void testMaterialTypeQueries();
void testMaterialUtilityMethods();
void testMaterialFactoryMethods();
void testMaterialCopySemantics();
void testMaterialValidation();
void testMaterialEquality();

int main() {
    std::cout << "Running Material tests..." << std::endl;
    
    try {
        testMaterialConstructors();
        std::cout << "✓ Material constructor tests passed" << std::endl;
        
        testMaterialProperties();
        std::cout << "✓ Material property tests passed" << std::endl;
        
        testMaterialColors();
        std::cout << "✓ Material color tests passed" << std::endl;
        
        testMaterialTypeQueries();
        std::cout << "✓ Material type query tests passed" << std::endl;
        
        testMaterialUtilityMethods();
        std::cout << "✓ Material utility method tests passed" << std::endl;
        
        testMaterialFactoryMethods();
        std::cout << "✓ Material factory method tests passed" << std::endl;
        
        testMaterialCopySemantics();
        std::cout << "✓ Material copy semantics tests passed" << std::endl;
        
        testMaterialValidation();
        std::cout << "✓ Material validation tests passed" << std::endl;
        
        testMaterialEquality();
        std::cout << "✓ Material equality tests passed" << std::endl;
        
        std::cout << "All Material tests passed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}

void testMaterialConstructors() {
    // Test default constructor
    Material defaultMaterial;
    assert(defaultMaterial.hasAlbedo());
    assert(defaultMaterial.getAlbedo() == RGBA_Color(1.0, 1.0, 1.0, 1.0));
    assert(!defaultMaterial.hasSpecular());
    assert(!defaultMaterial.hasEmissive());
    
    // Test albedo-only constructor
    RGBA_Color red(1.0, 0.0, 0.0, 1.0);
    Material redMaterial(red);
    assert(redMaterial.hasAlbedo());
    assert(redMaterial.getAlbedo() == red);
    assert(!redMaterial.hasSpecular());
    assert(!redMaterial.hasEmissive());
    
    // Test albedo + specular constructor
    RGBA_Color white(1.0, 1.0, 1.0, 1.0);
    Material specularMaterial(red, white);
    assert(specularMaterial.hasAlbedo());
    assert(specularMaterial.getAlbedo() == red);
    assert(specularMaterial.hasSpecular());
    assert(specularMaterial.getSpecular() == white);
    assert(!specularMaterial.hasEmissive());
    
    // Test full constructor
    RGBA_Color blue(0.0, 0.0, 1.0, 1.0);
    Material fullMaterial(red, white, blue);
    assert(fullMaterial.hasAlbedo());
    assert(fullMaterial.getAlbedo() == red);
    assert(fullMaterial.hasSpecular());
    assert(fullMaterial.getSpecular() == white);
    assert(fullMaterial.hasEmissive());
    assert(fullMaterial.getEmissive() == blue);
}

void testMaterialProperties() {
    Material material;
    
    // Test default values
    assert(isEqual(material.getAbsorption(), 0.0));
    assert(isEqual(material.getRoughness(), 0.5));
    assert(isEqual(material.getMetalness(), 0.0));
    assert(isEqual(material.getRefractiveIndex(), 1.0));
    assert(isEqual(material.getTransmission(), 0.0));
    
    // Test setting valid values
    material.setAbsorption(0.3);
    assert(isEqual(material.getAbsorption(), 0.3));
    
    material.setRoughness(0.8);
    assert(isEqual(material.getRoughness(), 0.8));
    
    material.setMetalness(1.0);
    assert(isEqual(material.getMetalness(), 1.0));
    
    material.setRefractiveIndex(1.5);
    assert(isEqual(material.getRefractiveIndex(), 1.5));
    
    material.setTransmission(0.9);
    assert(isEqual(material.getTransmission(), 0.9));
    
    // Test error handling for out-of-bounds values
    try {
        material.setAbsorption(1.5); // Should clamp to 1.0
        assert(false); // Should'nt reach here
    } catch (const std::invalid_argument&) {
        // Expected exception
    }
    
    try {
        material.setRoughness(-0.1); // Should clamp to 0.0
        assert(false); // Should'nt reach here
    } catch (const std::invalid_argument&) {
        // Expected exception
    }
}

void testMaterialColors() {
    Material material;
    
    // Test albedo operations
    RGBA_Color red(1.0, 0.0, 0.0, 1.0);
    material.setAlbedo(red);
    assert(material.hasAlbedo());
    assert(material.getAlbedo() == red);
    
    material.clearAlbedo();
    assert(!material.hasAlbedo());
    
    // Test specular operations
    RGBA_Color white(1.0, 1.0, 1.0, 1.0);
    material.setSpecular(white);
    assert(material.hasSpecular());
    assert(material.getSpecular() == white);
    
    material.clearSpecular();
    assert(!material.hasSpecular());
    
    // Test emissive operations
    RGBA_Color blue(0.0, 0.0, 1.0, 1.0);
    material.setEmissive(blue);
    assert(material.hasEmissive());
    assert(material.getEmissive() == blue);
    
    material.clearEmissive();
    assert(!material.hasEmissive());
}

void testMaterialTypeQueries() {
    Material material;
    
    // Test non-emissive material
    assert(!material.isEmissive());
    
    // Test emissive material
    material.setEmissive(RGBA_Color(1.0, 0.5, 0.0, 1.0));
    assert(material.isEmissive());
    
    // Test non-emissive with black emission
    material.setEmissive(RGBA_Color(0.0, 0.0, 0.0, 1.0));
    assert(!material.isEmissive());
    
    // Test transparency
    assert(!material.isTransparent());
    material.setTransmission(0.5);
    assert(material.isTransparent());
    
    material.setTransmission(0.0);
    material.setAlbedo(RGBA_Color(1.0, 0.0, 0.0, 0.5)); // Semi-transparent
    assert(material.isTransparent());
    
    // Test metallic
    assert(!material.isMetallic());
    material.setMetalness(0.8);
    assert(material.isMetallic());
    
    // Test reflective
    material.setMetalness(0.0);
    assert(!material.isReflective());
    material.setSpecular(RGBA_Color(1.0, 1.0, 1.0, 1.0));
    assert(material.isReflective());
}

void testMaterialUtilityMethods() {
    Material material(RGBA_Color(0.8, 0.4, 0.2, 1.0));
    material.setSpecular(RGBA_Color(1.0, 1.0, 1.0, 1.0));
    
    RGBA_Color lightColor(1.0, 0.8, 0.6, 1.0);
    
    // Test diffuse calculation
    RGBA_Color diffuse = material.calculateDiffuseColor(lightColor);
    assert(isEqual(diffuse.r(), 0.8 * 1.0));
    assert(isEqual(diffuse.g(), 0.4 * 0.8));
    assert(isEqual(diffuse.b(), 0.2 * 0.6));
    
    // Test specular calculation
    material.setRoughness(0.2); // Low roughness for high specular
    RGBA_Color specular = material.calculateSpecularColor(lightColor, 1.0);
    double expectedIntensity = 1.0 * (1.0 - 0.2); // intensity * (1 - roughness)
    assert(isEqual(specular.r(), 1.0 * 1.0 * expectedIntensity));
    assert(isEqual(specular.g(), 1.0 * 0.8 * expectedIntensity));
    assert(isEqual(specular.b(), 1.0 * 0.6 * expectedIntensity));
}

void testMaterialFactoryMethods() {
    // Test createDiffuse
    RGBA_Color red(1.0, 0.0, 0.0, 1.0);
    Material diffuse = Material::createDiffuse(red);
    assert(diffuse.getAlbedo() == red);
    assert(isEqual(diffuse.getRoughness(), 1.0));
    assert(isEqual(diffuse.getMetalness(), 0.0));
    
    // Test createMetal
    Material metal = Material::createMetal(red, 0.2);
    assert(metal.getAlbedo() == red);
    assert(metal.hasSpecular());
    assert(metal.getSpecular() == red);
    assert(isEqual(metal.getRoughness(), 0.2));
    assert(isEqual(metal.getMetalness(), 1.0));
    
    // Test createGlass
    Material glass = Material::createGlass(RGBA_Color(0.9, 0.9, 1.0, 0.1), 1.5);
    assert(isEqual(glass.getRefractiveIndex(), 1.5));
    assert(isEqual(glass.getTransmission(), 0.9));
    assert(isEqual(glass.getRoughness(), 0.0));
    assert(isEqual(glass.getMetalness(), 0.0));
    
    // Test createEmissive
    Material emissive = Material::createEmissive(RGBA_Color(1.0, 0.5, 0.0, 1.0), 2.0);
    assert(emissive.hasEmissive());
    assert(isEqual(emissive.getEmissive().r(), 1.0));
    assert(isEqual(emissive.getEmissive().g(), 0.5));
    assert(emissive.isEmissive());
    
    // Test createPlastic
    Material plastic = Material::createPlastic(red, 0.6);
    assert(plastic.getAlbedo() == red);
    assert(plastic.hasSpecular());
    assert(plastic.getSpecular() == RGBA_Color(1.0, 1.0, 1.0, 1.0));
    assert(isEqual(plastic.getRoughness(), 0.6));
    assert(isEqual(plastic.getMetalness(), 0.0));
}

void testMaterialCopySemantics() {
    // Create original material
    Material original(RGBA_Color(1.0, 0.0, 0.0, 1.0));
    original.setSpecular(RGBA_Color(1.0, 1.0, 1.0, 1.0));
    original.setEmissive(RGBA_Color(0.0, 0.0, 1.0, 1.0));
    original.setRoughness(0.3);
    original.setMetalness(0.7);
    
    // Test copy constructor
    Material copied(original);
    assert(copied == original);
    assert(copied.hasAlbedo() && copied.getAlbedo() == original.getAlbedo());
    assert(copied.hasSpecular() && copied.getSpecular() == original.getSpecular());
    assert(copied.hasEmissive() && copied.getEmissive() == original.getEmissive());
    assert(isEqual(copied.getRoughness(), original.getRoughness()));
    assert(isEqual(copied.getMetalness(), original.getMetalness()));
    
    // Test copy assignment
    Material assigned;
    assigned = original;
    assert(assigned == original);
    
    // Test move semantics
    Material moved(std::move(copied));
    assert(moved == original);
    
    Material moveAssigned;
    moveAssigned = std::move(assigned);
    assert(moveAssigned == original);
}

void testMaterialValidation() {
    Material material;
    
    // Test invalid property values
    bool exceptionCaught = false;
    try {
        material.setAbsorption(-0.5);
    } catch (const std::invalid_argument&) {
        exceptionCaught = true;
    }
    assert(exceptionCaught);
    
    exceptionCaught = false;
    try {
        material.setRoughness(1.5);
    } catch (const std::invalid_argument&) {
        exceptionCaught = true;
    }
    assert(exceptionCaught);
    
    exceptionCaught = false;
    try {
        material.setRefractiveIndex(0.5); // Less than 1.0
    } catch (const std::invalid_argument&) {
        exceptionCaught = true;
    }
    assert(exceptionCaught);
    
    // Test accessing non-existent colors
    Material emptyMaterial;
    emptyMaterial.clearAlbedo();
    
    exceptionCaught = false;
    try {
        emptyMaterial.getSpecular();
    } catch (const std::runtime_error&) {
        exceptionCaught = true;
    }
    assert(exceptionCaught);
}

void testMaterialEquality() {
    RGBA_Color red(1.0, 0.0, 0.0, 1.0);
    RGBA_Color white(1.0, 1.0, 1.0, 1.0);
    
    Material mat1(red);
    mat1.setSpecular(white);
    mat1.setRoughness(0.5);
    
    Material mat2(red);
    mat2.setSpecular(white);
    mat2.setRoughness(0.5);
    
    // Test equality
    assert(mat1 == mat2);
    assert(!(mat1 != mat2));
    
    // Test inequality with different properties
    mat2.setRoughness(0.6);
    assert(mat1 != mat2);
    assert(!(mat1 == mat2));
    
    // Test inequality with different colors
    mat2.setRoughness(0.5);
    mat2.setAlbedo(RGBA_Color(0.0, 1.0, 0.0, 1.0));
    assert(mat1 != mat2);
    
    // Test inequality with missing colors
    mat2 = mat1;
    mat2.clearSpecular();
    assert(mat1 != mat2);
}