#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <string>
#include "../Lib/Geometry/Box.h"
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
void testBoxConstructor();
void testBoxBasicProperties();
void testBoxVolumeAndArea();
void testBoxPointOperations();
void testBoxCorners();
void testBoxIntersections();
void testBoxTransformations();
void testBoxSetters();
void testBoxValidation();
void testBoxNormals();

int main() {
    std::cout << "=== Box Test Suite ===" << std::endl;
    
    try {
        testBoxConstructor();
        std::cout << "✓ Box constructor test passed" << std::endl;
        
        testBoxBasicProperties();
        std::cout << "✓ Box basic properties test passed" << std::endl;
        
        testBoxVolumeAndArea();
        std::cout << "✓ Box volume and surface area test passed" << std::endl;
        
        testBoxPointOperations();
        std::cout << "✓ Box point operations test passed" << std::endl;
        
        testBoxCorners();
        std::cout << "✓ Box corners test passed" << std::endl;
        
        testBoxIntersections();
        std::cout << "✓ Box intersections test passed" << std::endl;
        
        testBoxTransformations();
        std::cout << "✓ Box transformations test passed" << std::endl;
        
        testBoxSetters();
        std::cout << "✓ Box setters test passed" << std::endl;
        
        testBoxValidation();
        std::cout << "✓ Box validation test passed" << std::endl;

        testBoxNormals();
        std::cout << "✓ Box normals test passed" << std::endl;
        
        std::cout << "\n🎉 All Box tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void testBoxConstructor() {
    Vector3D origin(1, 2, 3);
    Vector3D normal(0, 2, 0);
    double w = 4.0, h = 5.0, p = 6.0;
    
    Box box(origin, w, h, p, normal);
    
    assert(isEqual(box.getOrigin(), origin));
    assert(isEqual(box.getWidth(), w));
    assert(isEqual(box.getHeight(), h));
    assert(isEqual(box.getDepth(), p));
    assert(isEqual(box.getNormal(), normal.normal()));
}

void testBoxBasicProperties() {
    Vector3D origin(0, 0, 0);
    Vector3D normal(0, 0, 1);
    Box box(origin, 3.0, 4.0, 5.0, normal);
    
    // Test dimensions
    assert(isEqual(box.getWidth(), 3.0));
    assert(isEqual(box.getHeight(), 4.0));
    assert(isEqual(box.getDepth(), 5.0));
    
    // Test center calculation
    Vector3D expectedCenter(1.5, 2.0, 2.5);
    assert(isEqual(box.getCenter(), expectedCenter));
    
    // Test min/max corners
    assert(isEqual(box.getMinCorner(), origin));
    Vector3D expectedMax(3.0, 4.0, 5.0);
    assert(isEqual(box.getMaxCorner(), expectedMax));
}

void testBoxVolumeAndArea() {
    Vector3D origin(0, 0, 0);
    Vector3D normal(1, 0, 0);
    Box box(origin, 2.0, 3.0, 4.0, normal);
    
    // Test volume: w * h * p
    double expectedVolume = 2.0 * 3.0 * 4.0;
    assert(isEqual(box.getVolume(), expectedVolume));
    
    // Test surface area: 2(wh + wp + hp)
    double expectedArea = 2.0 * (2.0*3.0 + 2.0*4.0 + 3.0*4.0);
    assert(isEqual(box.getSurfaceArea(), expectedArea));
}

void testBoxPointOperations() {
    Vector3D origin(1, 1, 1);
    Vector3D normal(0, 0, 1);
    Box box(origin, 2.0, 2.0, 2.0, normal);
    
    // Test points inside the box
    Vector3D pointInside1(1.5, 1.5, 1.5);
    Vector3D pointInside2(2.0, 2.0, 2.0);
    assert(box.containsPoint(pointInside1));
    assert(box.containsPoint(pointInside2));
    
    // Test points outside the box
    Vector3D pointOutside1(0.5, 1.5, 1.5);  // Before origin
    Vector3D pointOutside2(3.5, 1.5, 1.5);  // Beyond max
    Vector3D pointOutside3(1.5, 0.5, 1.5);  // Below origin
    assert(!box.containsPoint(pointOutside1));
    assert(!box.containsPoint(pointOutside2));
    assert(!box.containsPoint(pointOutside3));
    
    // Test points on surface
    Vector3D pointOnSurface1(1.0, 1.5, 1.5);  // On min X face
    Vector3D pointOnSurface2(3.0, 1.5, 1.5);  // On max X face
    assert(box.isPointOnSurface(pointOnSurface1));
    assert(box.isPointOnSurface(pointOnSurface2));
}

void testBoxCorners() {
    Vector3D origin(0, 0, 0);
    Vector3D normal(0, 1, 0);
    Box box(origin, 1.0, 2.0, 3.0, normal);
    
    Vector3D corners[8];
    box.getCorners(corners);
    
    // Expected corners for a box from (0,0,0) to (1,2,3)
    Vector3D expected[8] = {
        Vector3D(0, 0, 0),  // (0,0,0)
        Vector3D(1, 0, 0),  // (w,0,0)
        Vector3D(1, 2, 0),  // (w,h,0)
        Vector3D(0, 2, 0),  // (0,h,0)
        Vector3D(0, 0, 3),  // (0,0,p)
        Vector3D(1, 0, 3),  // (w,0,p)
        Vector3D(1, 2, 3),  // (w,h,p)
        Vector3D(0, 2, 3)   // (0,h,p)
    };
    
    for (int i = 0; i < 8; i++) {
        assert(isEqual(corners[i], expected[i]));
    }
}

void testBoxIntersections() {
    Vector3D origin1(0, 0, 0);
    Vector3D normal1(0, 0, 1);
    Box box1(origin1, 4.0, 4.0, 4.0, normal1);
    
    Vector3D origin2(2, 2, 2);
    Vector3D normal2(0, 0, 1);
    Box box2(origin2, 4.0, 4.0, 4.0, normal2);
    
    // Test intersection detection
    assert(box1.intersects(box2));
    
    // Test intersection calculation - expect exception since not implemented
    try {
        auto intersection = box1.intersectionPoints(box2);
        // If we reach here, the intersection was implemented
        assert(intersection.has_value());
        std::cout << "Note: Box intersection is now implemented!" << std::endl;
        
    } catch (const std::runtime_error& e) {
        // Expected behavior: intersection not implemented yet
        std::string expectedMessage = "Box::intersection not implemented yet";
        assert(std::string(e.what()) == expectedMessage);
        std::cout << "Note: Box intersection not yet implemented (expected)" << std::endl;
    }
    
    // Test non-intersecting boxes
    Vector3D origin3(10, 10, 10);
    Box box3(origin3, 1.0, 1.0, 1.0, normal1);
    assert(!box1.intersects(box3));
    
    // Test intersection calculation for non-intersecting boxes - also expect exception
    try {
        auto noIntersection = box1.intersectionPoints(box3);
        // If intersection is implemented, it should return nullopt for non-intersecting boxes
        assert(!noIntersection.has_value());
        std::cout << "Note: Box intersection correctly handles non-intersecting boxes" << std::endl;
    } catch (const std::runtime_error& e) {
        // Expected behavior: intersection not implemented yet
        std::string expectedMessage = "Box::intersection not implemented yet";
        assert(std::string(e.what()) == expectedMessage);
    }
}

void testBoxTransformations() {
    Vector3D origin(1, 1, 1);
    Vector3D normal(0, 0, 1);
    Box box(origin, 2.0, 2.0, 2.0, normal);
    
    // Test translation
    Vector3D offset(3, 4, 5);
    Box translated = box.translate(offset);
    Vector3D expectedNewOrigin = origin + offset;
    assert(isEqual(translated.getOrigin(), expectedNewOrigin));
    assert(isEqual(translated.getWidth(), box.getWidth()));
    assert(isEqual(translated.getHeight(), box.getHeight()));
    assert(isEqual(translated.getDepth(), box.getDepth()));
    
    // Test expansion
    Box expanded = box.expand(1.0);
    Vector3D expectedExpandedOrigin = origin - Vector3D(1, 1, 1);
    assert(isEqual(expanded.getOrigin(), expectedExpandedOrigin));
    assert(isEqual(expanded.getWidth(), 4.0));  // 2.0 + 2*1.0
    assert(isEqual(expanded.getHeight(), 4.0));
    assert(isEqual(expanded.getDepth(), 4.0));
}

void testBoxSetters() {
    Vector3D origin(0, 0, 0);
    Vector3D normal(0, 0, 1);
    Box box(origin, 1.0, 1.0, 1.0, normal);
    
    // Test setOrigin
    Vector3D newOrigin(5, 6, 7);
    box.setOrigin(newOrigin);
    assert(isEqual(box.getOrigin(), newOrigin));
    
    // Test setDimensions
    box.setDimensions(3.0, 4.0, 5.0);
    assert(isEqual(box.getWidth(), 3.0));
    assert(isEqual(box.getHeight(), 4.0));
    assert(isEqual(box.getDepth(), 5.0));
    
    // Test setNormal
    Vector3D newNormal(1, 1, 1);
    box.setNormal(newNormal);
    assert(isEqual(box.getNormal(), newNormal.normal()));
    assert(isEqual(box.getNormal().length(), 1.0));
}

void testBoxValidation() {
    Vector3D origin(0, 0, 0);
    Vector3D normal(0, 0, 1);
    
    // Test valid box
    Box validBox(origin, 1.0, 2.0, 3.0, normal);
    assert(validBox.isValid());
    
    // Test invalid boxes
    Box invalidBox1(origin, 0.0, 2.0, 3.0, normal);  // Zero width
    Box invalidBox2(origin, 1.0, -2.0, 3.0, normal); // Negative height
    Box invalidBox3(origin, 1.0, 2.0, 0.0, normal);  // Zero depth
    
    assert(!invalidBox1.isValid());
    assert(!invalidBox2.isValid());
    assert(!invalidBox3.isValid());
}

void testBoxNormals() {
    // Axis-aligned box (normal along Z)
    Vector3D origin(0,0,0);
    Vector3D znormal(0,0,1);
    Box box(origin, 2.0, 2.0, 2.0, znormal);

    // Face centers
    Vector3D minXCenter(0.0, 1.0, 1.0);
    Vector3D maxXCenter(2.0, 1.0, 1.0);
    Vector3D minYCenter(1.0, 0.0, 1.0);
    Vector3D maxYCenter(1.0, 2.0, 1.0);
    Vector3D minZCenter(1.0, 1.0, 0.0);
    Vector3D maxZCenter(1.0, 1.0, 2.0);

    assert(isEqual(box.getNormalAt(minXCenter), Vector3D(-1,0,0)));
    assert(isEqual(box.getNormalAt(maxXCenter), Vector3D(1,0,0)));
    assert(isEqual(box.getNormalAt(minYCenter), Vector3D(0,-1,0)));
    assert(isEqual(box.getNormalAt(maxYCenter), Vector3D(0,1,0)));
    assert(isEqual(box.getNormalAt(minZCenter), Vector3D(0,0,-1)));
    assert(isEqual(box.getNormalAt(maxZCenter), Vector3D(0,0,1)));

    // Oriented box: normal is diagonal; test Z-face normals map to world-space zAxis
    Vector3D diagNormal(1,1,1);
    Box obox(origin, 2.0, 2.0, 2.0, diagNormal);
    Vector3D zAxis = obox.getNormal().normal();
    // choose stable up as in implementation
    Vector3D worldUp = Vector3D::UNIT_Y;
    if (std::abs(zAxis.dot(worldUp)) > 0.99) worldUp = Vector3D::UNIT_X;
    Vector3D xAxis = worldUp.cross(zAxis).normal();
    Vector3D yAxis = zAxis.cross(xAxis).normal();

    // point on min Z face center in local coords (cx=w/2, cy=h/2, cz=0)
    Vector3D minZ_oriented = origin + xAxis*(obox.getWidth()/2.0) + yAxis*(obox.getHeight()/2.0) + zAxis*0.0;
    Vector3D maxZ_oriented = origin + xAxis*(obox.getWidth()/2.0) + yAxis*(obox.getHeight()/2.0) + zAxis*(obox.getDepth());

    Vector3D normalAtMinZ = obox.getNormalAt(minZ_oriented);
    Vector3D normalAtMaxZ = obox.getNormalAt(maxZ_oriented);

    assert(isEqual(normalAtMinZ, zAxis * -1.0));
    assert(isEqual(normalAtMaxZ, zAxis));
}
