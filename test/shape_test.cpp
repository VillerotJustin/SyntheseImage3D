#include <iostream>
#include <cassert>
#include <stdexcept>
#include <memory>

// Include the Shape header
#include "../Lib/Rendering/Shape.hpp"

// For now, we'll create a simple mock geometry class to test Shape
// We'll include actual geometry classes once dependency issues are resolved
#include "../Lib/Vector3.h"

using namespace rendering;
using namespace math;

// Simple mock geometry class for testing Shape functionality
class MockGeometry {
public:
    MockGeometry() : value_(0.0) {}
    MockGeometry(double value) : value_(value) {}
    
    double getValue() const { return value_; }
    void setValue(double value) { value_ = value; }
    
    bool isValid() const { return value_ >= 0.0; }
    
private:
    double value_;
};

// Specialize the is_allowed_geometry trait for our mock class
namespace rendering {
    template<> struct is_allowed_geometry<MockGeometry> : std::true_type {};
}

// Helper function for floating-point comparison
bool isEqual(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
}

// Test function declarations
void testShapeConstructors();
void testShapeAssignmentOperators();
void testGeometryManagement();
void testColorManagement();
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
    
    // Test default constructor
    Shape<MockGeometry> emptyShape;
    assert(!emptyShape.hasGeometry());
    assert(!emptyShape.hasColor());
    assert(!emptyShape.isComplete());
    
    // Create test objects for construction
    MockGeometry geom(5.0);
    RGBA_Color red(1.0f, 0.0f, 0.0f, 1.0f);
    
    // Test constructor with geometry only
    Shape<MockGeometry> geomOnly(geom);
    assert(geomOnly.hasGeometry());
    assert(!geomOnly.hasColor());
    assert(!geomOnly.isComplete());
    assert(geomOnly.getGeometry() != nullptr);
    
    // Test constructor with geometry and color
    Shape<MockGeometry> completeShape(geom, red);
    assert(completeShape.hasGeometry());
    assert(completeShape.hasColor());
    assert(completeShape.isComplete());
    assert(completeShape.getGeometry() != nullptr);
    assert(completeShape.getColor() != nullptr);
    assert(*completeShape.getColor() == red);
    
    // Test constructor with unique_ptr geometry and color
    auto geomPtr = std::make_unique<MockGeometry>(geom);
    Shape<MockGeometry> ptrShape(std::move(geomPtr), red);
    assert(ptrShape.hasGeometry());
    assert(ptrShape.hasColor());
    assert(ptrShape.isComplete());
    
    // Test copy constructor
    Shape<MockGeometry> copiedShape(completeShape);
    assert(copiedShape.hasGeometry());
    assert(copiedShape.hasColor());
    assert(copiedShape.isComplete());
    assert(copiedShape.getGeometry() != completeShape.getGeometry()); // Different objects
    assert(*copiedShape.getColor() == *completeShape.getColor()); // Same values
    
    // Test move constructor
    Shape<MockGeometry> originalForMove(geom, red);
    Shape<MockGeometry> movedShape(std::move(originalForMove));
    assert(movedShape.hasGeometry());
    assert(movedShape.hasColor());
    assert(movedShape.isComplete());
    // Original should be moved-from (undefined state, but shouldn't crash)
}

void testShapeAssignmentOperators() {
    std::cout << "Testing Shape assignment operators..." << std::endl;
    
    MockGeometry geom(10.0);
    RGBA_Color blue(0.0f, 0.0f, 1.0f, 1.0f);
    
    Shape<MockGeometry> source(geom, blue);
    Shape<MockGeometry> target;
    
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
    Shape<MockGeometry> moveTarget;
    Shape<MockGeometry> moveSource(geom, blue);
    moveTarget = std::move(moveSource);
    assert(moveTarget.hasGeometry());
    assert(moveTarget.hasColor());
    assert(moveTarget.isComplete());
}

void testGeometryManagement() {
    std::cout << "Testing geometry management..." << std::endl;
    
    Shape<MockGeometry> shape;
    assert(!shape.hasGeometry());
    assert(shape.getGeometry() == nullptr);
    
    // Set geometry
    MockGeometry geom(5.0);
    shape.setGeometry(geom);
    assert(shape.hasGeometry());
    assert(shape.getGeometry() != nullptr);
    assert(shape.getGeometry()->getValue() == 5.0);
    
    // Set geometry with unique_ptr
    auto geomPtr = std::make_unique<MockGeometry>(3.0);
    shape.setGeometry(std::move(geomPtr));
    assert(shape.hasGeometry());
    assert(shape.getGeometry()->getValue() == 3.0);
    
    // Clear geometry
    shape.clearGeometry();
    assert(!shape.hasGeometry());
    assert(shape.getGeometry() == nullptr);
}

void testColorManagement() {
    std::cout << "Testing color management..." << std::endl;
    
    Shape<MockGeometry> shape;
    assert(!shape.hasColor());
    assert(shape.getColor() == nullptr);
    
    // Set color
    RGBA_Color green(0.0f, 1.0f, 0.0f, 1.0f);
    shape.setColor(green);
    assert(shape.hasColor());
    assert(shape.getColor() != nullptr);
    assert(*shape.getColor() == green);
    
    // Test color modification
    shape.getColor()->setRed(0.5f);
    assert(shape.getColor()->getRed() == 0.5f);
    
    // Clear color
    shape.clearColor();
    assert(!shape.hasColor());
    assert(shape.getColor() == nullptr);
}

void testConvenienceMethods() {
    std::cout << "Testing convenience methods..." << std::endl;
    
    Shape<MockGeometry> shape;
    
    // Test isComplete
    assert(!shape.isComplete());
    
    MockGeometry geom(5.0);
    shape.setGeometry(geom);
    assert(!shape.isComplete()); // Has geometry but no color
    
    RGBA_Color yellow(1.0f, 1.0f, 0.0f, 1.0f);
    shape.setColor(yellow);
    assert(shape.isComplete()); // Has both geometry and color
    
    // Test isValid (basic check)
    assert(shape.isValid()); // Has geometry
    
    shape.clearGeometry();
    assert(!shape.isValid()); // No geometry
    
    // Test operator* and operator->
    shape.setGeometry(geom);
    
    // Test const access
    const Shape<MockGeometry>& constShape = shape;
    const MockGeometry& geomRef = *constShape;
    assert(geomRef.getValue() == 5.0);
    
    const MockGeometry* geomPtr = constShape.operator->();
    assert(geomPtr->getValue() == 5.0);
    
    // Test non-const access
    MockGeometry& mutableGeomRef = *shape;
    assert(mutableGeomRef.getValue() == 5.0);
    
    MockGeometry* mutableGeomPtr = shape.operator->();
    assert(mutableGeomPtr->getValue() == 5.0);
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    Shape<MockGeometry> emptyShape;
    
    // Test accessing geometry when none exists
    bool threwException = false;
    try {
        const MockGeometry& geom = *emptyShape;
        (void)geom; // Suppress unused variable warning
    } catch (const std::runtime_error&) {
        threwException = true;
    }
    assert(threwException);
    
    // Test operator-> when no geometry exists
    threwException = false;
    try {
        const MockGeometry* geom = emptyShape.operator->();
        (void)geom; // Suppress unused variable warning
    } catch (const std::runtime_error&) {
        threwException = true;
    }
    assert(threwException);
    
    // Test non-const versions
    threwException = false;
    try {
        MockGeometry& geom = *emptyShape;
        (void)geom; // Suppress unused variable warning
    } catch (const std::runtime_error&) {
        threwException = true;
    }
    assert(threwException);
    
    threwException = false;
    try {
        MockGeometry* geom = emptyShape.operator->();
        (void)geom; // Suppress unused variable warning
    } catch (const std::runtime_error&) {
        threwException = true;
    }
    assert(threwException);
}

void testTypeAliases() {
    std::cout << "Testing type aliases..." << std::endl;
    
    // Note: Type aliases are for actual geometry classes, which have dependency issues
    // For now, we'll test that the Shape template works with our MockGeometry
    
    Shape<MockGeometry> mockShape;
    assert(!mockShape.hasGeometry());
    assert(!mockShape.hasColor());
    
    // Test with actual geometry and color
    RGBA_Color purple(0.5f, 0.0f, 0.5f, 1.0f);
    MockGeometry geom(42.0);
    
    Shape<MockGeometry> coloredMock(geom, purple);
    assert(coloredMock.hasGeometry());
    assert(coloredMock.hasColor());
    assert(coloredMock.getGeometry()->getValue() == 42.0);
    assert(*coloredMock.getColor() == purple);
    
    std::cout << "  Note: Actual geometry type aliases need Vector3D compatibility fixes" << std::endl;
}
