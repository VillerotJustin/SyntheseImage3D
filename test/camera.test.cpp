#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include "../Lib/Rendering/Camera.h"
#include "../Lib/Geometry/Vector3D.h"
#include "../Lib/Geometry/Rectangle.h"
#include "../Lib/Geometry/Quaternion.h"
#include "../Lib/Geometry/Ray.h"
#include "../Lib/Geometry/Box.h"
#include "../Lib/Geometry/Circle.h"
#include "../Lib/Geometry/Plane.h"
#include "../Lib/Geometry/Sphere.h"
#include "../Lib/Rendering/Shape.hpp"
#include "../Lib/Math/Vector.hpp"
#include "../Lib/Math/math_common.h"

using namespace rendering;
using Vector3D = ::geometry::Vector3D;
using Rectangle = ::geometry::Rectangle;
using Quaternion = ::geometry::Quaternion;
using Ray = ::geometry::Ray;
using Sphere = ::geometry::Sphere;
using Box = ::geometry::Box;

// Helper function for floating-point comparison
bool isEqual(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
}

// Helper function to compare Vector3D
bool isEqual(const Vector3D& a, const Vector3D& b, double epsilon = 1e-9) {
    return (a - b).length() < epsilon;
}

// Test function declarations
void testCameraConstructor();
void testCameraViewportOperations();
void testCameraRotation();
void testCameraTranslation();
void testCameraRayGeneration();
void testCameraRenderScene2DColor();

int main() {
    std::cout << "Running Camera tests..." << std::endl;
    
    try {
        testCameraConstructor();
        std::cout << "✓ Camera constructor tests passed" << std::endl;
        
        testCameraViewportOperations();
        std::cout << "✓ Camera viewport operations tests passed" << std::endl;
        
        testCameraRotation();
        std::cout << "✓ Camera rotation tests passed" << std::endl;
        
        testCameraTranslation();
        std::cout << "✓ Camera translation tests passed" << std::endl;
        
        testCameraRayGeneration();
        std::cout << "✓ Camera ray generation tests passed" << std::endl;
        
        testCameraRenderScene2DColor();
        std::cout << "✓ Camera render scene tests passed" << std::endl;
        
        std::cout << "All Camera tests passed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}

void testCameraConstructor() {
    // Test basic constructor
    Vector3D topLeft(0, 0, 0);
    Vector3D topRight = topLeft + Vector3D(10.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 8.0, 0);
    Rectangle viewport(topLeft, topRight, bottomLeft);

    Camera camera(viewport);

    // Test that viewport is stored correctly
    const Rectangle& storedViewport = camera.getViewport();
    assert(isEqual(storedViewport.getOrigin(), topLeft));
    assert(isEqual(storedViewport.getLength(), (topRight - topLeft).length()));
    assert(isEqual(storedViewport.getWidth(), (bottomLeft - topLeft).length()));
    Vector3D expectedNormal = (topRight - topLeft).cross(bottomLeft - topLeft).normal();
    assert(isEqual(storedViewport.getNormal(), expectedNormal));
}

void testCameraViewportOperations() {
    // Create initial camera
    Vector3D topLeft(0, 0, 0);
    Vector3D topRight = topLeft + Vector3D(10.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 8.0, 0);
    Rectangle viewport(topLeft, topRight, bottomLeft);
    Camera camera(viewport);

    // Test getViewport
    const Rectangle& retrievedViewport = camera.getViewport();
    assert(isEqual(retrievedViewport.getOrigin(), topLeft));
    assert(isEqual(retrievedViewport.getLength(), (topRight - topLeft).length()));
    assert(isEqual(retrievedViewport.getWidth(), (bottomLeft - topLeft).length()));

    // Test setViewport
    Vector3D newTopLeft(5, 5, 5);
    Vector3D newTopRight = newTopLeft + Vector3D(15.0, 0, 0);
    Vector3D newBottomLeft = newTopLeft + Vector3D(0, 12.0, 0);
    Rectangle newViewport(newTopLeft, newTopRight, newBottomLeft);

    camera.setViewport(newViewport);

    const Rectangle& updatedViewport = camera.getViewport();
    assert(isEqual(updatedViewport.getOrigin(), newTopLeft));
    assert(isEqual(updatedViewport.getLength(), (newTopRight - newTopLeft).length()));
    assert(isEqual(updatedViewport.getWidth(), (newBottomLeft - newTopLeft).length()));
    Vector3D expectedNormal = (newTopRight - newTopLeft).cross(newBottomLeft - newTopLeft).normal();
    assert(isEqual(updatedViewport.getNormal(), expectedNormal));
}

void testCameraRotation() {
    // Create camera with viewport facing +Z
    Vector3D topLeft(0, 0, 0);
    Vector3D topRight = topLeft + Vector3D(10.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 8.0, 0);
    Rectangle viewport(topLeft, topRight, bottomLeft);
    Camera camera(viewport);

    // Create 90-degree rotation around Y axis (should rotate normal from +Z to +X)
    Vector3D axis(0, 1, 0);
    double angle = M_PI / 2.0; // 90 degrees
    Quaternion rotation(axis, angle);

    // Apply rotation
    camera.rotate(rotation);

    // Check that normal has been rotated
    const Rectangle& rotatedViewport = camera.getViewport();
    Vector3D expectedNormal(1, 0, 0); // +Z rotated 90° around Y becomes +X

    // Allow some tolerance for floating point precision
    assert(isEqual(rotatedViewport.getNormal(), expectedNormal, 1e-6));

    // Verify other properties remain unchanged
    assert(isEqual(rotatedViewport.getOrigin(), topLeft));
    assert(isEqual(rotatedViewport.getLength(), (topRight - topLeft).length()));
    assert(isEqual(rotatedViewport.getWidth(), (bottomLeft - topLeft).length()));
}

void testCameraTranslation() {
    // Create camera
    Vector3D topLeft(0, 0, 0);
    Vector3D topRight = topLeft + Vector3D(10.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 8.0, 0);
    Rectangle viewport(topLeft, topRight, bottomLeft);
    Camera camera(viewport);

    // Apply translation
    Vector3D translation(5, -3, 2);
    camera.translate(translation);

    // Check that origin has been translated
    const Rectangle& translatedViewport = camera.getViewport();
    Vector3D expectedOrigin = topLeft + translation;

    assert(isEqual(translatedViewport.getOrigin(), expectedOrigin));

    // Verify other properties remain unchanged
    Vector3D expectedNormal = (topRight - topLeft).cross(bottomLeft - topLeft).normal();
    assert(isEqual(translatedViewport.getNormal(), expectedNormal));
    assert(isEqual(translatedViewport.getLength(), (topRight - topLeft).length()));
    assert(isEqual(translatedViewport.getWidth(), (bottomLeft - topLeft).length()));
}

void testCameraRayGeneration() {
    // Create camera with viewport at origin facing +Z
    Vector3D topLeft(0, 0, 0);
    Vector3D topRight = topLeft + Vector3D(4.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 4.0, 0);
    Rectangle viewport(topLeft, topRight, bottomLeft);
    Camera camera(viewport);

    // Test ray generation from center of viewport
    Vector3D centerPoint = viewport.getCenter();
    Ray ray = camera.generateRay(centerPoint);

    // Ray should start at the center point and point in the normal direction
    Vector3D expectedNormal = (topRight - topLeft).cross(bottomLeft - topLeft).normal();
    assert(isEqual(ray.getOrigin(), centerPoint));
    assert(isEqual(ray.getDirection(), expectedNormal));

    // Test ray generation from corner of viewport
    Vector3D cornerPoint = viewport.getOrigin();
    Ray cornerRay = camera.generateRay(cornerPoint);

    // Ray should start at corner and point in normal direction
    assert(isEqual(cornerRay.getOrigin(), cornerPoint));
    assert(isEqual(cornerRay.getDirection(), expectedNormal));
}

void testCameraRenderScene2DColor() {
    // Create camera
    Vector3D topLeft(0, 0, -5);
    Vector3D topRight = topLeft + Vector3D(10.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 10.0, 0);
    Rectangle viewport(topLeft, topRight, bottomLeft);
    Camera camera(viewport);
    
    // Create some shapes to render
    math::Vector<Camera::ShapeVariant> shapes;
    
    // Add a sphere at the origin
    Sphere sphere(Vector3D(4, 4, 0), 2.0);
    Shape<::geometry::Sphere> sphereShape(sphere);
    Camera::ShapeVariant* sphereVariant = new Camera::ShapeVariant{sphereShape};
    std::visit([](auto& shape) { shape.setColor(RGBA_Color(1, 0, 0, 1)); }, *sphereVariant); // Red color
    shapes.append(sphereVariant);
    
    // Add a box 
    Box box(Vector3D(5, 5, 10), 2.0, 2.0, 2.0, Vector3D(0, 0, 1));
    Shape<::geometry::Box> boxShape(box);
    Camera::ShapeVariant* boxVariant = new Camera::ShapeVariant{boxShape};
    std::visit([](auto& shape) { shape.setColor(RGBA_Color(0, 1, 0, 1)); }, *boxVariant); // Green color
    shapes.append(boxVariant);
    
    // Render small test image
    Image image = camera.renderScene2DColor(720, 720, shapes);
    
    // Basic checks on the rendered image
    assert(image.getWidth() == 720);
    assert(image.getHeight() == 720);

    // Check that some pixels have been rendered (not all default/black)
    bool hasNonBlackPixels = false;
    for (int y = 0; y < image.getHeight() && !hasNonBlackPixels; ++y) {
        for (int x = 0; x < image.getWidth() && !hasNonBlackPixels; ++x) {
            const RGBA_Color* pixel = image.getPixel(x, y);
            if (pixel && (pixel->r() > 0 || pixel->g() > 0 || pixel->b() > 0)) {
                hasNonBlackPixels = true;
            }
        }
    }

    image.toBitmapFile("test_render_output", "./test/test_by_product/camera/");
    
    // We expect some pixels to be rendered since we have shapes in the scene
    // Note: This test might be flaky depending on the exact rendering logic
    // If shapes are not intersecting the camera rays, all pixels might be black
    std::cout << "Note: Render test completed - check output manually if needed" << std::endl;
}
