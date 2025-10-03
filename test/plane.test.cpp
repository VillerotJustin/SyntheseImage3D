#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include "../Lib/Geometry/Plane.h"
#include "../Lib/Geometry/Vector3D.h"
#include "../Lib/Math/math_common.h"

using namespace geometry;

// Helper function for floating-point comparison
bool isEqual(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
}

// Helper function to compare Vector3D
bool isEqual(const Vector3D& a, const Vector3D& b, double epsilon = 1e-9) {
    return (a - b).length() < epsilon;
}

// Test function declarations
void testPlaneConstructors();
void testPlaneBasicProperties();
void testPlaneDistances();
void testPlanePointOperations();
void testPlaneLineIntersections();
void testPlaneProjections();
void testPlaneEquation();
void testPlaneSetters();
void testPlaneValidation();

int main() {
    std::cout << "=== Plane Test Suite ===" << std::endl;
    
    try {
        testPlaneConstructors();
        std::cout << "✓ Plane constructors test passed" << std::endl;
        
        testPlaneBasicProperties();
        std::cout << "✓ Plane basic properties test passed" << std::endl;
        
        testPlaneDistances();
        std::cout << "✓ Plane distance calculations test passed" << std::endl;
        
        testPlanePointOperations();
        std::cout << "✓ Plane point operations test passed" << std::endl;
        
        testPlaneLineIntersections();
        std::cout << "✓ Plane line intersections test passed" << std::endl;
        
        testPlaneProjections();
        std::cout << "✓ Plane projections test passed" << std::endl;
        
        testPlaneEquation();
        std::cout << "✓ Plane equation test passed" << std::endl;
        
        testPlaneSetters();
        std::cout << "✓ Plane setters test passed" << std::endl;
        
        testPlaneValidation();
        std::cout << "✓ Plane validation test passed" << std::endl;
        
        std::cout << "\n🎉 All Plane tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void testPlaneConstructors() {
    // Test constructor from point and normal
    Vector3D origin(1, 2, 3);
    Vector3D normal(0, 0, 1);
    Plane plane1(origin, normal);
    
    assert(isEqual(plane1.getOrigin(), origin));
    assert(isEqual(plane1.getNormal(), normal.normal()));
    
    // Test constructor from three points
    Vector3D point1(0, 0, 0);
    Vector3D point2(1, 0, 0);
    Vector3D point3(0, 1, 0);
    Plane plane2(point1, point2, point3);
    
    // Should create XY plane with normal (0, 0, 1)
    Vector3D expectedNormal(0, 0, 1);
    assert(isEqual(plane2.getOrigin(), point1));
    assert(isEqual(plane2.getNormal(), expectedNormal));
}

void testPlaneBasicProperties() {
    Vector3D origin(5, 10, -2);
    Vector3D normal(1, 1, 1);  // Will be normalized
    Plane plane(origin, normal);
    
    // Test that normal is normalized
    assert(isEqual(plane.getNormal().length(), 1.0));
    
    // Test getters
    assert(isEqual(plane.getOrigin(), origin));
    Vector3D expectedNormal = normal.normal();
    assert(isEqual(plane.getNormal(), expectedNormal));
}

void testPlaneDistances() {
    // Create XY plane at z = 0
    Vector3D origin(0, 0, 0);
    Vector3D normal(0, 0, 1);
    Plane plane(origin, normal);
    
    // Test points at various distances
    Vector3D pointAbove(5, 3, 2);   // 2 units above plane
    Vector3D pointBelow(1, 1, -3);  // 3 units below plane
    Vector3D pointOnPlane(10, -5, 0); // On the plane
    
    assert(isEqual(plane.distanceToPoint(pointAbove), 2.0));
    assert(isEqual(plane.distanceToPoint(pointBelow), -3.0));
    assert(isEqual(plane.distanceToPoint(pointOnPlane), 0.0));
}

void testPlanePointOperations() {
    // Create plane through origin with normal pointing up
    Vector3D origin(0, 0, 0);
    Vector3D normal(0, 0, 1);
    Plane plane(origin, normal);
    
    // Test points on the plane
    Vector3D pointOnPlane1(1, 2, 0);
    Vector3D pointOnPlane2(-5, 10, 0);
    Vector3D pointOnPlane3(0, 0, 0);
    
    assert(plane.containsPoint(pointOnPlane1));
    assert(plane.containsPoint(pointOnPlane2));
    assert(plane.containsPoint(pointOnPlane3));
    
    // Test points not on the plane
    Vector3D pointOffPlane1(1, 2, 1);
    Vector3D pointOffPlane2(0, 0, -0.1);
    
    assert(!plane.containsPoint(pointOffPlane1));
    assert(!plane.containsPoint(pointOffPlane2));
    
    // Test with tolerance
    Vector3D nearPlane(1, 1, 1e-10);
    assert(plane.containsPoint(nearPlane, 1e-9));
}

void testPlaneLineIntersections() {
    // Create XY plane at z = 5
    Vector3D origin(0, 0, 5);
    Vector3D normal(0, 0, 1);
    Plane plane(origin, normal);
    
    // Test line that intersects plane
    Vector3D linePoint(2, 3, 0);
    Vector3D lineDirection(0, 0, 1);  // Pointing up
    
    Vector3D intersection = plane.lineIntersection(linePoint, lineDirection);
    Vector3D expectedIntersection(2, 3, 5);
    assert(isEqual(intersection, expectedIntersection));
    
    // Test parallel line detection
    Vector3D parallelDirection(1, 1, 0);  // Parallel to XY plane
    assert(plane.isLineParallel(parallelDirection));
    
    // Test non-parallel line
    Vector3D nonParallelDirection(1, 1, 1);
    assert(!plane.isLineParallel(nonParallelDirection));
}

void testPlaneProjections() {
    // Create plane with normal (1, 0, 0) passing through (3, 0, 0)
    Vector3D origin(3, 0, 0);
    Vector3D normal(1, 0, 0);
    Plane plane(origin, normal);
    
    // Test point projection
    Vector3D point(7, 5, -2);
    Vector3D projected = plane.projectPoint(point);
    
    // Should project to (3, 5, -2) - same y,z but x = 3
    Vector3D expectedProjection(3, 5, -2);
    assert(isEqual(projected, expectedProjection));
    
    // Verify projected point is on the plane
    assert(plane.containsPoint(projected));
    
    // Test that projecting a point on the plane returns the same point
    Vector3D pointOnPlane(3, 10, -5);
    Vector3D projectedOnPlane = plane.projectPoint(pointOnPlane);
    assert(isEqual(projectedOnPlane, pointOnPlane));
}

void testPlaneEquation() {
    // Create plane: 2x + 3y + 6z - 12 = 0
    // This plane passes through (6, 0, 0), (0, 4, 0), (0, 0, 2)
    Vector3D origin(6, 0, 0);
    Vector3D normal(2, 3, 6);  // Will be normalized
    Plane plane(origin, normal);
    
    double a, b, c, d;
    plane.getPlaneEquation(a, b, c, d);
    
    // The normal should be normalized, so coefficients will be scaled
    Vector3D normalizedNormal = normal.normal();
    assert(isEqual(a, normalizedNormal.x()));
    assert(isEqual(b, normalizedNormal.y()));
    assert(isEqual(c, normalizedNormal.z()));
    
    // Verify the equation works for points on the plane
    Vector3D testPoint(0, 4, 0);  // Should be on the plane
    double result = a * testPoint.x() + b * testPoint.y() + c * testPoint.z() + d;
    assert(isEqual(result, 0.0));
}

void testPlaneSetters() {
    Vector3D origin(0, 0, 0);
    Vector3D normal(0, 0, 1);
    Plane plane(origin, normal);
    
    // Test setOrigin
    Vector3D newOrigin(5, -3, 2);
    plane.setOrigin(newOrigin);
    assert(isEqual(plane.getOrigin(), newOrigin));
    
    // Test setNormal
    Vector3D newNormal(1, 2, 2);
    plane.setNormal(newNormal);
    Vector3D expectedNormal = newNormal.normal();
    assert(isEqual(plane.getNormal(), expectedNormal));
    assert(isEqual(plane.getNormal().length(), 1.0));
}

void testPlaneValidation() {
    Vector3D origin(0, 0, 0);
    
    // Test valid plane
    Vector3D validNormal(0, 1, 0);
    Plane validPlane(origin, validNormal);
    assert(validPlane.isValid());
    
    // Test invalid plane with zero normal
    Vector3D zeroNormal(0, 0, 0);
    try {
        Plane invalidPlane(origin, zeroNormal);
        assert(!invalidPlane.isValid()); // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected exception for zero normal
    }
    
    
    // Test plane that becomes invalid after setting zero normal
    try {
        validPlane.setNormal(zeroNormal);
        assert(!validPlane.isValid());
    } catch (const std::invalid_argument&) {
        // Expected exception for zero normal
    }
}
