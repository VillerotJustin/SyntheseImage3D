#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include "../Lib/Geometry/Circle.h"
#include "../Lib/Geometry/Vector3D.h"  // Assuming Vector3D will be created
#include "../Lib/Math/math_common.h"

using namespace geometry;

// Helper function for floating-point comparison
bool isEqual(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
}

// Helper function to compare Vector3D (assuming it has comparison operators)
bool isEqual(const Vector3D& a, const Vector3D& b, double epsilon = 1e-9) {
    return (a - b).length() < epsilon;
}

// Test function declarations
void testCircleConstructor();
void testCircleBasicProperties();
void testCircleGeometry();
void testCirclePointOperations();
void testCircleSetters();
void testCircleValidation();
void testCircleParametricPoints();

int main() {
    std::cout << "=== Circle Test Suite ===" << std::endl;
    
    try {
        testCircleConstructor();
        std::cout << "✓ Circle constructor test passed" << std::endl;
        
        testCircleBasicProperties();
        std::cout << "✓ Circle basic properties test passed" << std::endl;
        
        testCircleGeometry();
        std::cout << "✓ Circle geometry calculations test passed" << std::endl;
        
        testCirclePointOperations();
        std::cout << "✓ Circle point operations test passed" << std::endl;
        
        testCircleSetters();
        std::cout << "✓ Circle setters test passed" << std::endl;
        
        testCircleValidation();
        std::cout << "✓ Circle validation test passed" << std::endl;
        
        testCircleParametricPoints();
        std::cout << "✓ Circle parametric points test passed" << std::endl;
        
        std::cout << "\n🎉 All Circle tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void testCircleConstructor() {
    // Test basic constructor
    Vector3D center(0, 0, 0);
    Vector3D normal(0, 0, 1);
    double radius = 5.0;
    
    Circle circle(center, radius, normal);
    
    // Test getters
    assert(isEqual(circle.getCenter(), center));
    assert(isEqual(circle.getRadius(), radius));
    assert(isEqual(circle.getNormal(), normal.normal())); // Should be normal
}

void testCircleBasicProperties() {
    Vector3D center(1, 2, 3);
    Vector3D normal(1, 1, 1); // Will be normal to (√3/3, √3/3, √3/3)
    double radius = 4.0;
    
    Circle circle(center, radius, normal);
    
    // Test circumference: 2πr
    double expectedCircumference = 2 * math::pi * radius;
    assert(isEqual(circle.getCircumference(), expectedCircumference));
    
    // Test area: πr²
    double expectedArea = math::pi * radius * radius;
    assert(isEqual(circle.getArea(), expectedArea));
    
    // Test that normal is normal
    assert(isEqual(circle.getNormal().length(), 1.0));
}

void testCircleGeometry() {
    // Create a circle in XY plane (normal pointing in Z direction)
    Vector3D center(0, 0, 0);
    Vector3D normal(0, 0, 1);
    double radius = 3.0;
    
    Circle circle(center, radius, normal);
    
    // Test various circumferences and areas
    assert(isEqual(circle.getCircumference(), 2 * math::pi * 3.0));
    assert(isEqual(circle.getArea(), math::pi * 9.0));
    
    // Test with different radius
    Vector3D center2(5, 5, 5);
    Vector3D normal2(1, 0, 0);
    double radius2 = 2.5;
    
    Circle circle2(center2, radius2, normal2);
    assert(isEqual(circle2.getCircumference(), 2 * math::pi * 2.5));
    assert(isEqual(circle2.getArea(), math::pi * 6.25));
}

void testCirclePointOperations() {
    // Create circle in XY plane
    Vector3D center(0, 0, 0);
    Vector3D normal(0, 0, 1);
    double radius = 2.0;
    
    Circle circle(center, radius, normal);
    
    // Test points on the circle (in XY plane at distance 2 from origin)
    Vector3D pointOnCircle1(2, 0, 0);  // On positive X axis
    Vector3D pointOnCircle2(0, 2, 0);  // On positive Y axis
    Vector3D pointOnCircle3(-2, 0, 0); // On negative X axis
    
    assert(circle.containsPoint(pointOnCircle1));
    assert(circle.containsPoint(pointOnCircle2));
    assert(circle.containsPoint(pointOnCircle3));

    // Test normals at points on the circle (should equal the circle plane normal)
    assert(isEqual(circle.getNormalAt(pointOnCircle1), circle.getNormal()));
    assert(isEqual(circle.getNormalAt(pointOnCircle2), circle.getNormal()));
    assert(isEqual(circle.getNormalAt(pointOnCircle3), circle.getNormal()));
    
    // Test points NOT on the circle
    Vector3D pointInside(1, 0, 0);     // Inside the circle
    Vector3D pointOutside(3, 0, 0);    // Outside the circle
    Vector3D pointOffPlane(2, 0, 1);   // Right distance but off plane
    
    assert(!circle.containsPoint(pointInside));
    assert(!circle.containsPoint(pointOutside));
    assert(!circle.containsPoint(pointOffPlane));
}

void testCircleSetters() {
    Vector3D center(0, 0, 0);
    Vector3D normal(0, 0, 1);
    double radius = 1.0;
    
    Circle circle(center, radius, normal);
    
    // Test setCenter
    Vector3D newCenter(5, 10, -3);
    circle.setCenter(newCenter);
    assert(isEqual(circle.getCenter(), newCenter));
    
    // Test setRadius
    double newRadius = 7.5;
    circle.setRadius(newRadius);
    assert(isEqual(circle.getRadius(), newRadius));
    
    // Test setNormal
    Vector3D newNormal(1, 2, 2); // Will be normal
    circle.setNormal(newNormal);
    Vector3D expectedNormal = newNormal.normal();
    assert(isEqual(circle.getNormal(), expectedNormal));
    
    // Verify normal is still normal after setting
    assert(isEqual(circle.getNormal().length(), 1.0));
}

void testCircleValidation() {
    Vector3D center(0, 0, 0);
    Vector3D normal(0, 0, 1);
    
    // Test valid circle
    Circle validCircle(center, 5.0, normal);
    assert(validCircle.isValid());
    
    // Test invalid circle with zero radius
    try {
        Circle invalidCircle1(center, 0.0, normal);
        assert(!invalidCircle1.isValid()); // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected exception for zero radius
    }
    
    // Test invalid circle with negative radius
    try {
        Circle invalidCircle2(center, -2.0, normal);
        assert(!invalidCircle2.isValid());
    } catch (const std::invalid_argument&) {
        // Expected exception for negative radius
    }

    // Test circle with zero normal vector
    Vector3D zeroNormal(0, 0, 0);
    try {
        Circle invalidCircle3(center, 3.0, zeroNormal);
        assert(!invalidCircle3.isValid());
    } catch (const std::invalid_argument&) {
        // Expected exception for zero normal
    }
}

void testCircleParametricPoints() {
    // Create circle in XY plane
    Vector3D center(0, 0, 0);
    Vector3D normal(0, 0, 1);
    double radius = 1.0;
    
    Circle circle(center, radius, normal);
    
    // Test parametric points at specific angles
    Vector3D point0 = circle.getPointAtAngle(0);                    // Should be (1, 0, 0)
    Vector3D pointPi2 = circle.getPointAtAngle(math::pi / 2);       // Should be (0, 1, 0)
    Vector3D pointPi = circle.getPointAtAngle(math::pi);            // Should be (-1, 0, 0)
    Vector3D point3Pi2 = circle.getPointAtAngle(3 * math::pi / 2);  // Should be (0, -1, 0)
    
    // Check that these points are approximately correct
    assert(isEqual(point0, Vector3D(1, 0, 0)));
    assert(isEqual(pointPi2, Vector3D(0, 1, 0)));
    assert(isEqual(pointPi, Vector3D(-1, 0, 0)));
    assert(isEqual(point3Pi2, Vector3D(0, -1, 0)));
    
    // Verify all points are on the circle
    assert(circle.containsPoint(point0));
    assert(circle.containsPoint(pointPi2));
    assert(circle.containsPoint(pointPi));
    assert(circle.containsPoint(point3Pi2));
    
    // Test that all parametric points are at the correct distance from center
    assert(isEqual((point0 - center).length(), radius));
    assert(isEqual((pointPi2 - center).length(), radius));
    assert(isEqual((pointPi - center).length(), radius));
    assert(isEqual((point3Pi2 - center).length(), radius));
}
