#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <optional>
#include "../Lib/Geometry/Sphere.h"
#include "../Lib/Geometry/Circle.h"
#include "../Lib/Geometry/Plane.h"
#include "../Lib/Geometry/Ray.h"
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
void testSphereConstructor();
void testSphereBasicProperties();
void testSphereVolumeAndArea();
void testSpherePointOperations();
void testSphereIntersections();
void testSphereRayIntersections();
void testSphereLineIntersections();
void testSphereTransformations();
void testSphereAdvancedOperations();
void testSphereRandomPoints();
void testSphereComparisons();
void testSphereValidation();

int main() {
    std::cout << "=== Sphere Test Suite ===" << std::endl;
    
    try {
        testSphereConstructor();
        std::cout << "✓ Sphere constructor test passed" << std::endl;
        
        testSphereBasicProperties();
        std::cout << "✓ Sphere basic properties test passed" << std::endl;
        
        testSphereVolumeAndArea();
        std::cout << "✓ Sphere volume and surface area test passed" << std::endl;
        
        testSpherePointOperations();
        std::cout << "✓ Sphere point operations test passed" << std::endl;
        
        testSphereIntersections();
        std::cout << "✓ Sphere intersections test passed" << std::endl;
        
        testSphereRayIntersections();
        std::cout << "✓ Sphere ray intersections test passed" << std::endl;
        
        testSphereLineIntersections();
        std::cout << "✓ Sphere line intersections test passed" << std::endl;
        
        testSphereTransformations();
        std::cout << "✓ Sphere transformations test passed" << std::endl;
        
        testSphereAdvancedOperations();
        std::cout << "✓ Sphere advanced operations test passed" << std::endl;
        
        testSphereRandomPoints();
        std::cout << "✓ Sphere random points test passed" << std::endl;
        
        testSphereComparisons();
        std::cout << "✓ Sphere comparisons test passed" << std::endl;
        
        testSphereValidation();
        std::cout << "✓ Sphere validation test passed" << std::endl;
        
        std::cout << "\n🎉 All Sphere tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void testSphereConstructor() {
    Vector3D center(1, 2, 3);
    double radius = 5.0;
    
    Sphere sphere(center, radius);
    
    assert(isEqual(sphere.getCenter(), center));
    assert(isEqual(sphere.getRadius(), radius));
}

void testSphereBasicProperties() {
    Vector3D center(0, 0, 0);
    double radius = 3.0;
    
    Sphere sphere(center, radius);
    
    // Test basic getters
    assert(isEqual(sphere.getCenter(), center));
    assert(isEqual(sphere.getRadius(), radius));
}

void testSphereVolumeAndArea() {
    Vector3D center(5, -2, 10);
    double radius = 2.0;
    
    Sphere sphere(center, radius);
    
    // Test surface area: 4πr²
    double expectedArea = 4 * math::pi * radius * radius;
    assert(isEqual(sphere.getSurfaceArea(), expectedArea));
    
    // Test volume: (4/3)πr³
    double expectedVolume = (4.0 / 3.0) * math::pi * radius * radius * radius;
    assert(isEqual(sphere.getVolume(), expectedVolume));
}

void testSpherePointOperations() {
    Vector3D center(0, 0, 0);
    double radius = 2.0;
    
    Sphere sphere(center, radius);
    
    // Test points inside sphere
    Vector3D pointInside1(0, 0, 0);     // Center
    Vector3D pointInside2(1, 1, 0);     // Distance sqrt(2) < 2
    Vector3D pointInside3(1, 0, 0);     // Distance 1 < 2
    
    assert(sphere.containsPoint(pointInside1));
    assert(sphere.containsPoint(pointInside2));
    assert(sphere.containsPoint(pointInside3));
    
    // Test points on sphere surface
    Vector3D pointOnSurface1(2, 0, 0);  // Distance 2 = radius
    Vector3D pointOnSurface2(0, 2, 0);
    Vector3D pointOnSurface3(0, 0, -2);
    
    assert(sphere.surfacePoint(pointOnSurface1));
    assert(sphere.surfacePoint(pointOnSurface2));
    assert(sphere.surfacePoint(pointOnSurface3));
    
    // Test points outside sphere
    Vector3D pointOutside1(3, 0, 0);    // Distance 3 > 2
    Vector3D pointOutside2(2, 2, 0);    // Distance sqrt(8) > 2
    
    assert(!sphere.containsPoint(pointOutside1));
    assert(!sphere.containsPoint(pointOutside2));
    assert(!sphere.surfacePoint(pointOutside1));
    assert(!sphere.surfacePoint(pointOutside2));
    
    // Test distance calculations
    Vector3D testPoint(5, 0, 0);
    double expectedDistance = 5.0 - 2.0; // point distance - radius
    assert(isEqual(sphere.distanceToPoint(testPoint), expectedDistance));
    
    // Test closest point on surface
    Vector3D closestPoint = sphere.closestPointOnSurface(testPoint);
    Vector3D expectedClosest(2, 0, 0);   // On surface toward test point
    assert(isEqual(closestPoint, expectedClosest));
}

void testSphereIntersections() {
    Vector3D center1(0, 0, 0);
    double radius1 = 3.0;
    Sphere sphere1(center1, radius1);
    
    // Test intersecting spheres
    Vector3D center2(4, 0, 0);
    double radius2 = 2.0;
    Sphere sphere2(center2, radius2);
    
    assert(sphere1.intersects(sphere2)); // Distance 4 < 3+2=5
    
    // Test non-intersecting spheres
    Vector3D center3(10, 0, 0);
    double radius3 = 1.0;
    Sphere sphere3(center3, radius3);
    
    assert(!sphere1.intersects(sphere3)); // Distance 10 > 3+1=4
    
    // Test tangent spheres (external)
    Vector3D center4(5, 0, 0);
    double radius4 = 2.0;
    Sphere sphere4(center4, radius4);
    
    assert(sphere1.intersects(sphere4)); // Distance 5 = 3+2
    
    // Test one sphere inside another
    Vector3D center5(1, 0, 0);
    double radius5 = 1.0;
    Sphere sphere5(center5, radius5);
    
    assert(!sphere1.intersects(sphere5)); // sphere5 is inside sphere1
    
    // Test intersection points (should return Circle for intersecting spheres)
    auto intersection = sphere1.intersectionPoints(sphere2);
    assert(intersection.has_value());
    // Should be a Circle for intersecting spheres
    assert(std::holds_alternative<Circle>(intersection.value()));

    // For non-intersecting spheres
    auto noIntersection = sphere1.intersectionPoints(sphere3);
    assert(!noIntersection.has_value());

    // Test tangent spheres (should return Vector3D)
    auto tangentIntersection = sphere1.intersectionPoints(sphere4);
    assert(tangentIntersection.has_value());
    assert(std::holds_alternative<Vector3D>(tangentIntersection.value()));

    // Test identical spheres (should return Sphere)
    Sphere identicalSphere(center1, radius1);
    auto identicalIntersection = sphere1.intersectionPoints(identicalSphere);
    assert(identicalIntersection.has_value());
    assert(std::holds_alternative<Sphere>(identicalIntersection.value()));
}

void testSphereRayIntersections() {
    Vector3D center(0, 0, 0);
    double radius = 2.0;
    Sphere sphere(center, radius);
    
    // Test ray that intersects sphere
    Vector3D rayOrigin(-5, 0, 0);
    Vector3D rayDirection(1, 0, 0);
    Ray ray1(rayOrigin, rayDirection);
    
    assert(sphere.rayIntersect(ray1));
    
    // Test ray that misses sphere
    Vector3D rayOrigin2(-5, 5, 0);  // Too high
    Ray ray2(rayOrigin2, rayDirection);
    
    assert(!sphere.rayIntersect(ray2));
    
    // Test ray starting inside sphere
    Vector3D rayOrigin3(0, 0, 0);   // At center
    Ray ray3(rayOrigin3, rayDirection);
    
    assert(sphere.rayIntersect(ray3));

    // Test ray recover hit 
    auto hit = sphere.rayIntersectionHit(ray1);

    // check if hit is Vector3D (should be at x = -2, y = 0, z = 0)
    assert(hit.has_value());
    assert(isEqual(hit.value().x(), -2.0));
    assert(isEqual(hit.value().y(), 0.0));
    assert(isEqual(hit.value().z(), 0.0));

    // Check if no hit (ray2 misses the sphere)
    auto missHit = sphere.rayIntersectionHit(ray2);
    assert(!missHit.has_value());
}

void testSphereLineIntersections() {
    Vector3D center(0, 0, 0);
    double radius = 2.0;
    Sphere sphere(center, radius);
    
    // Test line that intersects sphere
    Vector3D linePoint(0, 0, 0);     // Line through center
    Vector3D lineDirection(1, 0, 0);
    
    assert(sphere.lineIntersects(linePoint, lineDirection));
    
    // Test line that misses sphere
    Vector3D linePoint2(0, 5, 0);    // Line too far away
    
    assert(!sphere.lineIntersects(linePoint2, lineDirection));
    
    // Test line tangent to sphere
    Vector3D linePoint3(0, 2, 0);    // Line tangent at top
    
    assert(sphere.lineIntersects(linePoint3, lineDirection));
}

void testSphereTransformations() {
    Vector3D center(1, 2, 3);
    double radius = 2.0;
    Sphere sphere(center, radius);
    
    // Test translation
    Vector3D translation(5, -3, 2);
    sphere.translate(translation);
    
    Vector3D expectedNewCenter = center + translation;
    assert(isEqual(sphere.getCenter(), expectedNewCenter));
    assert(isEqual(sphere.getRadius(), radius)); // Radius unchanged
    
    // Test scaling
    double scaleFactor = 1.5;
    sphere.scale(scaleFactor);
    
    double expectedNewRadius = radius * scaleFactor;
    assert(isEqual(sphere.getRadius(), expectedNewRadius));
    
    // Test invalid scaling (should be ignored)
    double originalRadius = sphere.getRadius();
    sphere.scale(-2.0);  // Negative scale should be ignored
    assert(isEqual(sphere.getRadius(), originalRadius));
}

void testSphereAdvancedOperations() {
    Vector3D center(0, 0, 0);
    double radius = 3.0;
    Sphere sphere(center, radius);
    
    // Test normal calculation at surface point
    Vector3D surfacePoint(3, 0, 0);
    Vector3D normal = sphere.normalAt(surfacePoint);
    Vector3D expectedNormal(1, 0, 0);  // Should point outward
    assert(isEqual(normal, expectedNormal));
    
    // Test tangent plane at surface point
    Plane tangentPlane = sphere.tangentPlaneAtPoint(surfacePoint);
    assert(isEqual(tangentPlane.getOrigin(), surfacePoint));
    assert(isEqual(tangentPlane.getNormal(), expectedNormal));
    
    // Test point projection onto surface
    Vector3D externalPoint(6, 0, 0);
    Vector3D projected = sphere.projectPointOntoSurface(externalPoint);
    Vector3D expectedProjected(3, 0, 0);
    assert(isEqual(projected, expectedProjected));
    
    // Test bounding box
    Box boundingBox = sphere.getBoundingBox();
    Vector3D expectedMin(-3, -3, -3);
    Vector3D expectedMax(3, 3, 3);
    assert(isEqual(boundingBox.getMinCorner(), expectedMin));
    assert(isEqual(boundingBox.getMaxCorner(), expectedMax));
}

void testSphereRandomPoints() {
    Vector3D center(5, 5, 5);
    double radius = 2.0;
    Sphere sphere(center, radius);
    
    // Test random points on surface
    for (int i = 0; i < 10; i++) {
        Vector3D randomSurface = sphere.randomPointOnSurface();
        
        // Should be approximately on the surface
        double distanceFromCenter = (randomSurface - center).length();
        assert(isEqual(distanceFromCenter, radius, 1e-6));
    }
    
    // Test random points inside sphere
    for (int i = 0; i < 10; i++) {
        Vector3D randomInside = sphere.randomPointInside();
        
        // Should be inside or on the sphere
        assert(sphere.containsPoint(randomInside) || sphere.surfacePoint(randomInside));
    }
}

void testSphereComparisons() {
    Vector3D center1(1, 2, 3);
    Vector3D center2(1, 2, 3);
    Vector3D center3(4, 5, 6);
    double radius1 = 2.0;
    double radius2 = 2.0;
    double radius3 = 3.0;
    
    Sphere sphere1(center1, radius1);
    Sphere sphere2(center2, radius2);  // Same as sphere1
    Sphere sphere3(center3, radius3);  // Different
    
    // Test equality operators
    assert(sphere1 == sphere2);
    assert(!(sphere1 == sphere3));
    assert(sphere1 != sphere3);
    assert(!(sphere1 != sphere2));
    
    // Test equals with tolerance
    Vector3D nearCenter(1.000001, 2.000001, 3.000001);
    double nearRadius = 2.000001;
    Sphere nearSphere(nearCenter, nearRadius);
    
    assert(sphere1.equals(nearSphere, 1e-5));
    assert(!sphere1.equals(nearSphere, 1e-7));
}

void testSphereValidation() {
    Vector3D center(0, 0, 0);
    
    // Test valid sphere
    Sphere validSphere(center, 1.0);
    assert(validSphere.isValid());
    
    // Test invalid spheres
    try {
        Sphere invalidSphere1(center, 0.0);   // Zero radius
        assert(!invalidSphere1.isValid());
    } catch (const std::invalid_argument&) {
        // Expected exception for zero radius
    }

    try {
        Sphere invalidSphere2(center, -1.0);  // Negative radius
        assert(!invalidSphere2.isValid());
    } catch (const std::invalid_argument&) {
        // Expected exception for negative radius
    }
}
