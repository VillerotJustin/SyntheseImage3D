#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <chrono>
#include <iomanip>
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

// Logger class for render information
class RenderLogger {
    std::ofstream logFile;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

public:
    RenderLogger(const std::string& testName) {
        // Create the camera directory if it doesn't exist
        std::string dirPath = "test/test_by_product/camera";
        if (system(("mkdir -p " + dirPath).c_str()) != 0) {
            throw std::runtime_error("Failed to create directory: " + dirPath);
        }

        std::string logPath = dirPath + "/" + testName + ".log";
        logFile.open(logPath);
        if (!logFile.is_open()) {
            throw std::runtime_error("Could not open log file: " + logPath);
        }
        startTime = std::chrono::high_resolution_clock::now();
        logFile << "=== Render Test: " << testName << " ===" << std::endl;
        logFile << "Start time: " << getCurrentTime() << std::endl << std::endl;
    }

    ~RenderLogger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    // Just delegate to logScene with an empty lights vector
    void logSceneObjects(const math::Vector<Camera::ShapeVariant>& shapes) {
        logScene(shapes, math::Vector<Light>());
    }

    void logScene(const math::Vector<Camera::ShapeVariant>& shapes, const math::Vector<Light>& lights) {
        logFile << "Scene Configuration:" << std::endl;
        logFile << "-------------------" << std::endl;
        
        // Log shapes
        logFile << "Shapes (" << shapes.size() << " total):" << std::endl;
        for (size_t i = 0; i < shapes.size(); ++i) {
            std::visit([&](const auto& shape) {
                logFile << "  Shape " << i + 1 << ":" << std::endl;
                
                if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, Shape<Sphere>>) {
                    if (const auto* geom = shape.getGeometry()) {
                        logFile << "    Type: Sphere" << std::endl;
                        logFile << "    Center: " << geom->getCenter() << std::endl;
                        logFile << "    Radius: " << geom->getRadius() << std::endl;
                    }
                } else if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, Shape<Box>>) {
                    if (const auto* geom = shape.getGeometry()) {
                        logFile << "    Type: Box" << std::endl;
                        logFile << "    Origin: " << geom->getOrigin() << std::endl;
                        logFile << "    Width: " << geom->getWidth() << std::endl;
                        logFile << "    Height: " << geom->getHeight() << std::endl;
                        logFile << "    Depth: " << geom->getDepth() << std::endl;
                    }
                } else if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, Shape<Rectangle>>) {
                    if (const auto* geom = shape.getGeometry()) {
                        logFile << "    Type: Rectangle" << std::endl;
                        logFile << "    Origin: " << geom->getOrigin() << std::endl;
                        logFile << "    Length: " << geom->getLength() << std::endl;
                        logFile << "    Width: " << geom->getWidth() << std::endl;
                    }
                } else if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, Shape<Circle>>) {
                    if (const auto* geom = shape.getGeometry()) {
                        logFile << "    Type: Circle" << std::endl;
                        logFile << "    Center: " << geom->getCenter() << std::endl;
                        logFile << "    Radius: " << geom->getRadius() << std::endl;
                    }
                } else if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, Shape<Plane>>) {
                    if (const auto* geom = shape.getGeometry()) {
                        logFile << "    Type: Plane" << std::endl;
                        logFile << "    Origin: " << geom->getOrigin() << std::endl;
                        logFile << "    Normal: " << geom->getNormal() << std::endl;
                    }
                }

                if (const auto* color = shape.getColor()) {
                    logFile << "    Color: " << *color << std::endl;
                } else {
                    logFile << "    Color: None" << std::endl;
                }
            }, shapes[i]);
        }
        logFile << std::endl;

        // Log lights
        logFile << "Lights (" << lights.size() << " total):" << std::endl;
        for (size_t i = 0; i < lights.size(); ++i) {
            const Light light = lights[i];
            logFile << "  Light " << i + 1 << ":" << std::endl;
            logFile << "    Type: Point Light" << std::endl;
            logFile << "    Position: " << light.getPosition() << std::endl;
            logFile << "    Intensity: " << light.getIntensity() << std::endl;
            logFile << "    Color: " << light.getColor() << std::endl;
        }
        logFile << std::endl;
    }

    void logCameraSettings(const Camera& camera, const Vector3D& origin) {
        logFile << "Camera Configuration:" << std::endl;
        logFile << "--------------------" << std::endl;
        logFile << "Origin: " << origin << std::endl;
        logFile << "Direction: " << camera.getDirection() << std::endl;
        logFile << "Field of View: " << camera.getFOVAngle() << std::endl;
        logFile << std::endl;
    }

    void logRenderTime() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        logFile << "Render Time: " << duration.count() << " ms" << std::endl << std::endl;
        logFile << "-------------" << std::endl;
        logFile << std::endl;
    }

    void logImageInfo(int width, int height) {
        logFile << "Output Image:" << std::endl;
        logFile << "-------------" << std::endl;
        logFile << "Dimensions: " << width << "x" << height << " pixels" << std::endl;
        logFile << std::endl;
    }

private:
    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto nowTime = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Test function declarations
void testCameraConstructor();
void testCameraViewportOperations();
void testCameraRotation();
void testCameraTranslation();
void testCameraRayGeneration();
void testCameraRenderScene2DColor();
void testCameraRenderScene2DDepth();
void testCameraRenderScene3DColor();
void testCameraRenderScene3DDepth();
void testCameraRenderScene3DLight();
void testCameraRenderScene3DLightAntiAliasing();

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

        testCameraRenderScene2DDepth();
        std::cout << "✓ Camera render scene depth tests passed" << std::endl;

        testCameraRenderScene3DColor();
        std::cout << "✓ Camera render scene 3D color tests passed" << std::endl;

        testCameraRenderScene3DDepth();
        std::cout << "✓ Camera render scene 3D depth tests passed" << std::endl;

        testCameraRenderScene3DLight();
        std::cout << "✓ Camera render scene 3D light tests passed" << std::endl;

        testCameraRenderScene3DLightAntiAliasing();
        std::cout << "✓ Camera render scene 3D light with anti-aliasing tests passed" << std::endl;

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

bool has_non_black_pixel(Image image) {
    for (size_t y = 0; y < image.getHeight(); ++y) {
        for (size_t x = 0; x < image.getWidth(); ++x) {
            const RGBA_Color pixel = image.getPixel(x, y);
            if ((pixel.r() > 0 || pixel.g() > 0 || pixel.b() > 0)) {
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
    Camera::ShapeVariant sphereVariant = Camera::ShapeVariant{sphereShape};
    std::visit([](auto& shape) { shape.setColor(RGBA_Color(1, 0, 0, 1)); }, sphereVariant); // Red color
    shapes.append(sphereVariant);
    
    // Add a box 
    Box box(Vector3D(5, 5, 10), 2.0, 2.0, 2.0, Vector3D(0, 0, 1));
    Shape<::geometry::Box> boxShape(box);
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

    assert(has_non_black_pixel(colorImage2D));

    colorImage2D.toBitmapFile("test_render_output", "./test/test_by_product/camera/");
    
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
    Camera::ShapeVariant sphereVariant = Camera::ShapeVariant{sphereShape};
    shapes.append(sphereVariant);
    
    // Add a box 
    Box box(Vector3D(5, 5, 10), 2.0, 2.0, 2.0, Vector3D(0, 0, 1));
    Shape<::geometry::Box> boxShape(box);
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
    assert(has_non_black_pixel(depthImage2D));

    depthImage2D.toBitmapFile("test_depth_output", "./test/test_by_product/camera/");
    
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
    Camera::ShapeVariant sphereVariant = Camera::ShapeVariant{sphereShape};
    shapes.append(sphereVariant);
    
    // Add a box 
    Box box(Vector3D(5, 3, 10), 3.0, 3.0, 3.0, Vector3D(0, 0, 1));
    Shape<::geometry::Box> boxShape(box);
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
    assert(has_non_black_pixel(colorImage3D));

    colorImage3D.toBitmapFile("test_3d_color_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Color render test completed - check output manually if needed" << std::endl;

    // Test with bigger FOV
    camera.setFOVAngle(120.0f);
    logger.logCameraSettings(camera, origin); // Wider FOV
    colorImage3D = camera.renderScene3DColor(720, 720, shapes);
    logger.logRenderTime();
    colorImage3D.toBitmapFile("test_3d_color_wide_fov_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Color render with wide FOV test completed - check output manually if needed" << std::endl;

    // Test with smaller FOV
    camera.setFOVAngle(30.0f);
    logger.logCameraSettings(camera, origin); // Narrower FOV
    colorImage3D = camera.renderScene3DColor(720, 720, shapes);
    logger.logRenderTime();
    colorImage3D.toBitmapFile("test_3d_color_narrow_fov_output", "./test/test_by_product/camera/");
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
    Camera::ShapeVariant sphereVariant = Camera::ShapeVariant{sphereShape};
    shapes.append(sphereVariant);
    
    // Add a box 
    Box box(Vector3D(5, 3, 10), 3.0, 3.0, 3.0, Vector3D(0, 0, 1));
    Shape<::geometry::Box> boxShape(box);
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
    assert(has_non_black_pixel(depthImage3D));

    depthImage3D.toBitmapFile("test_3d_depth_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Depth render test completed - check output manually if needed" << std::endl;

    // Test with bigger FOV
    camera.setFOVAngle(120.0f);
    logger.logCameraSettings(camera, origin); // Wider FOV
    depthImage3D = camera.renderScene3DDepth(720, 720, shapes);
    logger.logRenderTime();
    depthImage3D.toBitmapFile("test_3d_depth_wide_fov_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Depth render with wide FOV test completed - check output manually if needed" << std::endl;

    // Test with smaller FOV
    camera.setFOVAngle(30.0f);
    logger.logCameraSettings(camera, origin); // Narrower FOV
    depthImage3D = camera.renderScene3DDepth(720, 720, shapes);
    logger.logRenderTime();
    depthImage3D.toBitmapFile("test_3d_depth_narrow_fov_output", "./test/test_by_product/camera/");
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
    assert(has_non_black_pixel(lightImage3D));

    lightImage3D.toBitmapFile("test_3d_light_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Light render test completed - check output manually if needed" << std::endl;

    // Test with bigger FOV
    camera.setFOVAngle(120.0f);
    logger.logCameraSettings(camera, origin); // Wider FOV
    lightImage3D = camera.renderScene3DLight(720, 720, shapes, lights);
    logger.logRenderTime();
    lightImage3D.toBitmapFile("test_3d_light_wide_fov_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Light render with wide FOV test completed - check output manually if needed" << std::endl;

    // Test with smaller FOV
    camera.setFOVAngle(30.0f);
    logger.logCameraSettings(camera, origin); // Narrower FOV
    lightImage3D = camera.renderScene3DLight(720, 720, shapes, lights);
    logger.logRenderTime();
    lightImage3D.toBitmapFile("test_3d_light_narrow_fov_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Light render with narrow FOV test completed - check output manually if needed" << std::endl;
}

void testCameraRenderScene3DLightAntiAliasing() {
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

    // Render small test image
    Image AntiAliasingImage = camera.renderScene3DLightAntiAliasing(720, 720, shapes, lights, 16UL);
    
    // Basic checks on the rendered image
    assert(AntiAliasingImage.getWidth() == 720);
    assert(AntiAliasingImage.getHeight() == 720);

    // Check that some pixels have depth values (not all default/black)
    assert(has_non_black_pixel(AntiAliasingImage));

    AntiAliasingImage.toBitmapFile("test_3d_light_anti_aliasing_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Light with anti-aliasing render test completed - check output manually if needed" << std::endl;

    // Test with bigger FOV
    camera.setFOVAngle(120.0f);
    logger.logCameraSettings(camera, origin); // Wider FOV
    AntiAliasingImage = camera.renderScene3DLightAntiAliasing(720, 720, shapes, lights, 16UL);
    logger.logRenderTime();
    AntiAliasingImage.toBitmapFile("test_3d_light_anti_aliasing_wide_fov_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Light with anti-aliasing render with wide FOV test completed - check output manually if needed" << std::endl;

    // Test with smaller FOV
    camera.setFOVAngle(30.0f);
    logger.logCameraSettings(camera, origin); // Narrower FOV
    AntiAliasingImage = camera.renderScene3DLightAntiAliasing(720, 720, shapes, lights, 16UL);
    logger.logRenderTime();
    AntiAliasingImage.toBitmapFile("test_3d_light_anti_aliasing_narrow_fov_output", "./test/test_by_product/camera/");
    std::cout << "Note: 3D Light with anti-aliasing render with narrow FOV test completed - check output manually if needed" << std::endl;
}