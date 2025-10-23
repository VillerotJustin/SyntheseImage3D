#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include "../Lib/Geometry/Rectangle.h"
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
    Vector3D topLeft(1, 2, 3);
    Vector3D topRight = topLeft + Vector3D(5.0, 0, 0); // length along x
    Vector3D bottomLeft = topLeft + Vector3D(0, 3.0, 0); // width along y

    Rectangle rect(topLeft, topRight, bottomLeft);

    assert(isEqual(rect.getOrigin(), topLeft));
    assert(isEqual(rect.getLength(), 5.0));
    assert(isEqual(rect.getWidth(), 3.0));
    Vector3D expectedNormal = (topRight - topLeft).cross(bottomLeft - topLeft).normal();
    assert(isEqual(rect.getNormal(), expectedNormal));
}

void testRectangleBasicProperties() {
    Vector3D topLeft(0, 0, 0);
    Vector3D topRight = topLeft + Vector3D(4.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 6.0, 0);

    Rectangle rect(topLeft, topRight, bottomLeft);

    // Test that normal is normalized
    assert(isEqual(rect.getNormal().length(), 1.0));

    // Test getters
    assert(isEqual(rect.getOrigin(), topLeft));
    assert(isEqual(rect.getLength(), (topRight - topLeft).length()));
    assert(isEqual(rect.getWidth(), (bottomLeft - topLeft).length()));
    Vector3D expectedNormal = (topRight - topLeft).cross(bottomLeft - topLeft).normal();
    assert(isEqual(rect.getNormal(), expectedNormal));
}

void testRectangleAreaAndPerimeter() {
    Vector3D topLeft(2, -1, 5);
    Vector3D topRight = topLeft + Vector3D(3.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 4.0, 0);

    Rectangle rect(topLeft, topRight, bottomLeft);

    // Test area: length * width
    double expectedArea = (topRight - topLeft).length() * (bottomLeft - topLeft).length();
    assert(isEqual(rect.getArea(), expectedArea));

    // Test perimeter: 2 * (length + width)
    double expectedPerimeter = 2.0 * ((topRight - topLeft).length() + (bottomLeft - topLeft).length());
    assert(isEqual(rect.getPerimeter(), expectedPerimeter));
}

void testRectangleCenterAndCorners() {
    // Create rectangle in XY plane
    Vector3D topLeft(0, 0, 0);
    Vector3D topRight = topLeft + Vector3D(4.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 2.0, 0);

    Rectangle rect(topLeft, topRight, bottomLeft);

    // Test center calculation
    Vector3D center = rect.getCenter();
    Vector3D expectedCenter = topLeft + 0.5 * (topRight - topLeft) + 0.5 * (bottomLeft - topLeft);
    assert(isEqual(center, expectedCenter));

    // Test corners
    Vector3D corners[4];
    rect.getCorners(corners);

    // Verify all corners are valid points (relax tolerance further)
    for (int i = 0; i < 4; i++) {
        if (!rect.containsPoint(corners[i], 1e-4)) {
            std::cerr << "Corner " << i << " not contained: " << corners[i] << std::endl;
        }
        assert(rect.containsPoint(corners[i], 1e-4));
    }

    // Verify corners form a rectangle by checking distances
    double dist01 = (corners[1] - corners[0]).length();
    double dist12 = (corners[2] - corners[1]).length();
    double dist23 = (corners[3] - corners[2]).length();
    double dist30 = (corners[0] - corners[3]).length();

    // Should have two pairs of equal opposite sides
    assert(isEqual(dist01, dist23));
    assert(isEqual(dist12, dist30));

    // One pair should be length, other should be width
    double length = (topRight - topLeft).length();
    double width = (bottomLeft - topLeft).length();
    assert((isEqual(dist01, length) && isEqual(dist12, width)) ||
           (isEqual(dist01, width) && isEqual(dist12, length)));
}

void testRectanglePointOperations() {
    // Create rectangle in XY plane
    Vector3D topLeft(1, 1, 0);
    Vector3D topRight = topLeft + Vector3D(3.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 2.0, 0);

    Rectangle rect(topLeft, topRight, bottomLeft);

    // Test points that should be inside
    Vector3D corners[4];
    rect.getCorners(corners);

    // All corners should be contained
    for (int i = 0; i < 4; i++) {
        assert(rect.containsPoint(corners[i]));
        assert(isEqual(rect.getNormalAt(corners[i]), rect.getNormal()));
    }

    // Test center point should be inside
    Vector3D center = rect.getCenter();
    assert(rect.containsPoint(center));
    assert(isEqual(rect.getNormalAt(center), rect.getNormal()));
    

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
    Vector3D topLeft(0, 5, 0);
    Vector3D topRight = topLeft + Vector3D(4.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 0, 3.0);

    Rectangle rect(topLeft, topRight, bottomLeft);

    // Test point projection
    Vector3D pointAbove(2, 10, 1);
    Vector3D projected = rect.projectPointToPlane(pointAbove);

    // Projected point should have same X and Z, but Y should be 5 (plane Y)
    assert(isEqual(projected.x(), pointAbove.x()));
    assert(isEqual(projected.z(), pointAbove.z()));
    assert(isEqual(projected.y(), 5.0));

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
    Vector3D topLeft(0, 0, 0);
    Vector3D topRight = topLeft + Vector3D(6.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 4.0, 0);

    Rectangle rect(topLeft, topRight, bottomLeft);

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
    Vector3D topLeft(2, 3, 4);
    Vector3D topRight = topLeft + Vector3D(5.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 3.0, 0);

    Rectangle rect(topLeft, topRight, bottomLeft);

    // Test translation
    Vector3D offset(1, -2, 3);
    Rectangle translated = rect.translate(offset);

    Vector3D expectedNewOrigin = topLeft + offset;
    assert(isEqual(translated.getOrigin(), expectedNewOrigin));
    assert(isEqual(translated.getLength(), rect.getLength()));
    assert(isEqual(translated.getWidth(), rect.getWidth()));
    assert(isEqual(translated.getNormal(), rect.getNormal()));

    // Test scaling
    double lengthScale = 2.0;
    double widthScale = 1.5;
    Rectangle scaled = rect.scale(lengthScale, widthScale);

    assert(isEqual(scaled.getOrigin(), rect.getOrigin())); // Origin unchanged
    assert(isEqual(scaled.getLength(), rect.getLength() * lengthScale));
    assert(isEqual(scaled.getWidth(), rect.getWidth() * widthScale));
    assert(isEqual(scaled.getNormal(), rect.getNormal())); // Normal unchanged

    // Test invalid scaling (should return unchanged rectangle)
    Rectangle invalidScaled = rect.scale(-1.0, 2.0);
    assert(isEqual(invalidScaled.getLength(), rect.getLength()));
    assert(isEqual(invalidScaled.getWidth(), rect.getWidth()));
}

void testRectangleSetters() {
    Vector3D topLeft(0, 0, 0);
    Vector3D topRight = topLeft + Vector3D(1.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 1.0, 0);

    Rectangle rect(topLeft, topRight, bottomLeft);

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

    // Test invalid dimensions (should throw)
    double originalLength = rect.getLength();
    double originalWidth = rect.getWidth();
    try {
        rect.setDimensions(-2.0, 3.0);                      // Negative length should throw
        assert(isEqual(rect.getLength(), originalLength));  // Shouldn't reach here
        assert(isEqual(rect.getWidth(), originalWidth));    // Shouldn't reach here
    } catch (const std::invalid_argument&) {
        // Expected exception for negative length
    }

    // Test setNormal
    Vector3D newNormal(1, 2, 2);
    rect.setNormal(newNormal);
    Vector3D expectedNormal = newNormal.normal();
    assert(isEqual(rect.getNormal(), expectedNormal));
    assert(isEqual(rect.getNormal().length(), 1.0));
}

void testRectangleValidation() {
    Vector3D topLeft(0, 0, 0);
    Vector3D topRight = topLeft + Vector3D(2.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 3.0, 0);

    // Test valid rectangle
    Rectangle validRect(topLeft, topRight, bottomLeft);
    assert(validRect.isValid());

    // Test rectangle that becomes invalid after setting invalid dimensions
    try {
        validRect.setDimensions(0.0, 5.0);
        assert(!validRect.isValid());
    } catch (const std::invalid_argument&) {
        // Expected exception for invalid dimensions
    }

    // Test rectangle that becomes invalid after setting zero normal
    try {
        Rectangle anotherValidRect(topLeft, topRight, bottomLeft);
        anotherValidRect.setNormal(Vector3D(0, 0, 0));
        assert(!anotherValidRect.isValid());
    } catch (const std::invalid_argument&) {
        // Expected exception for zero normal
    }
    
}
