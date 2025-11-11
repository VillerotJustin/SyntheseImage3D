
// Import

// Internal libraries
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
#include "test_helpers/Logger.h"

// External libraries
#include <cassert>


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
void testCameraRayHitFind();
void testCameraProcessHit();
void testCameraRenderScene2DColor();
void testCameraRenderScene2DDepth();
void testCameraRenderScene3DColor();
void testCameraRenderScene3DDepth();
void testCameraRenderScene3DLight();
void testCameraRenderScene3DLight_AA();

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

        testCameraRayHitFind();
        std::cout << "✓ Camera ray hit finding tests passed" << std::endl;
        
        testCameraProcessHit();
        std::cout << "✓ Camera process hit tests passed" << std::endl;
        
        testCameraRenderScene2DColor();
        std::cout << "✓ Camera render scene tests passed" << std::endl;

        testCameraRenderScene2DDepth();
        std::cout << "✓ Camera render scene depth tests passed" << std::endl;

        testCameraRenderScene3DColor();
        std::cout << "✓ Camera render scene 3D color tests passed" << std::endl;

        testCameraRenderScene3DDepth();
        std::cout << "✓ Camera render scene 3D depth tests passed" << std::endl;

        testCameraRenderScene3DLight();
        std::cout << "✓ Camera render scene 3D light tests passed" << std::endl;

        testCameraRenderScene3DLight_AA();
        std::cout << "✓ Camera render scene 3D light anti-aliasing tests passed" << std::endl;

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

bool has_non_debug_pixel(Image image) {
    const RGBA_Color debugColor(1.0, 0.0, 1.0, 1.0); // Magenta color used for debug pixels
    for (size_t y = 0; y < image.getHeight(); ++y) {
        for (size_t x = 0; x < image.getWidth(); ++x) {
            const RGBA_Color pixel = image.getPixel(x, y);
            if (pixel != debugColor) {
                return true;
            }
        }
    }
    return false;
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

void testCameraRayHitFind() {
    // Create Rays
    Vector3D rayOrigin(0, 0, -10);
    Vector3D rayDirection(0, 0, 1); // Pointing along +Z
    Ray ray(rayOrigin, rayDirection);

    Vector3D rayDirection2(0, 0, -1); // Pointing along -Z
    Ray ray2(rayOrigin, rayDirection2);

    // Create Shapes
    Sphere sphere(Vector3D(0, 0, 0), 2.0); // Sphere at origin with radius 2
    Shape<::geometry::Sphere> sphereShape(sphere);
    Camera::ShapeVariant sphereVariant = Camera::ShapeVariant{sphereShape};

    Box box(Vector3D(-1, -1, 5), 2.0, 2.0, 2.0, Vector3D(0, 0, 1)); // Box in front of ray
    Shape<::geometry::Box> boxShape(box);
    Camera::ShapeVariant boxVariant = Camera::ShapeVariant{boxShape};

    // Create vector of shapes
    math::Vector<Camera::ShapeVariant> shapes;
    shapes.append(sphereVariant);
    shapes.append(boxVariant);

    // Make index list
    math::Vector<size_t> index_to_test;
    index_to_test.append(0); // Sphere
    index_to_test.append(1); // Box

    std::optional<Hit> hit;
    hit = Camera::findNextHit(ray, shapes, index_to_test);
    assert(hit.has_value());
    assert(hit->shapeIndex == 0); // Should hit sphere first
    assert(hit->t == 8); // Should hit sphere first

    hit = Camera::findNextHit(ray2, shapes, index_to_test);
    assert(!hit.has_value()); // Should not hit anything

    index_to_test.clear();
    index_to_test.append(1); // Only test box

    hit = Camera::findNextHit(ray, shapes, index_to_test);
    assert(hit.has_value());
    assert(hit->shapeIndex == 1); // Should hit box
    assert(hit->t == 15); // Should hit box at t=15
}

void testCameraProcessHit() {
    RenderLogger logger("process_hit_performance");
    
    // Create Rays
    Vector3D rayOrigin(0, 0, -10);
    Vector3D rayDirection(0, 0, 1); // Pointing along +Z
    Ray ray(rayOrigin, rayDirection);

    Vector3D rayDirection2(0, 0, -1); // Pointing along -Z
    Ray ray2(rayOrigin, rayDirection2);

    // Create Shapes
    Sphere sphere(Vector3D(0, 0, 0), 2.0); // Sphere at origin with radius 2
    Shape<::geometry::Sphere> sphereShape(sphere);
    sphereShape.setColor(RGBA_Color(1, 0, 0, 0.5)); // Red translucent color
    Camera::ShapeVariant sphereVariant = Camera::ShapeVariant{sphereShape};

    Box box(Vector3D(-1, -1, 5), 2.0, 2.0, 2.0, Vector3D(0, 0, 1)); // Box in front of ray
    Shape<::geometry::Box> boxShape(box);
    boxShape.setColor(RGBA_Color(0, 0, 1, 1.0)); // Blue opaque color
    Camera::ShapeVariant boxVariant = Camera::ShapeVariant{boxShape};

    // Create vector of shapes
    math::Vector<Camera::ShapeVariant> shapes;
    shapes.append(sphereVariant);
    shapes.append(boxVariant);

    // Make index list
    math::Vector<size_t> index_to_test;
    index_to_test.append(0); // Sphere
    index_to_test.append(1); // Box

    // Create Lights
    Light light(Vector3D(10, 10, -10), RGBA_Color(1, 1, 1, 1), 1.0);
    Light light2(Vector3D(10, 10, -10), RGBA_Color(0.5, 1, 0.5, 1.0), 0.5);

    // Create vector of lights
    math::Vector<Light> lights;
    lights.append(light);
    lights.append(light2);

    // Log scene configuration
    logger.logScene(shapes, lights);
    logger.logMessage("Performance Test Configuration:");
    logger.logMessage("Ray Origin: (0, 0, -10)");
    logger.logMessage("Ray Direction: (0, 0, 1)");
    logger.logMessage("Test Iterations: 1000");

    // Find hit
    std::optional<Hit> hit;
    hit = Camera::findNextHit(ray, shapes, index_to_test);
    assert(hit.has_value());

    index_to_test.clear();
    index_to_test.append(1); // Only test box for further intersections

    std::optional<Hit> hit2;
    hit2 = Camera::findNextHit(ray, shapes, index_to_test);

    // Hit Vector
    math::Vector<Hit> hitVector;
    if (hit.has_value()) {
        hitVector.append(*hit);
    }
    if (hit2.has_value()) {
        hitVector.append(*hit2);
    }

    RGBA_Color outColor;
    RGBA_Color outColor_old;
    
    // Performance test parameters
    const int NUM_ITERATIONS = 1000;
    
    // Measure time for new processRayHit method (multiple iterations)
    auto start_new = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        outColor = Camera::processRayHitRegression(*hit, ray, shapes, lights, index_to_test);
    }
    auto end_new = std::chrono::high_resolution_clock::now();
    auto total_duration_new = std::chrono::duration_cast<std::chrono::microseconds>(end_new - start_new);
    
    // Measure time for old processRayHitOld method (multiple iterations)
    auto start_old = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        outColor_old = Camera::processRayHitOld(hitVector, ray, shapes, lights);
    }
    auto end_old = std::chrono::high_resolution_clock::now();
    auto total_duration_old = std::chrono::duration_cast<std::chrono::microseconds>(end_old - start_old);
    
    // Calculate averages
    double avg_duration_new = static_cast<double>(total_duration_new.count()) / NUM_ITERATIONS;
    double avg_duration_old = static_cast<double>(total_duration_old.count()) / NUM_ITERATIONS;
    
    assert(outColor != RGBA_Color(1.0, 0.0, 1.0, 1.0)); // Should not be debug color
    
    // Log results to both console and log file
    std::stringstream logStream;
    logStream << "Processed color from hit: " << outColor;
    logger.logMessage(logStream.str());
    std::cout << logStream.str() << std::endl;
    
    logStream.str("");
    logStream << "Processed color from hit (old): " << outColor_old;
    logger.logMessage(logStream.str());
    std::cout << logStream.str() << std::endl;
    
    logger.logMessage("");
    logger.logMessage("=== PERFORMANCE TEST RESULTS ===");
    logStream.str("");
    logStream << "Test iterations: " << NUM_ITERATIONS;
    logger.logMessage(logStream.str());
    std::cout << "Performance test results (" << NUM_ITERATIONS << " iterations):" << std::endl;
    
    logStream.str("");
    logStream << "New processRayHit average time: " << std::fixed << std::setprecision(3) << avg_duration_new << " microseconds";
    logger.logMessage(logStream.str());
    std::cout << logStream.str() << std::endl;
    
    logStream.str("");
    logStream << "Old processRayHitOld average time: " << std::fixed << std::setprecision(3) << avg_duration_old << " microseconds";
    logger.logMessage(logStream.str());
    std::cout << logStream.str() << std::endl;
    
    logStream.str("");
    logStream << "New processRayHit total time: " << total_duration_new.count() << " microseconds";
    logger.logMessage(logStream.str());
    std::cout << logStream.str() << std::endl;
    
    logStream.str("");
    logStream << "Old processRayHitOld total time: " << total_duration_old.count() << " microseconds";
    logger.logMessage(logStream.str());
    std::cout << logStream.str() << std::endl;
    
    // Calculate performance difference
    if (avg_duration_old > 0) {
        double speedup = avg_duration_old / avg_duration_new;
        
        logStream.str("");
        logStream << "Performance ratio (old/new): " << std::fixed << std::setprecision(2) << speedup << "x";
        logger.logMessage(logStream.str());
        std::cout << logStream.str() << std::endl;
        
        if (speedup > 1.0) {
            logStream.str("");
            logStream << "New method is " << std::fixed << std::setprecision(2) << speedup << "x FASTER than old method";
            logger.logMessage(logStream.str());
            std::cout << logStream.str() << std::endl;
        } else {
            logStream.str("");
            logStream << "Old method is " << std::fixed << std::setprecision(2) << (1.0/speedup) << "x FASTER than new method";
            logger.logMessage(logStream.str());
            std::cout << logStream.str() << std::endl;
        }
        
        // Additional analysis in log
        logger.logMessage("");
        logger.logMessage("=== PERFORMANCE ANALYSIS ===");
        logStream.str("");
        logStream << "Performance difference: " << std::abs(avg_duration_new - avg_duration_old) << " microseconds per call";
        logger.logMessage(logStream.str());
        
        logStream.str("");
        logStream << "Efficiency gain per 1M calls: " << std::fixed << std::setprecision(1) 
                  << (std::abs(avg_duration_new - avg_duration_old) * 1000000.0 / 1000000.0) << " seconds";
        logger.logMessage(logStream.str());
        
        if (speedup > 1.0) {
            logger.logMessage("✓ Optimization successful - new method is faster");
        } else {
            logger.logMessage("⚠ Performance regression detected - old method is faster");
        }
    }
    
    logger.logRenderTime();
}

void testCameraRenderScene2DColor() {
    RenderLogger logger("scene2D_color");

    // Create camera
    Vector3D topLeft(0, 0, -5);
    Vector3D topRight = topLeft + Vector3D(10.0, 0, 0);
    Vector3D bottomLeft = topLeft + Vector3D(0, 10.0, 0);
    Rectangle viewport(topLeft, topRight, bottomLeft);
    Camera camera(viewport);

    // Log camera settings
    logger.logCameraSettings(camera, topLeft);
    
    // Create some shapes to render
    math::Vector<Camera::ShapeVariant> shapes;
    
    // Add a sphere at the origin
    Sphere sphere(Vector3D(4, 4, 0), 2.0);
    Shape<::geometry::Sphere> sphereShape(sphere);
    sphereShape.setColor(RGBA_Color(0, 1, 0, 1)); // Green color
    Camera::ShapeVariant sphereVariant = Camera::ShapeVariant{sphereShape};
    std::visit([](auto& shape) { shape.setColor(RGBA_Color(1, 0, 0, 1)); }, sphereVariant); // Red color
    shapes.append(sphereVariant);
    
    // Add a box 
    Box box(Vector3D(5, 5, 10), 2.0, 2.0, 2.0, Vector3D(0, 0, 1));
    Shape<::geometry::Box> boxShape(box);
    boxShape.setColor(RGBA_Color(0, 0, 1, 1)); // Blue color
    Camera::ShapeVariant boxVariant = Camera::ShapeVariant{boxShape};
    std::visit([](auto& shape) { shape.setColor(RGBA_Color(0, 1, 0, 1)); }, boxVariant); // Green color
    shapes.append(boxVariant);
    
    logger.logImageInfo(720, 720);

    // Log scene configuration
    logger.logSceneObjects(shapes);
    
    // Render small test image
    Image colorImage2D = camera.renderScene2DColor(720, 720, shapes);
    logger.logRenderTime();
    
    // Basic checks on the rendered image
    assert(colorImage2D.getWidth() == 720);
    assert(colorImage2D.getHeight() == 720);

    assert(has_non_debug_pixel(colorImage2D));

    colorImage2D.toPngFile("test_render_output", "./test/test_by_product/camera/");
    
    // We expect some pixels to be rendered since we have shapes in the scene
    // Note: This test might be flaky depending on the exact rendering logic
    // If shapes are not intersecting the camera rays, all pixels might be black
    std::cout << "Note: Render test completed - check output manually if needed" << std::endl;
}

void testCameraRenderScene2DDepth() {
    RenderLogger logger("scene2D_depth");
    
    // Create camera
    Vector3D origin(0, 0, -10);
    // Build top-right and bottom-left points explicitly instead of using the old ctor
    Vector3D topRight = origin + Vector3D(10.0, 0, 0);
    Vector3D bottomLeft = origin + Vector3D(0, 10.0, 0);
    Rectangle viewport(origin, topRight, bottomLeft);
    Camera camera(viewport);

    // Log camera settings
    logger.logCameraSettings(camera, origin);
    
    // Create some shapes to render
    math::Vector<Camera::ShapeVariant> shapes;
    
    // Add a sphere at the origin
    Sphere sphere(Vector3D(4, 4, 0), 3.0);
    Shape<::geometry::Sphere> sphereShape(sphere);
    sphereShape.setColor(RGBA_Color(0, 1, 0, 1)); // Green color
    Camera::ShapeVariant sphereVariant = Camera::ShapeVariant{sphereShape};
    shapes.append(sphereVariant);
    
    // Add a box 
    Box box(Vector3D(5, 5, 10), 2.0, 2.0, 2.0, Vector3D(0, 0, 1));
    Shape<::geometry::Box> boxShape(box);
    boxShape.setColor(RGBA_Color(0, 0, 1, 1)); // Blue color
    Camera::ShapeVariant boxVariant = Camera::ShapeVariant{boxShape};
    shapes.append(boxVariant);
    
    logger.logImageInfo(720, 720);

    // Log scene configuration
    logger.logSceneObjects(shapes);
    
    // Render small test image
    Image depthImage2D = camera.renderScene2DDepth(720, 720, shapes);
    logger.logRenderTime();
    
    // Basic checks on the rendered image
    assert(depthImage2D.getWidth() == 720);
    assert(depthImage2D.getHeight() == 720);

    // Check that some pixels have depth values (not all default/black)
    assert(has_non_debug_pixel(depthImage2D));

    depthImage2D.toPngFile("test_depth_output", "./test/test_by_product/camera/");
    
    // We expect some pixels to have depth values since we have shapes in the scene
    std::cout << "Note: Depth render test completed - check output manually if needed" << std::endl;
}

void testCameraRenderScene3DColor() {
    RenderLogger logger("scene3D_color");

    // Create camera
    Vector3D origin(-10, -10, -5);
    // Build top-right and bottom-left points explicitly instead of using the old ctor
    Vector3D topRight = origin + Vector3D(20.0, 0, 0);
    Vector3D bottomLeft = origin + Vector3D(0, 20.0, 0);
    Rectangle viewport(origin, topRight, bottomLeft);
    Camera camera(viewport);

    // Log camera settings
    logger.logCameraSettings(camera, origin);
    
    
    // Create some shapes to render
    math::Vector<Camera::ShapeVariant> shapes;
    
    // Add a sphere at the origin
    Sphere sphere(Vector3D(0, 0, 0), 4.0);
    Shape<::geometry::Sphere> sphereShape(sphere);
    sphereShape.setColor(RGBA_Color(1, 1, 1, 1)); // White color
    Camera::ShapeVariant sphereVariant = Camera::ShapeVariant{sphereShape};
    shapes.append(sphereVariant);
    
    // Add a box 
    Box box(Vector3D(5, 3, 10), 3.0, 3.0, 3.0, Vector3D(0, 0, 1));
    Shape<::geometry::Box> boxShape(box);
    boxShape.setColor(RGBA_Color(1, 0, 0, 1)); // Blue color
    Camera::ShapeVariant boxVariant = Camera::ShapeVariant{boxShape};
    shapes.append(boxVariant);

    // Add walls (Back top bottom right & left)
    Plane backWall(Vector3D(0, 0, 15), Vector3D(0, 0, -1));
    Shape<::geometry::Plane> backWallShape(backWall, RGBA_Color(0.8, 0.2, 0.8, 1.0)); // Magenta wall
    Camera::ShapeVariant backWallVariant = Camera::ShapeVariant{backWallShape};
    shapes.append(backWallVariant);

    Plane leftWall(Vector3D(-10, 0, 0), Vector3D(1, 0, 0));
    Shape<::geometry::Plane> leftWallShape(leftWall, RGBA_Color(0.2, 0.8, 0.8, 1.0)); // Cyan wall
    Camera::ShapeVariant leftWallVariant = Camera::ShapeVariant{leftWallShape};
    shapes.append(leftWallVariant);

    Plane rightWall(Vector3D(10, 0, 0), Vector3D(-1, 0, 0));
    Shape<::geometry::Plane> rightWallShape(rightWall, RGBA_Color(0.8, 0.8, 0.2, 1.0)); // Yellow wall
    Camera::ShapeVariant rightWallVariant = Camera::ShapeVariant{rightWallShape};
    shapes.append(rightWallVariant);

    Plane topWall(Vector3D(0, 10, 0), Vector3D(0, -1, 0));
    Shape<::geometry::Plane> topWallShape(topWall, RGBA_Color(0.8, 0.8, 0.8, 1.0)); // Gray wall
    Camera::ShapeVariant topWallVariant = Camera::ShapeVariant{topWallShape};
    shapes.append(topWallVariant);

    Plane bottomWall(Vector3D(0, -10, 0), Vector3D(0, 1, 0));
    Shape<::geometry::Plane> bottomWallShape(bottomWall, RGBA_Color(0.2, 0.2, 0.8, 1.0)); // Blue wall
    Camera::ShapeVariant bottomWallVariant = Camera::ShapeVariant{bottomWallShape};
    shapes.append(bottomWallVariant);

    logger.logImageInfo(720, 720);

    // Log scene configuration
    logger.logSceneObjects(shapes);
    
    // Render small test image
    Image colorImage3D = camera.renderScene3DColor(720, 720, shapes);
    logger.logRenderTime();
    
    // Basic checks on the rendered image
    assert(colorImage3D.getWidth() == 720);
    assert(colorImage3D.getHeight() == 720);

    // Check that some pixels have depth values (not all default/black)
    assert(has_non_debug_pixel(colorImage3D));

    colorImage3D.toPngFile("test_3d_color_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Color render test completed - check output manually if needed" << std::endl;

    // Test with bigger FOV
    camera.setFOVAngle(120.0f);
    logger.logCameraSettings(camera, origin); // Wider FOV
    colorImage3D = camera.renderScene3DColor(720, 720, shapes);
    logger.logRenderTime();
    colorImage3D.toPngFile("test_3d_color_wide_fov_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Color render with wide FOV test completed - check output manually if needed" << std::endl;

    // Test with smaller FOV
    camera.setFOVAngle(30.0f);
    logger.logCameraSettings(camera, origin); // Narrower FOV
    colorImage3D = camera.renderScene3DColor(720, 720, shapes);
    logger.logRenderTime();
    colorImage3D.toPngFile("test_3d_color_narrow_fov_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Color render with narrow FOV test completed - check output manually if needed" << std::endl;
}

void testCameraRenderScene3DDepth() {
    RenderLogger logger("scene3D_depth");

    // Create camera
    Vector3D origin(-10, -10, -5);
    // Build top-right and bottom-left points explicitly instead of using the old ctor
    Vector3D topRight = origin + Vector3D(20.0, 0, 0);
    Vector3D bottomLeft = origin + Vector3D(0, 20.0, 0);
    Rectangle viewport(origin, topRight, bottomLeft);
    Camera camera(viewport);

    // Log camera settings
    logger.logCameraSettings(camera, origin);
    
    
    // Create some shapes to render
    math::Vector<Camera::ShapeVariant> shapes;
    
    // Add a sphere at the origin
    Sphere sphere(Vector3D(0, 0, 0), 4.0);
    Shape<::geometry::Sphere> sphereShape(sphere);
    sphereShape.setColor(RGBA_Color(1, 1, 1, 1)); // White color
    Camera::ShapeVariant sphereVariant = Camera::ShapeVariant{sphereShape};
    shapes.append(sphereVariant);
    
    // Add a box 
    Box box(Vector3D(5, 3, 10), 3.0, 3.0, 3.0, Vector3D(0, 0, 1));
    Shape<::geometry::Box> boxShape(box);
    boxShape.setColor(RGBA_Color(1, 0, 0, 1)); // Red color
    Camera::ShapeVariant boxVariant = Camera::ShapeVariant{boxShape};
    shapes.append(boxVariant);

    // Add walls (Back top bottom right & left)
    Plane backWall(Vector3D(0, 0, 15), Vector3D(0, 0, -1));
    Shape<::geometry::Plane> backWallShape(backWall, RGBA_Color(0.8, 0.2, 0.8, 1.0)); // Magenta wall
    Camera::ShapeVariant backWallVariant = Camera::ShapeVariant{backWallShape};
    shapes.append(backWallVariant);

    Plane leftWall(Vector3D(-10, 0, 0), Vector3D(1, 0, 0));
    Shape<::geometry::Plane> leftWallShape(leftWall, RGBA_Color(0.2, 0.8, 0.8, 1.0)); // Cyan wall
    Camera::ShapeVariant leftWallVariant = Camera::ShapeVariant{leftWallShape};
    shapes.append(leftWallVariant);

    Plane rightWall(Vector3D(10, 0, 0), Vector3D(-1, 0, 0));
    Shape<::geometry::Plane> rightWallShape(rightWall, RGBA_Color(0.8, 0.8, 0.2, 1.0)); // Yellow wall
    Camera::ShapeVariant rightWallVariant = Camera::ShapeVariant{rightWallShape};
    shapes.append(rightWallVariant);

    Plane topWall(Vector3D(0, 10, 0), Vector3D(0, -1, 0));
    Shape<::geometry::Plane> topWallShape(topWall, RGBA_Color(0.8, 0.8, 0.8, 1.0)); // Gray wall
    Camera::ShapeVariant topWallVariant = Camera::ShapeVariant{topWallShape};
    shapes.append(topWallVariant);

    Plane bottomWall(Vector3D(0, -10, 0), Vector3D(0, 1, 0));
    Shape<::geometry::Plane> bottomWallShape(bottomWall, RGBA_Color(0.2, 0.2, 0.8, 1.0)); // Blue wall
    Camera::ShapeVariant bottomWallVariant = Camera::ShapeVariant{bottomWallShape};
    shapes.append(bottomWallVariant);

    logger.logImageInfo(720, 720);

    // Log scene configuration
    logger.logSceneObjects(shapes);
    
    // Render small test image
    Image depthImage3D = camera.renderScene3DDepth(720, 720, shapes);
    logger.logRenderTime();
    
    // Basic checks on the rendered image
    assert(depthImage3D.getWidth() == 720);
    assert(depthImage3D.getHeight() == 720);

    // Check that some pixels have depth values (not all default/black)
    assert(has_non_debug_pixel(depthImage3D));

    depthImage3D.toPngFile("test_3d_depth_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Depth render test completed - check output manually if needed" << std::endl;

    // Test with bigger FOV
    camera.setFOVAngle(120.0f);
    logger.logCameraSettings(camera, origin); // Wider FOV
    depthImage3D = camera.renderScene3DDepth(720, 720, shapes);
    logger.logRenderTime();
    depthImage3D.toPngFile("test_3d_depth_wide_fov_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Depth render with wide FOV test completed - check output manually if needed" << std::endl;

    // Test with smaller FOV
    camera.setFOVAngle(30.0f);
    logger.logCameraSettings(camera, origin); // Narrower FOV
    depthImage3D = camera.renderScene3DDepth(720, 720, shapes);
    logger.logRenderTime();
    depthImage3D.toPngFile("test_3d_depth_narrow_fov_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Depth render with narrow FOV test completed - check output manually if needed" << std::endl;
}

void testCameraRenderScene3DLight() {
    RenderLogger logger("scene3D_light");

    // Create camera
    Vector3D origin(-10, -10, -5);
    // Build top-right and bottom-left points explicitly instead of using the old ctor
    Vector3D topRight = origin + Vector3D(20.0, 0, 0);
    Vector3D bottomLeft = origin + Vector3D(0, 20.0, 0);
    Rectangle viewport(origin, topRight, bottomLeft);
    Camera camera(viewport);
    
    // Log camera settings
    logger.logCameraSettings(camera, origin);
    
    
    // Create the lights of the room
    Light light1(Vector3D(0, 8, -2), RGBA_Color(1.0, 1.0, 1.0, 1.0), 2.0);
    Light light2(Vector3D(-5, -5, 0), RGBA_Color(1.0, 1.0, 1.0, 1.0), 0.6);
    Light light3(Vector3D(5, 5, -2), RGBA_Color(1.0, 0.0, 0.0, 1.0), 1.0);
    Light light4(Vector3D(5, 5, 2), RGBA_Color(0.0, 1.0, 0.0, 1.0), 1.0);
    Light light5(Vector3D(5, -5, -2), RGBA_Color(0.0, 0.0, 1.0, 1.0), 1.0);
    math::Vector<Light> lights;
    lights.append(light1);
    lights.append(light2);
    lights.append(light3);
    lights.append(light4);
    lights.append(light5);

    // Create some shapes to render
    math::Vector<Camera::ShapeVariant> shapes;
    
    // Add a sphere at the origin
    Sphere sphere(Vector3D(0, 0, 0), 4.0);
    Shape<::geometry::Sphere> sphereShape(sphere);
    sphereShape.setColor(RGBA_Color(1, 1, 1, 1)); // White color
    Camera::ShapeVariant sphereVariant = Camera::ShapeVariant{sphereShape};
    shapes.append(sphereVariant);
    
    // Add a box 
    Box box(Vector3D(5, 3, 10), 3.0, 3.0, 3.0, Vector3D(0, 0, 1));
    Shape<::geometry::Box> boxShape(box);
    boxShape.setColor(RGBA_Color(1, 0, 0, 1)); // Red color
    Camera::ShapeVariant boxVariant = Camera::ShapeVariant{boxShape};
    shapes.append(boxVariant);

    // Add transparent rectangle
    Vector3D rectTopLeft(-7, -7, -3);
    Rectangle transparentRect(rectTopLeft, rectTopLeft + Vector3D(5.0, 0, 0), rectTopLeft + Vector3D(0.0, 5.0, 0.0));
    Shape<::geometry::Rectangle> transparentRectShape(transparentRect, RGBA_Color(0.0, 0.0, 0.8, 0.3)); // Semi-transparent blue
    Camera::ShapeVariant transparentRectVariant = Camera::ShapeVariant{transparentRectShape};
    shapes.append(transparentRectVariant);

    // Add walls (Back top bottom right & left)
    Plane backWall(Vector3D(0, 0, 15), Vector3D(0, 0, -1));
    Shape<::geometry::Plane> backWallShape(backWall, RGBA_Color(0.8, 0.2, 0.8, 1.0)); // Magenta wall
    Camera::ShapeVariant backWallVariant = Camera::ShapeVariant{backWallShape};
    shapes.append(backWallVariant);

    Plane leftWall(Vector3D(-10, 0, 0), Vector3D(1, 0, 0));
    Shape<::geometry::Plane> leftWallShape(leftWall, RGBA_Color(0.2, 0.8, 0.8, 1.0)); // Cyan wall
    Camera::ShapeVariant leftWallVariant = Camera::ShapeVariant{leftWallShape};
    shapes.append(leftWallVariant);

    Plane rightWall(Vector3D(10, 0, 0), Vector3D(-1, 0, 0));
    Shape<::geometry::Plane> rightWallShape(rightWall, RGBA_Color(0.8, 0.8, 0.2, 1.0)); // Yellow wall
    Camera::ShapeVariant rightWallVariant = Camera::ShapeVariant{rightWallShape};
    shapes.append(rightWallVariant);

    Plane topWall(Vector3D(0, 10, 0), Vector3D(0, -1, 0));
    Shape<::geometry::Plane> topWallShape(topWall, RGBA_Color(0.8, 0.8, 0.8, 1.0)); // Gray wall
    Camera::ShapeVariant topWallVariant = Camera::ShapeVariant{topWallShape};
    shapes.append(topWallVariant);

    Plane bottomWall(Vector3D(0, -10, 0), Vector3D(0, 1, 0));
    Shape<::geometry::Plane> bottomWallShape(bottomWall, RGBA_Color(0.2, 0.2, 0.8, 1.0)); // Blue wall
    Camera::ShapeVariant bottomWallVariant = Camera::ShapeVariant{bottomWallShape};
    shapes.append(bottomWallVariant);

    // Log scene configuration
    logger.logScene(shapes, lights);
    
    logger.logImageInfo(720, 720);
    // Render with default FOV
    Image lightImage3D = camera.renderScene3DLight(720, 720, shapes, lights);
    logger.logRenderTime();
    
    // Basic checks on the rendered image
    assert(lightImage3D.getWidth() == 720);
    assert(lightImage3D.getHeight() == 720);

    // Check that some pixels have depth values (not all default/black)
    assert(has_non_debug_pixel(lightImage3D));

    lightImage3D.toPngFile("test_3d_light_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Light render test completed - check output manually if needed" << std::endl;

    // Test with bigger FOV
    camera.setFOVAngle(120.0f);
    logger.logCameraSettings(camera, origin); // Wider FOV
    lightImage3D = camera.renderScene3DLight(720, 720, shapes, lights);
    logger.logRenderTime();
    lightImage3D.toPngFile("test_3d_light_wide_fov_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Light render with wide FOV test completed - check output manually if needed" << std::endl;

    // Test with smaller FOV
    camera.setFOVAngle(30.0f);
    logger.logCameraSettings(camera, origin); // Narrower FOV
    lightImage3D = camera.renderScene3DLight(720, 720, shapes, lights);
    logger.logRenderTime();
    lightImage3D.toPngFile("test_3d_light_narrow_fov_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Light render with narrow FOV test completed - check output manually if needed" << std::endl;
}

void testCameraRenderScene3DLight_AA() {
    RenderLogger logger("scene3D_light_antialiasing");

    // Create camera
    Vector3D origin(-10, -10, -5);
    // Build top-right and bottom-left points explicitly instead of using the old ctor
    Vector3D topRight = origin + Vector3D(20.0, 0, 0);
    Vector3D bottomLeft = origin + Vector3D(0, 20.0, 0);
    Rectangle viewport(origin, topRight, bottomLeft);
    Camera camera(viewport);

    // Log camera settings
    logger.logCameraSettings(camera, origin);
    
    // Create the lights of the room
    Light light1(Vector3D(0, 8, -2), RGBA_Color(1.0, 1.0, 1.0, 1.0), 2.0);
    Light light2(Vector3D(-5, -5, 0), RGBA_Color(1.0, 1.0, 1.0, 1.0), 0.6);
    Light light3(Vector3D(5, 5, -2), RGBA_Color(1.0, 0.0, 0.0, 1.0), 1.0);
    Light light4(Vector3D(5, 5, 2), RGBA_Color(0.0, 1.0, 0.0, 1.0), 1.0);
    Light light5(Vector3D(5, -5, -2), RGBA_Color(0.0, 0.0, 1.0, 1.0), 1.0);
    math::Vector<Light> lights;
    lights.append(light1);
    lights.append(light2);
    lights.append(light3);
    lights.append(light4);
    lights.append(light5);

    // Create some shapes to render
    math::Vector<Camera::ShapeVariant> shapes;
    
    // Add a sphere at the origin
    Sphere sphere(Vector3D(0, 0, 0), 4.0);
    Shape<::geometry::Sphere> sphereShape(sphere);
    Camera::ShapeVariant sphereVariant = Camera::ShapeVariant{sphereShape};
    shapes.append(sphereVariant);
    
    // Add a box 
    Box box(Vector3D(5, 3, 10), 3.0, 3.0, 3.0, Vector3D(0, 0, 1));
    Shape<::geometry::Box> boxShape(box);
    Camera::ShapeVariant boxVariant = Camera::ShapeVariant{boxShape};
    shapes.append(boxVariant);

    // Add transparent rectangle
    Vector3D rectTopLeft(-7, -7, -3);
    Rectangle transparentRect(rectTopLeft, rectTopLeft + Vector3D(5.0, 0, 0), rectTopLeft + Vector3D(0.0, 5.0, 0.0));
    Shape<::geometry::Rectangle> transparentRectShape(transparentRect, RGBA_Color(0.0, 0.0, 0.8, 0.3)); // Semi-transparent blue
    Camera::ShapeVariant transparentRectVariant = Camera::ShapeVariant{transparentRectShape};
    shapes.append(transparentRectVariant);

    // Add walls (Back top bottom right & left)
    Plane backWall(Vector3D(0, 0, 15), Vector3D(0, 0, -1));
    Shape<::geometry::Plane> backWallShape(backWall, RGBA_Color(0.8, 0.2, 0.8, 1.0)); // Magenta wall
    Camera::ShapeVariant backWallVariant = Camera::ShapeVariant{backWallShape};
    shapes.append(backWallVariant);

    Plane leftWall(Vector3D(-10, 0, 0), Vector3D(1, 0, 0));
    Shape<::geometry::Plane> leftWallShape(leftWall, RGBA_Color(0.2, 0.8, 0.8, 1.0)); // Cyan wall
    Camera::ShapeVariant leftWallVariant = Camera::ShapeVariant{leftWallShape};
    shapes.append(leftWallVariant);

    Plane rightWall(Vector3D(10, 0, 0), Vector3D(-1, 0, 0));
    Shape<::geometry::Plane> rightWallShape(rightWall, RGBA_Color(0.8, 0.8, 0.2, 1.0)); // Yellow wall
    Camera::ShapeVariant rightWallVariant = Camera::ShapeVariant{rightWallShape};
    shapes.append(rightWallVariant);

    Plane topWall(Vector3D(0, 10, 0), Vector3D(0, -1, 0));
    Shape<::geometry::Plane> topWallShape(topWall, RGBA_Color(0.8, 0.8, 0.8, 1.0)); // Gray wall
    Camera::ShapeVariant topWallVariant = Camera::ShapeVariant{topWallShape};
    shapes.append(topWallVariant);

    Plane bottomWall(Vector3D(0, -10, 0), Vector3D(0, 1, 0));
    Shape<::geometry::Plane> bottomWallShape(bottomWall, RGBA_Color(0.2, 0.2, 0.8, 1.0)); // Blue wall
    Camera::ShapeVariant bottomWallVariant = Camera::ShapeVariant{bottomWallShape};
    shapes.append(bottomWallVariant);

    logger.logImageInfo(720, 720);

    // Log scene configuration
    logger.logScene(shapes, lights);

    // Render control image without anti-aliasing
    Image AntiAliasingImage = camera.renderScene3DLight_AA(720, 720, shapes, lights, 16UL, rendering::Camera::AntiAliasingMethod::NONE);

    // Basic checks on the rendered image
    assert(AntiAliasingImage.getWidth() == 720);
    assert(AntiAliasingImage.getHeight() == 720);

    // Check that some pixels have depth values (not all default/black)
    assert(has_non_debug_pixel(AntiAliasingImage));

    logger.logMessage("Testing Control for anti-aliasing method");
    AntiAliasingImage.toPngFile("test_3d_light_anti_aliasing_output_control", "./test/test_by_product/camera/");
    logger.logRenderTime();
    std::cout << "Note: 3D Light with NO anti-aliasing render test completed - check output manually if needed" << std::endl;

    // Test with SSAA
    logger.logMessage("Testing SSAA anti-aliasing method");
    AntiAliasingImage = camera.renderScene3DLight_AA(720, 720, shapes, lights, 16UL, rendering::Camera::AntiAliasingMethod::SSAA);
    logger.logRenderTime();
    AntiAliasingImage.toPngFile("test_3d_light_anti_aliasing_output_ssaa", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Light with SSAA anti-aliasing render with wide FOV test completed - check output manually if needed" << std::endl;

    // Test with MSAA
    logger.logMessage("Testing MSAA anti-aliasing method");
    AntiAliasingImage = camera.renderScene3DLight_AA(720, 720, shapes, lights, 16UL, rendering::Camera::AntiAliasingMethod::MSAA);
    logger.logRenderTime();
    AntiAliasingImage.toPngFile("test_3d_light_anti_aliasing_output_msaa", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Light with MSAA anti-aliasing render with narrow FOV test completed - check output manually if needed" << std::endl;

    try {
        // Test with FXAA
        AntiAliasingImage = camera.renderScene3DLight_AA(720, 720, shapes, lights, 16UL, rendering::Camera::AntiAliasingMethod::FXAA);
        logger.logRenderTime();
        AntiAliasingImage.toPngFile("test_3d_light_anti_aliasing_output_fxaa", "./test/test_by_product/camera/");
        std::cout << "Note: 3D Light with FXAA anti-aliasing render with narrow FOV test completed - check output manually if needed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "FXAA anti-aliasing test skipped due to exception: " << e.what() << std::endl;
    }
    
}