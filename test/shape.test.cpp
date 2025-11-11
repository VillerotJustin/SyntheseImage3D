#include <iostream>
#include <cassert>
#include <stdexcept>
#include <memory>

// Include the Shape header
#include "../Lib/Rendering/Shape.hpp"

#include "../Lib/Geometry/Box.h"
#include "../Lib/Geometry/Circle.h"
#include "../Lib/Geometry/Plane.h"
#include "../Lib/Geometry/Rectangle.h"
#include "../Lib/Geometry/Sphere.h"
#include "../Lib/Geometry/Vector3D.h"

using namespace rendering;

// Helper function for floating-point comparison
bool isEqual(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
}

// Test function declarations
void testShapeConstructors();
void testShapeAssignmentOperators();
void testGeometryManagement();
void testColorManagement();
void testMaterialManagement();
void testConvenienceMethods();
void testErrorHandling();
void testTypeAliases();

int main() {
    std::cout << "=== Shape Test Suite ===" << std::endl;
    
    try {
        testShapeConstructors();
        std::cout << "✓ Shape constructors test passed" << std::endl;
        
        testShapeAssignmentOperators();
        std::cout << "✓ Shape assignment operators test passed" << std::endl;
        
        testGeometryManagement();
        std::cout << "✓ Geometry management test passed" << std::endl;
        
        testColorManagement();
        std::cout << "✓ Color management test passed" << std::endl;
        
        testMaterialManagement();
        std::cout << "✓ Material management test passed" << std::endl;
        
        testConvenienceMethods();
        std::cout << "✓ Convenience methods test passed" << std::endl;
        
        testErrorHandling();
        std::cout << "✓ Error handling test passed" << std::endl;
        
        testTypeAliases();
        std::cout << "✓ Type aliases test passed" << std::endl;
        
        std::cout << "\n🎉 All Shape tests passed successfully! 🎉" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}

void testShapeConstructors() {
    std::cout << "Testing Shape constructors..." << std::endl;
    
    // Test default constructor with Box
    Shape<::geometry::Box> emptyBoxShape;
    assert(!emptyBoxShape.hasGeometry());
    assert(!emptyBoxShape.hasColor());
    assert(!emptyBoxShape.isComplete());
    
    // Create test objects for construction
    ::geometry::Vector3D origin(0.0, 0.0, 0.0);
    ::geometry::Vector3D normal(0.0, 0.0, 1.0);
    // ::geometry::Vector3D topRight = origin + ::geometry::Vector3D(2.0, 0.0, 0.0);
    // ::geometry::Vector3D bottomLeft = origin + ::geometry::Vector3D(0.0, 3.0, 0.0);
    ::geometry::Box testBox(origin, 2.0, 3.0, 4.0, normal);
    RGBA_Color red(1.0f, 0.0f, 0.0f, 1.0f);

    // Test constructor with geometry only (Box)
    Shape<::geometry::Box> boxOnly(testBox);
    assert(boxOnly.hasGeometry());
    assert(!boxOnly.hasColor());
    assert(!boxOnly.isComplete());
    assert(boxOnly.getGeometry() != nullptr);

    // Test constructor with geometry and color (Box)
    Shape<::geometry::Box> completeBoxShape(testBox, red);
    assert(completeBoxShape.hasGeometry());
    assert(completeBoxShape.hasColor());
    assert(completeBoxShape.isComplete());
    assert(completeBoxShape.getGeometry() != nullptr);
    assert(completeBoxShape.getColor() != nullptr);
    assert(*completeBoxShape.getColor() == red);

    // Test constructor with geometry and Material
    Material testMaterial = Material::createMetal(red, 0.2);
    Shape<::geometry::Box> materialBoxShape(testBox, testMaterial);
    assert(materialBoxShape.hasGeometry());
    assert(materialBoxShape.hasMaterial());
    assert(materialBoxShape.hasColor()); // Should have color from material albedo
    assert(materialBoxShape.isComplete());
    assert(materialBoxShape.getMaterial()->isMetallic());
    assert(isEqual(materialBoxShape.getMaterial()->getRoughness(), 0.2));

    // Test constructor with unique_ptr geometry and color (Box)
    auto boxPtr = std::make_unique<::geometry::Box>(testBox);
    Shape<::geometry::Box> ptrBoxShape(std::move(boxPtr), red);
    assert(ptrBoxShape.hasGeometry());
    assert(ptrBoxShape.hasColor());
    assert(ptrBoxShape.isComplete());

    // Test constructor with unique_ptr geometry and Material
    auto boxPtr2 = std::make_unique<::geometry::Box>(testBox);
    Material glassMaterial = Material::createGlass(RGBA_Color(0.9f, 0.9f, 1.0f, 0.1f), 1.5);
    Shape<::geometry::Box> ptrMaterialBoxShape(std::move(boxPtr2), glassMaterial);
    assert(ptrMaterialBoxShape.hasGeometry());
    assert(ptrMaterialBoxShape.hasMaterial());
    assert(ptrMaterialBoxShape.isComplete());
    assert(isEqual(ptrMaterialBoxShape.getMaterial()->getRefractiveIndex(), 1.5));

    // Test copy constructor (Box)
    Shape<::geometry::Box> copiedBoxShape(completeBoxShape);
    assert(copiedBoxShape.hasGeometry());
    assert(copiedBoxShape.hasColor());
    assert(copiedBoxShape.isComplete());
    assert(copiedBoxShape.getGeometry() != completeBoxShape.getGeometry()); // Different objects
    assert(*copiedBoxShape.getColor() == *completeBoxShape.getColor()); // Same values

    // Test move constructor (Box)
    Shape<::geometry::Box> originalForMove(testBox, red);
    Shape<::geometry::Box> movedBoxShape(std::move(originalForMove));
    assert(movedBoxShape.hasGeometry());
    assert(movedBoxShape.hasColor());
    assert(movedBoxShape.isComplete());

    // Test with Sphere
    ::geometry::Vector3D center(1.0, 2.0, 3.0);
    ::geometry::Sphere testSphere(center, 5.0);
    RGBA_Color blue(0.0f, 0.0f, 1.0f, 1.0f);

    Shape<::geometry::Sphere> sphereShape(testSphere, blue);
    assert(sphereShape.hasGeometry());
    assert(sphereShape.hasColor());
    assert(sphereShape.isComplete());

    // Test with Circle
    ::geometry::Circle testCircle(center, 3.0, normal);
    Shape<::geometry::Circle> circleShape(testCircle, blue);
    assert(circleShape.hasGeometry());
    assert(circleShape.hasColor());
    assert(circleShape.isComplete());

    // Test with Rectangle
    ::geometry::Vector3D rectTopLeft = origin;
    ::geometry::Vector3D rectTopRight = rectTopLeft + ::geometry::Vector3D(5.0, 0.0, 0.0);
    ::geometry::Vector3D rectBottomLeft = rectTopLeft + ::geometry::Vector3D(0.0, 3.0, 0.0);
    ::geometry::Rectangle testRect(rectTopLeft, rectTopRight, rectBottomLeft);
    Shape<::geometry::Rectangle> rectShape(testRect, blue);
    assert(rectShape.hasGeometry());
    assert(rectShape.hasColor());
    assert(rectShape.isComplete());

    // Test with Plane
    ::geometry::Plane testPlane(origin, normal);
    Shape<::geometry::Plane> planeShape(testPlane, blue);
    assert(planeShape.hasGeometry());
    assert(planeShape.hasColor());
    assert(planeShape.isComplete());
}

void testShapeAssignmentOperators() {
    std::cout << "Testing Shape assignment operators..." << std::endl;
    
    ::geometry::Vector3D origin(5.0, 10.0, 15.0);
    ::geometry::Vector3D normal(1.0, 0.0, 0.0);
    ::geometry::Box testBox(origin, 10.0, 5.0, 7.0, normal);
    RGBA_Color blue(0.0f, 0.0f, 1.0f, 1.0f);
    
    Shape<::geometry::Box> source(testBox, blue);
    Shape<::geometry::Box> target;
    
    // Test copy assignment
    target = source;
    assert(target.hasGeometry());
    assert(target.hasColor());
    assert(target.isComplete());
    assert(target.getGeometry() != source.getGeometry()); // Different objects
    assert(*target.getColor() == *source.getColor()); // Same values
    
    // Test self-assignment
    target = target;
    assert(target.hasGeometry());
    assert(target.hasColor());
    assert(target.isComplete());
    
    // Test move assignment
    Shape<::geometry::Box> moveTarget;
    Shape<::geometry::Box> moveSource(testBox, blue);
    moveTarget = std::move(moveSource);
    assert(moveTarget.hasGeometry());
    assert(moveTarget.hasColor());
    assert(moveTarget.isComplete());
    
    // Test with Sphere
    ::geometry::Vector3D center(2.0, 3.0, 4.0);
    ::geometry::Sphere testSphere(center, 8.0);
    Shape<::geometry::Sphere> sphereSource(testSphere, blue);
    Shape<::geometry::Sphere> sphereTarget;
    
    sphereTarget = sphereSource;
    assert(sphereTarget.hasGeometry());
    assert(sphereTarget.hasColor());
    assert(sphereTarget.isComplete());
}

void testGeometryManagement() {
    std::cout << "Testing geometry management..." << std::endl;
    
    Shape<::geometry::Circle> shape;
    assert(!shape.hasGeometry());
    assert(shape.getGeometry() == nullptr);
    
    // Set geometry
    ::geometry::Vector3D center(1.0, 2.0, 3.0);
    ::geometry::Vector3D normal(0.0, 1.0, 0.0);
    ::geometry::Circle testCircle(center, 5.0, normal);
    shape.setGeometry(testCircle);
    assert(shape.hasGeometry());
    assert(shape.getGeometry() != nullptr);
    assert(shape.getGeometry()->getRadius() == 5.0);
    
    // Set geometry with unique_ptr
    auto circlePtr = std::make_unique<::geometry::Circle>(center, 3.0, normal);
    shape.setGeometry(std::move(circlePtr));
    assert(shape.hasGeometry());
    assert(shape.getGeometry()->getRadius() == 3.0);
    
    // Clear geometry
    shape.clearGeometry();
    assert(!shape.hasGeometry());
    assert(shape.getGeometry() == nullptr);
}

void testColorManagement() {
    std::cout << "Testing color management (backward compatibility)..." << std::endl;
    
    Shape<::geometry::Plane> shape;
    assert(!shape.hasColor());
    assert(shape.getColor() == nullptr);
    
    // Set color
    RGBA_Color green(0.0f, 1.0f, 0.0f, 1.0f);
    shape.setColor(green);
    assert(shape.hasColor());
    assert(shape.getColor() != nullptr);
    assert(*shape.getColor() == green);
    
    // Test that setting color creates a material
    assert(shape.hasMaterial());
    assert(shape.getMaterial() != nullptr);
    assert(shape.getMaterial()->hasAlbedo());
    assert(shape.getMaterial()->getAlbedo() == green);

    // Clear material (removes color too)
    shape.clearMaterial();
    assert(!shape.hasColor());
    assert(shape.getColor() == nullptr);
    assert(!shape.hasMaterial());
}

void testMaterialManagement() {
    std::cout << "Testing material management..." << std::endl;
    
    Shape<::geometry::Sphere> shape;
    assert(!shape.hasMaterial());
    assert(shape.getMaterial() == nullptr);
    
    // Create a test material
    RGBA_Color red(1.0f, 0.0f, 0.0f, 1.0f);
    RGBA_Color white(1.0f, 1.0f, 1.0f, 1.0f);
    Material testMaterial(red, white);  // albedo and specular
    testMaterial.setRoughness(0.3);
    testMaterial.setMetalness(0.8);
    
    // Set material
    shape.setMaterial(testMaterial);
    assert(shape.hasMaterial());
    assert(shape.getMaterial() != nullptr);
    
    // Verify material properties
    const Material* mat = shape.getMaterial();
    assert(mat->hasAlbedo());
    assert(mat->getAlbedo() == red);
    assert(mat->hasSpecular());
    assert(mat->getSpecular() == white);
    assert(isEqual(mat->getRoughness(), 0.3));
    assert(isEqual(mat->getMetalness(), 0.8));
    
    // Test backward compatibility - material albedo should be accessible via getColor()
    assert(shape.hasColor());
    assert(shape.getColor() != nullptr);
    assert(*shape.getColor() == red);
    
    // Test constructor with material
    ::geometry::Vector3D center(1.0, 2.0, 3.0);
    ::geometry::Sphere testSphere(center, 5.0);
    Material glassMaterial = Material::createGlass(RGBA_Color(0.9f, 0.9f, 1.0f, 0.1f), 1.5);
    
    Shape<::geometry::Sphere> glassShape(testSphere, glassMaterial);
    assert(glassShape.hasMaterial());
    assert(glassShape.hasGeometry());
    assert(glassShape.isComplete());
    
    const Material* glassMat = glassShape.getMaterial();
    assert(isEqual(glassMat->getRefractiveIndex(), 1.5));
    assert(isEqual(glassMat->getTransmission(), 0.9));
    assert(isEqual(glassMat->getRoughness(), 0.0));
    
    // Test emissive material
    Material emissiveMaterial = Material::createEmissive(RGBA_Color(1.0f, 0.5f, 0.0f, 1.0f), 2.0);
    shape.setMaterial(emissiveMaterial);
    
    const Material* emissiveMat = shape.getMaterial();
    assert(emissiveMat->hasEmissive());
    assert(emissiveMat->isEmissive());
    
    // Test metallic material
    Material metalMaterial = Material::createMetal(RGBA_Color(0.8f, 0.8f, 0.9f, 1.0f), 0.1);
    shape.setMaterial(metalMaterial);
    
    const Material* metalMat = shape.getMaterial();
    assert(metalMat->isMetallic());
    assert(metalMat->isReflective());
    assert(isEqual(metalMat->getMetalness(), 1.0));
    
    // Clear material
    shape.clearMaterial();
    assert(!shape.hasMaterial());
    assert(shape.getMaterial() == nullptr);
    assert(!shape.hasColor());  // Should also clear backward compatibility color
}

void testConvenienceMethods() {
    std::cout << "Testing convenience methods..." << std::endl;
    
    Shape<::geometry::Rectangle> shape;
    
    // Test isComplete
    assert(!shape.isComplete());
    
    ::geometry::Vector3D origin(0.0, 0.0, 0.0);
    ::geometry::Vector3D normal(0.0, 0.0, 1.0);
    ::geometry::Vector3D rectTopLeft = origin;
    ::geometry::Vector3D rectTopRight = rectTopLeft + ::geometry::Vector3D(5.0, 0.0, 0.0);
    ::geometry::Vector3D rectBottomLeft = rectTopLeft + ::geometry::Vector3D(0.0, 3.0, 0.0);
    ::geometry::Rectangle testRect(rectTopLeft, rectTopRight, rectBottomLeft);
    shape.setGeometry(testRect);
    assert(!shape.isComplete()); // Has geometry but no color
    
    RGBA_Color yellow(1.0f, 1.0f, 0.0f, 1.0f);
    shape.setColor(yellow);
    assert(shape.isComplete()); // Has both geometry and color
    
    // Test isComplete with material
    shape.clearMaterial(); // Remove the material created by setColor
    assert(!shape.isComplete()); // Should not be complete without material
    
    Material testMaterial = Material::createPlastic(yellow, 0.7);
    shape.setMaterial(testMaterial);
    assert(shape.isComplete()); // Has both geometry and material
    
    // Test isValid (basic check)
    assert(shape.isValid()); // Has geometry
    
    shape.clearGeometry();
    assert(!shape.isValid()); // No geometry
    
    // Test operator* and operator->
    shape.setGeometry(testRect);
    
    // Test const access
    const Shape<::geometry::Rectangle>& constShape = shape;
    const ::geometry::Rectangle& geomRef = *constShape;
    assert(geomRef.getLength() == 5.0);
    
    const ::geometry::Rectangle* geomPtr = constShape.operator->();
    assert(geomPtr->getLength() == 5.0);
    
    // Test non-const access
    ::geometry::Rectangle& mutableGeomRef = *shape;
    assert(mutableGeomRef.getLength() == 5.0);
    
    ::geometry::Rectangle* mutableGeomPtr = shape.operator->();
    assert(mutableGeomPtr->getLength() == 5.0);
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    Shape<::geometry::Sphere> emptyShape;
    
    // Test accessing geometry when none exists
    bool threwException = false;
    try {
        const ::geometry::Sphere& geom = *emptyShape;
        (void)geom; // Suppress unused variable warning
    } catch (const std::runtime_error&) {
        threwException = true;
    }
    assert(threwException);
    
    // Test operator-> when no geometry exists
    threwException = false;
    try {
        const ::geometry::Sphere* geom = emptyShape.operator->();
        (void)geom; // Suppress unused variable warning
    } catch (const std::runtime_error&) {
        threwException = true;
    }
    assert(threwException);
    
    // Test non-const versions
    threwException = false;
    try {
        ::geometry::Sphere& geom = *emptyShape;
        (void)geom; // Suppress unused variable warning
    } catch (const std::runtime_error&) {
        threwException = true;
    }
    assert(threwException);
    
    threwException = false;
    try {
        ::geometry::Sphere* geom = emptyShape.operator->();
        (void)geom; // Suppress unused variable warning
    } catch (const std::runtime_error&) {
        threwException = true;
    }
    assert(threwException);
}

void testTypeAliases() {
    std::cout << "Testing type aliases..." << std::endl;
    
    // Test with Box
    ::geometry::Vector3D origin(0.0, 0.0, 0.0);
    ::geometry::Vector3D normal(0.0, 0.0, 1.0);
    ::geometry::Box testBox(origin, 3.0, 4.0, 5.0, normal);
    Shape<::geometry::Box> boxShape;
    assert(!boxShape.hasGeometry());
    assert(!boxShape.hasColor());
    
    RGBA_Color purple(0.5f, 0.0f, 0.5f, 1.0f);
    Shape<::geometry::Box> coloredBox(testBox, purple);
    assert(coloredBox.hasGeometry());
    assert(coloredBox.hasColor());
    assert(coloredBox.getGeometry()->getWidth() == 3.0);
    assert(coloredBox.getGeometry()->getHeight() == 4.0);
    assert(coloredBox.getGeometry()->getDepth() == 5.0);
    assert(*coloredBox.getColor() == purple);
    
    // Test with Circle  
    ::geometry::Vector3D center(1.0, 2.0, 3.0);
    ::geometry::Circle testCircle(center, 7.0, normal);
    Shape<::geometry::Circle> coloredCircle(testCircle, purple);
    assert(coloredCircle.hasGeometry());
    assert(coloredCircle.hasColor());
    assert(coloredCircle.getGeometry()->getRadius() == 7.0);
    
    // Test with Sphere
    ::geometry::Sphere testSphere(center, 4.0);
    Shape<::geometry::Sphere> coloredSphere(testSphere, purple);
    assert(coloredSphere.hasGeometry());
    assert(coloredSphere.hasColor());
    assert(coloredSphere.getGeometry()->getRadius() == 4.0);
    
    // Test with Rectangle
    ::geometry::Vector3D rectTopLeft = origin;
    ::geometry::Vector3D rectTopRight = rectTopLeft + ::geometry::Vector3D(6.0, 0.0, 0.0);
    ::geometry::Vector3D rectBottomLeft = rectTopLeft + ::geometry::Vector3D(0.0, 8.0, 0.0);
    ::geometry::Rectangle testRect(rectTopLeft, rectTopRight, rectBottomLeft);
    Shape<::geometry::Rectangle> coloredRect(testRect, purple);
    assert(coloredRect.hasGeometry());
    assert(coloredRect.hasColor());
    assert(coloredRect.getGeometry()->getLength() == 6.0);
    assert(coloredRect.getGeometry()->getWidth() == 8.0);
    
    // Test with Plane
    ::geometry::Plane testPlane(origin, normal);
    Shape<::geometry::Plane> coloredPlane(testPlane, purple);
    assert(coloredPlane.hasGeometry());
    assert(coloredPlane.hasColor());
    
    std::cout << "  ✓ All geometry types (Box, Circle, Sphere, Rectangle, Plane) work with Shape template" << std::endl;
}
