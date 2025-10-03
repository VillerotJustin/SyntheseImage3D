#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include "../Lib/Geometry/Rectangle.h"
#include "../Lib/Vector3D.h"
#include "../Lib/math_common.h"

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
void testRectangleConstructor();
void testRectangleBasicProperties();
void testRectangleAreaAndPerimeter();
void testRectangleCenterAndCorners();
void testRectanglePointOperations();
void testRectangleProjections();
void testRectangleParametricPoints();
void testRectangleTransformations();
void testRectangleSetters();
void testRectangleValidation();

int main() {
    std::cout << "=== Rectangle Test Suite ===" << std::endl;
    
    try {
        testRectangleConstructor();
        std::cout << "✓ Rectangle constructor test passed" << std::endl;
        
        testRectangleBasicProperties();
        std::cout << "✓ Rectangle basic properties test passed" << std::endl;
        
        testRectangleAreaAndPerimeter();
        std::cout << "✓ Rectangle area and perimeter test passed" << std::endl;
        
        testRectangleCenterAndCorners();
        std::cout << "✓ Rectangle center and corners test passed" << std::endl;
        
        testRectanglePointOperations();
        std::cout << "✓ Rectangle point operations test passed" << std::endl;
        
        testRectangleProjections();
        std::cout << "✓ Rectangle projections test passed" << std::endl;
        
        testRectangleParametricPoints();
        std::cout << "✓ Rectangle parametric points test passed" << std::endl;
        
        testRectangleTransformations();
        std::cout << "✓ Rectangle transformations test passed" << std::endl;
        
        testRectangleSetters();
        std::cout << "✓ Rectangle setters test passed" << std::endl;
        
        testRectangleValidation();
        std::cout << "✓ Rectangle validation test passed" << std::endl;
        
        std::cout << "\n🎉 All Rectangle tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void testRectangleConstructor() {
    Vector3D origin(1, 2, 3);
    double length = 5.0;
    double width = 3.0;
    Vector3D normal(0, 0, 1);
    
    Rectangle rect(origin, length, width, normal);
    
    assert(isEqual(rect.getOrigin(), origin));
    assert(isEqual(rect.getLength(), length));
    assert(isEqual(rect.getWidth(), width));
    assert(isEqual(rect.getNormal(), normal.normalized()));
}

void testRectangleBasicProperties() {
    Vector3D origin(0, 0, 0);
    Vector3D normal(1, 1, 1); // Will be normalized
    double length = 4.0;
    double width = 6.0;
    
    Rectangle rect(origin, length, width, normal);
    
    // Test that normal is normalized
    assert(isEqual(rect.getNormal().length(), 1.0));
    
    // Test getters
    assert(isEqual(rect.getOrigin(), origin));
    assert(isEqual(rect.getLength(), length));
    assert(isEqual(rect.getWidth(), width));
    Vector3D expectedNormal = normal.normalized();
    assert(isEqual(rect.getNormal(), expectedNormal));
}

void testRectangleAreaAndPerimeter() {
    Vector3D origin(2, -1, 5);
    Vector3D normal(0, 1, 0);
    double length = 3.0;
    double width = 4.0;
    
    Rectangle rect(origin, length, width, normal);
    
    // Test area: length * width
    double expectedArea = length * width;
    assert(isEqual(rect.getArea(), expectedArea));
    
    // Test perimeter: 2 * (length + width)
    double expectedPerimeter = 2.0 * (length + width);
    assert(isEqual(rect.getPerimeter(), expectedPerimeter));
}

void testRectangleCenterAndCorners() {
    // Create rectangle in XY plane
    Vector3D origin(0, 0, 0);
    Vector3D normal(0, 0, 1);
    double length = 4.0;
    double width = 2.0;
    
    Rectangle rect(origin, length, width, normal);
    
    // Test center calculation (should be at length/2, width/2 from origin)
    Vector3D center = rect.getCenter();
    // The exact center depends on the basis vectors generated, but we can test
    // that it's within the expected bounds and at correct distance from origin
    
    // Test corners
    Vector3D corners[4];
    rect.getCorners(corners);
    
    // Verify all corners are valid points
    for (int i = 0; i < 4; i++) {
        assert(rect.containsPoint(corners[i]));
    }
    
    // Verify corners form a rectangle by checking distances
    // Distance between adjacent corners should be either length or width
    double dist01 = (corners[1] - corners[0]).length();
    double dist12 = (corners[2] - corners[1]).length();
    double dist23 = (corners[3] - corners[2]).length();
    double dist30 = (corners[0] - corners[3]).length();
    
    // Should have two pairs of equal opposite sides
    assert(isEqual(dist01, dist23));
    assert(isEqual(dist12, dist30));
    
    // One pair should be length, other should be width
    assert((isEqual(dist01, length) && isEqual(dist12, width)) ||
           (isEqual(dist01, width) && isEqual(dist12, length)));
}

void testRectanglePointOperations() {
    // Create rectangle in XY plane
    Vector3D origin(1, 1, 0);
    Vector3D normal(0, 0, 1);
    double length = 3.0;
    double width = 2.0;
    
    Rectangle rect(origin, length, width, normal);
    
    // Test points that should be inside (we need to account for the generated basis vectors)
    // For simplicity, test the corners we know are valid
    Vector3D corners[4];
    rect.getCorners(corners);
    
    // All corners should be contained
    for (int i = 0; i < 4; i++) {
        assert(rect.containsPoint(corners[i]));
    }
    
    // Test center point should be inside
    Vector3D center = rect.getCenter();
    assert(rect.containsPoint(center));
    
    // Test point off the plane should not be contained
    Vector3D pointOffPlane = center + Vector3D(0, 0, 1);
    assert(!rect.containsPoint(pointOffPlane));
    
    // Test edge detection - corners should be on edges
    for (int i = 0; i < 4; i++) {
        assert(rect.isPointOnEdge(corners[i]));
    }
    
    // Center should not be on edge (unless rectangle is degenerate)
    assert(!rect.isPointOnEdge(center));
}

void testRectangleProjections() {
    // Create rectangle in XZ plane (normal in Y direction)
    Vector3D origin(0, 5, 0);
    Vector3D normal(0, 1, 0);
    double length = 4.0;
    double width = 3.0;
    
    Rectangle rect(origin, length, width, normal);
    
    // Test point projection
    Vector3D pointAbove(2, 10, 1);
    Vector3D projected = rect.projectPointToPlane(pointAbove);
    
    // Projected point should have same X and Z, but Y should be 5 (plane Y)
    assert(isEqual(projected.x, pointAbove.x));
    assert(isEqual(projected.z, pointAbove.z));
    assert(isEqual(projected.y, 5.0));
    
    // Test closest point on rectangle
    Vector3D testPoint(10, 8, 10); // Far from rectangle
    Vector3D closest = rect.closestPointOnRectangle(testPoint);
    
    // Closest point should be on the rectangle
    assert(rect.containsPoint(closest));
    
    // Test distance calculation
    double distance = rect.distanceToPoint(testPoint);
    double expectedDistance = (testPoint - closest).length();
    assert(isEqual(distance, expectedDistance));
}

void testRectangleParametricPoints() {
    Vector3D origin(0, 0, 0);
    Vector3D normal(0, 0, 1);
    double length = 6.0;
    double width = 4.0;
    
    Rectangle rect(origin, length, width, normal);
    
    // Test parametric points at corners
    Vector3D point00 = rect.getPointAt(0.0, 0.0); // Origin corner
    Vector3D point10 = rect.getPointAt(1.0, 0.0); // Length corner
    Vector3D point11 = rect.getPointAt(1.0, 1.0); // Opposite corner
    Vector3D point01 = rect.getPointAt(0.0, 1.0); // Width corner
    
    // All parametric points should be on the rectangle
    assert(rect.containsPoint(point00));
    assert(rect.containsPoint(point10));
    assert(rect.containsPoint(point11));
    assert(rect.containsPoint(point01));
    
    // Test center point
    Vector3D centerParam = rect.getPointAt(0.5, 0.5);
    Vector3D centerDirect = rect.getCenter();
    assert(isEqual(centerParam, centerDirect));
    
    // Test that parametric coordinates respect the bounds
    Vector3D pointClamped = rect.getPointAt(-0.5, 1.5); // Should be clamped to (0,1)
    Vector3D expected = rect.getPointAt(0.0, 1.0);
    assert(isEqual(pointClamped, expected));
}

void testRectangleTransformations() {
    Vector3D origin(2, 3, 4);
    Vector3D normal(1, 0, 0);
    double length = 5.0;
    double width = 3.0;
    
    Rectangle rect(origin, length, width, normal);
    
    // Test translation
    Vector3D offset(1, -2, 3);
    Rectangle translated = rect.translate(offset);
    
    Vector3D expectedNewOrigin = origin + offset;
    assert(isEqual(translated.getOrigin(), expectedNewOrigin));
    assert(isEqual(translated.getLength(), rect.getLength()));
    assert(isEqual(translated.getWidth(), rect.getWidth()));
    assert(isEqual(translated.getNormal(), rect.getNormal()));
    
    // Test scaling
    double lengthScale = 2.0;
    double widthScale = 1.5;
    Rectangle scaled = rect.scale(lengthScale, widthScale);
    
    assert(isEqual(scaled.getOrigin(), rect.getOrigin())); // Origin unchanged
    assert(isEqual(scaled.getLength(), length * lengthScale));
    assert(isEqual(scaled.getWidth(), width * widthScale));
    assert(isEqual(scaled.getNormal(), rect.getNormal())); // Normal unchanged
    
    // Test invalid scaling (should return unchanged rectangle)
    Rectangle invalidScaled = rect.scale(-1.0, 2.0);
    assert(isEqual(invalidScaled.getLength(), rect.getLength()));
    assert(isEqual(invalidScaled.getWidth(), rect.getWidth()));
}

void testRectangleSetters() {
    Vector3D origin(0, 0, 0);
    Vector3D normal(0, 0, 1);
    double length = 1.0;
    double width = 1.0;
    
    Rectangle rect(origin, length, width, normal);
    
    // Test setOrigin
    Vector3D newOrigin(5, -3, 2);
    rect.setOrigin(newOrigin);
    assert(isEqual(rect.getOrigin(), newOrigin));
    
    // Test setDimensions
    double newLength = 7.0;
    double newWidth = 4.5;
    rect.setDimensions(newLength, newWidth);
    assert(isEqual(rect.getLength(), newLength));
    assert(isEqual(rect.getWidth(), newWidth));
    
    // Test invalid dimensions (should be ignored)
    double originalLength = rect.getLength();
    double originalWidth = rect.getWidth();
    rect.setDimensions(-2.0, 3.0); // Negative length should be ignored
    assert(isEqual(rect.getLength(), originalLength));
    assert(isEqual(rect.getWidth(), originalWidth));
    
    // Test setNormal
    Vector3D newNormal(1, 2, 2);
    rect.setNormal(newNormal);
    Vector3D expectedNormal = newNormal.normalized();
    assert(isEqual(rect.getNormal(), expectedNormal));
    assert(isEqual(rect.getNormal().length(), 1.0));
}

void testRectangleValidation() {
    Vector3D origin(0, 0, 0);
    Vector3D normal(0, 1, 0);
    
    // Test valid rectangle
    Rectangle validRect(origin, 2.0, 3.0, normal);
    assert(validRect.isValid());
    
    // Test invalid rectangles
    Rectangle invalidRect1(origin, 0.0, 3.0, normal);  // Zero length
    Rectangle invalidRect2(origin, 2.0, -1.0, normal); // Negative width
    Rectangle invalidRect3(origin, 2.0, 3.0, Vector3D(0, 0, 0)); // Zero normal
    
    assert(!invalidRect1.isValid());
    assert(!invalidRect2.isValid());
    assert(!invalidRect3.isValid());
    
    // Test rectangle that becomes invalid after setting invalid dimensions
    validRect.setDimensions(0.0, 5.0);
    assert(!validRect.isValid());
    
    // Test rectangle that becomes invalid after setting zero normal
    Rectangle anotherValidRect(origin, 2.0, 3.0, normal);
    anotherValidRect.setNormal(Vector3D(0, 0, 0));
    assert(!anotherValidRect.isValid());
}
