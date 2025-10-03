#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include "../Lib/Geometry/Ray.h"
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
void testRayConstructor();
void testRayBasicProperties();
void testRayParametricPoints();
void testRayClosestPoint();
void testRayDistances();
void testRayContainment();
void testRayTransformations();
void testRayParallelism();
void testRayIntersections();
void testRaySetters();
void testRayValidation();

int main() {
    std::cout << "=== Ray Test Suite ===" << std::endl;
    
    try {
        testRayConstructor();
        std::cout << "✓ Ray constructor test passed" << std::endl;
        
        testRayBasicProperties();
        std::cout << "✓ Ray basic properties test passed" << std::endl;
        
        testRayParametricPoints();
        std::cout << "✓ Ray parametric points test passed" << std::endl;
        
        testRayClosestPoint();
        std::cout << "✓ Ray closest point test passed" << std::endl;
        
        testRayDistances();
        std::cout << "✓ Ray distance calculations test passed" << std::endl;
        
        testRayContainment();
        std::cout << "✓ Ray point containment test passed" << std::endl;
        
        testRayTransformations();
        std::cout << "✓ Ray transformations test passed" << std::endl;
        
        testRayParallelism();
        std::cout << "✓ Ray parallelism test passed" << std::endl;
        
        testRayIntersections();
        std::cout << "✓ Ray intersections test passed" << std::endl;
        
        testRaySetters();
        std::cout << "✓ Ray setters test passed" << std::endl;
        
        testRayValidation();
        std::cout << "✓ Ray validation test passed" << std::endl;
        
        std::cout << "\n🎉 All Ray tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void testRayConstructor() {
    Vector3D origin(1, 2, 3);
    Vector3D direction(2, 0, 0);  // Will be normalized to (1, 0, 0)
    
    Ray ray(origin, direction);
    
    assert(isEqual(ray.getOrigin(), origin));
    assert(isEqual(ray.getDirection(), direction.normalized()));
    assert(isEqual(ray.getDirection().length(), 1.0));
}

void testRayBasicProperties() {
    Vector3D origin(0, 0, 0);
    Vector3D direction(3, 4, 0);  // Length 5, normalized to (0.6, 0.8, 0)
    
    Ray ray(origin, direction);
    
    // Test that direction is normalized
    assert(isEqual(ray.getDirection().length(), 1.0));
    
    Vector3D expectedDirection(0.6, 0.8, 0);
    assert(isEqual(ray.getDirection(), expectedDirection));
}

void testRayParametricPoints() {
    Vector3D origin(1, 2, 3);
    Vector3D direction(1, 0, 0);  // Unit vector in X direction
    
    Ray ray(origin, direction);
    
    // Test points at various parameters
    Vector3D point0 = ray.getPointAt(0);      // Should be origin
    Vector3D point1 = ray.getPointAt(1);      // Should be origin + direction
    Vector3D point5 = ray.getPointAt(5);      // Should be origin + 5*direction
    Vector3D pointNeg = ray.getPointAt(-2);   // Should be origin - 2*direction
    
    assert(isEqual(point0, origin));
    assert(isEqual(point1, Vector3D(2, 2, 3)));
    assert(isEqual(point5, Vector3D(6, 2, 3)));
    assert(isEqual(pointNeg, Vector3D(-1, 2, 3)));
}

void testRayClosestPoint() {
    Vector3D origin(0, 0, 0);
    Vector3D direction(1, 0, 0);  // Ray along X-axis
    
    Ray ray(origin, direction);
    
    // Test point that projects onto ray
    Vector3D point1(3, 5, 0);  // Projects to (3, 0, 0)
    Vector3D closest1 = ray.closestPointTo(point1);
    double param1 = ray.closestParameterTo(point1);
    
    assert(isEqual(closest1, Vector3D(3, 0, 0)));
    assert(isEqual(param1, 3.0));
    
    // Test point behind the ray origin
    Vector3D point2(-2, 3, 0);  // Would project to (-2, 0, 0) but clamped to origin
    Vector3D closest2 = ray.closestPointTo(point2);
    double param2 = ray.closestParameterTo(point2);
    
    assert(isEqual(closest2, origin));  // Clamped to ray start
    assert(isEqual(param2, 0.0));       // Parameter clamped to 0
    
    // Test point on the ray
    Vector3D point3(4, 0, 0);
    Vector3D closest3 = ray.closestPointTo(point3);
    assert(isEqual(closest3, point3));
}

void testRayDistances() {
    Vector3D origin(0, 0, 0);
    Vector3D direction(1, 0, 0);
    Ray ray(origin, direction);
    
    // Test distance to point perpendicular to ray
    Vector3D point1(2, 3, 0);  // Distance should be 3
    assert(isEqual(ray.distanceToPoint(point1), 3.0));
    
    // Test distance to point on ray
    Vector3D point2(5, 0, 0);  // Distance should be 0
    assert(isEqual(ray.distanceToPoint(point2), 0.0));
    
    // Test distance to point behind ray
    Vector3D point3(-1, 4, 0);  // Distance to origin (closest point on ray)
    double expectedDistance = (point3 - origin).length();
    assert(isEqual(ray.distanceToPoint(point3), expectedDistance));
}

void testRayContainment() {
    Vector3D origin(1, 1, 1);
    Vector3D direction(0, 1, 0);
    Ray ray(origin, direction);
    
    // Test points on the ray
    Vector3D pointOnRay1 = ray.getPointAt(0);     // Origin
    Vector3D pointOnRay2 = ray.getPointAt(3);     // 3 units along ray
    Vector3D pointOnRay3 = ray.getPointAt(10);    // 10 units along ray
    
    assert(ray.containsPoint(pointOnRay1));
    assert(ray.containsPoint(pointOnRay2));
    assert(ray.containsPoint(pointOnRay3));
    
    // Test points not on the ray
    Vector3D pointOffRay1(1, 1, 2);     // Offset in Z
    Vector3D pointOffRay2(2, 2, 1);     // Offset in X
    Vector3D pointBehind = ray.getPointAt(-1);  // Behind ray origin
    
    assert(!ray.containsPoint(pointOffRay1));
    assert(!ray.containsPoint(pointOffRay2));
    assert(!ray.containsPoint(pointBehind));
}

void testRayTransformations() {
    Vector3D origin(2, 3, 4);
    Vector3D direction(0, 0, 1);
    Ray ray(origin, direction);
    
    // Test translation
    Vector3D offset(1, -2, 5);
    Ray translated = ray.translate(offset);
    
    Vector3D expectedNewOrigin = origin + offset;
    assert(isEqual(translated.getOrigin(), expectedNewOrigin));
    assert(isEqual(translated.getDirection(), ray.getDirection()));
    
    // Verify original ray unchanged
    assert(isEqual(ray.getOrigin(), origin));
}

void testRayParallelism() {
    Vector3D origin1(0, 0, 0);
    Vector3D direction1(1, 1, 0);
    Ray ray1(origin1, direction1);
    
    // Test parallel ray (same direction)
    Vector3D origin2(5, 5, 0);
    Vector3D direction2(2, 2, 0);  // Same direction, different magnitude
    Ray ray2(origin2, direction2);
    
    assert(ray1.isParallelTo(ray2));
    
    // Test parallel ray (opposite direction)
    Vector3D direction3(-1, -1, 0);
    Ray ray3(origin2, direction3);
    
    assert(ray1.isParallelTo(ray3));
    
    // Test non-parallel ray
    Vector3D direction4(1, 0, 1);
    Ray ray4(origin2, direction4);
    
    assert(!ray1.isParallelTo(ray4));
}

void testRayIntersections() {
    // Test closest points between two rays
    Vector3D origin1(0, 0, 0);
    Vector3D direction1(1, 0, 0);  // Ray along X-axis
    Ray ray1(origin1, direction1);
    
    Vector3D origin2(2, 2, 0);
    Vector3D direction2(0, -1, 0); // Ray along negative Y-axis
    Ray ray2(origin2, direction2);
    
    double t1, t2;
    double distance = ray1.closestPointsBetweenRays(ray2, t1, t2);
    
    // Rays should intersect at (2, 0, 0)
    Vector3D point1 = ray1.getPointAt(t1);
    Vector3D point2 = ray2.getPointAt(t2);
    
    assert(isEqual(point1, Vector3D(2, 0, 0)));
    assert(isEqual(point2, Vector3D(2, 0, 0)));
    assert(isEqual(distance, 0.0));
    
    // Test skew rays (don't intersect)
    Vector3D origin3(0, 0, 1);
    Vector3D direction3(1, 0, 0);
    Ray ray3(origin3, direction3);
    
    double t1_skew, t2_skew;
    double distance_skew = ray1.closestPointsBetweenRays(ray3, t1_skew, t2_skew);
    
    assert(distance_skew > 0);  // Should have non-zero distance
    assert(isEqual(distance_skew, 1.0));  // Distance should be 1 (Z offset)
}

void testRaySetters() {
    Vector3D origin(1, 2, 3);
    Vector3D direction(0, 0, 1);
    Ray ray(origin, direction);
    
    // Test setOrigin
    Vector3D newOrigin(-1, -2, -3);
    ray.setOrigin(newOrigin);
    assert(isEqual(ray.getOrigin(), newOrigin));
    
    // Test setDirection
    Vector3D newDirection(3, 4, 0);  // Will be normalized
    ray.setDirection(newDirection);
    Vector3D expectedDirection = newDirection.normalized();
    assert(isEqual(ray.getDirection(), expectedDirection));
    assert(isEqual(ray.getDirection().length(), 1.0));
}

void testRayValidation() {
    Vector3D origin(0, 0, 0);
    
    // Test valid ray
    Vector3D validDirection(1, 1, 1);
    Ray validRay(origin, validDirection);
    assert(validRay.isValid());
    
    // Test invalid ray with zero direction
    Vector3D zeroDirection(0, 0, 0);
    Ray invalidRay(origin, zeroDirection);
    assert(!invalidRay.isValid());
    
    // Test ray that becomes invalid after setting zero direction
    validRay.setDirection(zeroDirection);
    assert(!validRay.isValid());
}
