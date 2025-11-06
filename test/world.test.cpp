#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include "../Lib/Rendering/World.h"
#include "../Lib/Rendering/Camera.h"
#include "../Lib/Rendering/Light.h"
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

using namespace rendering;
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
void testWorldConstructor();
void testWorldAddRemoveObjects();
void testWorldAddRemoveLights();
void testWorldCameraAccess();
void testWorldClearObjects();
void testWorldRenderScene2DColor();
void testWorldRenderScene2DDepth();
void testWorldRenderScene3DColor();
void testWorldRenderScene3DDepth();
void testWorldRenderScene3DLight();

int main() {
    std::cout << "Running World tests..." << std::endl;
    
    try {
        testWorldConstructor();
        std::cout << "✓ World constructor tests passed" << std::endl;
        
        testWorldAddRemoveObjects();
        std::cout << "✓ World add/remove objects tests passed" << std::endl;
        
        testWorldAddRemoveLights();
        std::cout << "✓ World add/remove lights tests passed" << std::endl;
        
        testWorldCameraAccess();
        std::cout << "✓ World camera access tests passed" << std::endl;
        
        testWorldClearObjects();
        std::cout << "✓ World clear objects tests passed" << std::endl;
        
        testWorldRenderScene2DColor();
        std::cout << "✓ World render scene 2D color tests passed" << std::endl;
        
        testWorldRenderScene2DDepth();
        std::cout << "✓ World render scene 2D depth tests passed" << std::endl;
        
        testWorldRenderScene3DColor();
        std::cout << "✓ World render scene 3D color tests passed" << std::endl;
        
        testWorldRenderScene3DDepth();
        std::cout << "✓ World render scene 3D depth tests passed" << std::endl;
        
        testWorldRenderScene3DLight();
        std::cout << "✓ World render scene 3D light tests passed" << std::endl;
        
        std::cout << "All World tests passed!" << std::endl;
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

void testWorldConstructor() {
    // Test default constructor
    World world;
    
    // Check that world is initialized with 0 objects
    assert(world.getObjectCount() == 0);
    
    // Check that camera is initialized
    const Camera& camera = world.getCamera();
    const Rectangle& viewport = camera.getViewport();
    
    // Verify camera has reasonable default values
    assert(viewport.getLength() > 0);
    assert(viewport.getWidth() > 0);
}

void testWorldAddRemoveObjects() {
    World world;
    
    // Test adding a sphere
    Sphere sphere(Vector3D(0, 0, 0), 5.0);
    Shape<Sphere> sphereShape(sphere, RGBA_Color(1, 0, 0, 1));
    
    assert(world.getObjectCount() == 0);
    world.addObject(sphereShape);
    assert(world.getObjectCount() == 1);
    
    // Test adding a box
    Box box(Vector3D(5, 5, 5), 2.0, 2.0, 2.0, Vector3D(0, 0, 1));
    Shape<Box> boxShape(box, RGBA_Color(0, 1, 0, 1));
    
    world.addObject(boxShape);
    assert(world.getObjectCount() == 2);
    
    // Test adding a plane
    Plane plane(Vector3D(0, 0, 10), Vector3D(0, 0, -1));
    Shape<Plane> planeShape(plane, RGBA_Color(0, 0, 1, 1));
    
    world.addObject(planeShape);
    assert(world.getObjectCount() == 3);
    
    // Test removing an object
    world.removeObjectAt(0);
    assert(world.getObjectCount() == 2);
    
    // Test removing another object
    world.removeObjectAt(0);
    assert(world.getObjectCount() == 1);
}

void testWorldAddRemoveLights() {
    World world;
    
    // Test adding lights
    Light light1(Vector3D(0, 10, 0), RGBA_Color(1, 1, 1, 1), 1.0);
    world.addLight(light1);
    
    Light light2(Vector3D(5, 5, 5), RGBA_Color(1, 0, 0, 1), 0.5);
    world.addLight(light2);
    
    // Test removing light by index
    world.removeLightAt(0);
    
    // Test removing light by reference
    world.removeLight(light2);
    
    std::cout << "Note: Light management tests completed" << std::endl;
}

void testWorldCameraAccess() {
    World world;
    
    // Test getting camera (const version)
    const Camera& constCamera = world.getCamera();
    const Rectangle& viewport1 = constCamera.getViewport();
    
    // Test getting camera (non-const version)
    Camera& camera = world.getCamera();
    const Rectangle& viewport2 = camera.getViewport();
    
    // Both should refer to the same camera
    assert(isEqual(viewport1.getOrigin(), viewport2.getOrigin()));
    
    // Store the original origin before translation
    Vector3D originalOrigin = viewport1.getOrigin();
    
    // Test modifying camera through non-const reference
    Vector3D translation(5, 5, 5);
    camera.translate(translation);
    
    const Rectangle& viewport3 = world.getCamera().getViewport();
    assert(isEqual(viewport3.getOrigin(), originalOrigin + translation));
}

void testWorldClearObjects() {
    World world;
    
    // Add multiple objects
    Sphere sphere1(Vector3D(0, 0, 0), 2.0);
    Shape<Sphere> sphereShape1(sphere1);
    world.addObject(sphereShape1);
    
    Sphere sphere2(Vector3D(5, 5, 5), 3.0);
    Shape<Sphere> sphereShape2(sphere2);
    world.addObject(sphereShape2);
    
    Box box(Vector3D(10, 10, 10), 1.0, 1.0, 1.0, Vector3D(0, 0, 1));
    Shape<Box> boxShape(box);
    world.addObject(boxShape);
    
    assert(world.getObjectCount() == 3);
    
    // Clear all objects
    world.clearObjects();
    assert(world.getObjectCount() == 0);
}

void testWorldRenderScene2DColor() {
    World world;
    
    // Setup camera
    Vector3D origin(0, 0, -10);
    Vector3D topRight = origin + Vector3D(10.0, 0, 0);
    Vector3D bottomLeft = origin + Vector3D(0, 10.0, 0);
    Rectangle viewport(origin, topRight, bottomLeft);
    world.getCamera().setViewport(viewport);
    
    // Add objects to the world
    Sphere sphere(Vector3D(4, 4, 0), 3.0);
    Shape<Sphere> sphereShape(sphere, RGBA_Color(1, 0, 0, 1));
    world.addObject(sphereShape);
    
    Box box(Vector3D(5, 5, 10), 2.0, 2.0, 2.0, Vector3D(0, 0, 1));
    Shape<Box> boxShape(box, RGBA_Color(0, 1, 0, 1));
    world.addObject(boxShape);
    
    // Render scene
    Image image = world.renderScene2DColor(720, 720);
    
    // Basic checks
    assert(image.getWidth() == 720);
    assert(image.getHeight() == 720);
    
    // Check for rendered pixels
    assert(has_non_debug_pixel(image));
    
    image.toBitmapFile("test_world_2d_color_output", "./test/test_by_product/world/");
    std::cout << "Note: World 2D color render test completed - check output manually if needed" << std::endl;
}

void testWorldRenderScene2DDepth() {
    World world;
    
    // Setup camera
    Vector3D origin(0, 0, -10);
    Vector3D topRight = origin + Vector3D(10.0, 0, 0);
    Vector3D bottomLeft = origin + Vector3D(0, 10.0, 0);
    Rectangle viewport(origin, topRight, bottomLeft);
    world.getCamera().setViewport(viewport);
    
    // Add objects
    Sphere sphere(Vector3D(4, 4, 0), 3.0);
    Shape<Sphere> sphereShape(sphere);
    world.addObject(sphereShape);
    
    Box box(Vector3D(5, 5, 10), 2.0, 2.0, 2.0, Vector3D(0, 0, 1));
    Shape<Box> boxShape(box);
    world.addObject(boxShape);
    
    // Render scene
    Image depthImage = world.renderScene2DDepth(720, 720);
    
    // Basic checks
    assert(depthImage.getWidth() == 720);
    assert(depthImage.getHeight() == 720);
    
    depthImage.toBitmapFile("test_world_2d_depth_output", "./test/test_by_product/world/");
    std::cout << "Note: World 2D depth render test completed - check output manually if needed" << std::endl;
}

void testWorldRenderScene3DColor() {
    World world;
    
    // Setup camera
    Vector3D origin(-10, -10, -5);
    Vector3D topRight = origin + Vector3D(20.0, 0, 0);
    Vector3D bottomLeft = origin + Vector3D(0, 20.0, 0);
    Rectangle viewport(origin, topRight, bottomLeft);
    world.getCamera().setViewport(viewport);
    
    // Add objects
    Sphere sphere(Vector3D(0, 0, 0), 4.0);
    Shape<Sphere> sphereShape(sphere, RGBA_Color(1, 1, 1, 1));
    world.addObject(sphereShape);
    
    Box box(Vector3D(5, 3, 10), 3.0, 3.0, 3.0, Vector3D(0, 0, 1));
    Shape<Box> boxShape(box, RGBA_Color(1, 0, 0, 1));
    world.addObject(boxShape);
    
    // Add walls
    Plane backWall(Vector3D(0, 0, 15), Vector3D(0, 0, -1));
    Shape<Plane> backWallShape(backWall, RGBA_Color(0.8, 0.2, 0.8, 1.0));
    world.addObject(backWallShape);
    
    Plane leftWall(Vector3D(-10, 0, 0), Vector3D(1, 0, 0));
    Shape<Plane> leftWallShape(leftWall, RGBA_Color(0.2, 0.8, 0.8, 1.0));
    world.addObject(leftWallShape);
    
    Plane rightWall(Vector3D(10, 0, 0), Vector3D(-1, 0, 0));
    Shape<Plane> rightWallShape(rightWall, RGBA_Color(0.8, 0.8, 0.2, 1.0));
    world.addObject(rightWallShape);
    
    Plane topWall(Vector3D(0, 10, 0), Vector3D(0, -1, 0));
    Shape<Plane> topWallShape(topWall, RGBA_Color(0.8, 0.8, 0.8, 1.0));
    world.addObject(topWallShape);
    
    Plane bottomWall(Vector3D(0, -10, 0), Vector3D(0, 1, 0));
    Shape<Plane> bottomWallShape(bottomWall, RGBA_Color(0.2, 0.2, 0.8, 1.0));
    world.addObject(bottomWallShape);
    
    // Render scene
    Image image = world.renderScene3DColor(720, 720);
    
    // Basic checks
    assert(image.getWidth() == 720);
    assert(image.getHeight() == 720);
    
    // Check for rendered pixels
    assert(has_non_debug_pixel(image));
    
    image.toBitmapFile("test_world_3d_color_output", "./test/test_by_product/world/");
    std::cout << "Note: World 3D color render test completed - check output manually if needed" << std::endl;
}

void testWorldRenderScene3DDepth() {
    World world;
    
    // Setup camera
    Vector3D origin(-10, -10, -5);
    Vector3D topRight = origin + Vector3D(20.0, 0, 0);
    Vector3D bottomLeft = origin + Vector3D(0, 20.0, 0);
    Rectangle viewport(origin, topRight, bottomLeft);
    world.getCamera().setViewport(viewport);
    
    // Add objects
    Sphere sphere(Vector3D(0, 0, 0), 4.0);
    Shape<Sphere> sphereShape(sphere);
    world.addObject(sphereShape);
    
    Box box(Vector3D(5, 3, 10), 3.0, 3.0, 3.0, Vector3D(0, 0, 1));
    Shape<Box> boxShape(box);
    world.addObject(boxShape);
    
    // Add walls
    Plane backWall(Vector3D(0, 0, 15), Vector3D(0, 0, -1));
    Shape<Plane> backWallShape(backWall, RGBA_Color(0.8, 0.2, 0.8, 1.0));
    world.addObject(backWallShape);
    
    Plane leftWall(Vector3D(-10, 0, 0), Vector3D(1, 0, 0));
    Shape<Plane> leftWallShape(leftWall, RGBA_Color(0.2, 0.8, 0.8, 1.0));
    world.addObject(leftWallShape);
    
    Plane rightWall(Vector3D(10, 0, 0), Vector3D(-1, 0, 0));
    Shape<Plane> rightWallShape(rightWall, RGBA_Color(0.8, 0.8, 0.2, 1.0));
    world.addObject(rightWallShape);
    
    Plane topWall(Vector3D(0, 10, 0), Vector3D(0, -1, 0));
    Shape<Plane> topWallShape(topWall, RGBA_Color(0.8, 0.8, 0.8, 1.0));
    world.addObject(topWallShape);
    
    Plane bottomWall(Vector3D(0, -10, 0), Vector3D(0, 1, 0));
    Shape<Plane> bottomWallShape(bottomWall, RGBA_Color(0.2, 0.2, 0.8, 1.0));
    world.addObject(bottomWallShape);
    
    // Render scene
    Image depthImage = world.renderScene3DDepth(720, 720);
    
    // Basic checks
    assert(depthImage.getWidth() == 720);
    assert(depthImage.getHeight() == 720);
    
    // Check for rendered pixels
    assert(has_non_debug_pixel(depthImage));
    
    depthImage.toBitmapFile("test_world_3d_depth_output", "./test/test_by_product/world/");
    std::cout << "Note: World 3D depth render test completed - check output manually if needed" << std::endl;
}

void testWorldRenderScene3DLight() {
    World world;
    
    // Setup camera
    Vector3D origin(-10, -10, -5);
    Vector3D topRight = origin + Vector3D(20.0, 0, 0);
    Vector3D bottomLeft = origin + Vector3D(0, 20.0, 0);
    Rectangle viewport(origin, topRight, bottomLeft);
    world.getCamera().setViewport(viewport);
    
    // Add lights
    Light light1(Vector3D(0, 8, -2), RGBA_Color(1.0, 1.0, 1.0, 1.0), 1.0);
    world.addLight(light1);
    
    Light light2(Vector3D(-5, -5, 0), RGBA_Color(1.0, 1.0, 1.0, 1.0), 0.5);
    world.addLight(light2);
    
    // Add objects
    Sphere sphere(Vector3D(0, 0, 0), 4.0);
    Shape<Sphere> sphereShape(sphere, RGBA_Color(1, 1, 1, 1));
    world.addObject(sphereShape);
    
    Box box(Vector3D(5, 3, 10), 3.0, 3.0, 3.0, Vector3D(0, 0, 1));
    Shape<Box> boxShape(box, RGBA_Color(1, 0, 0, 1));
    world.addObject(boxShape);
    
    // Add walls
    Plane backWall(Vector3D(0, 0, 15), Vector3D(0, 0, -1));
    Shape<Plane> backWallShape(backWall, RGBA_Color(0.8, 0.2, 0.8, 1.0));
    world.addObject(backWallShape);
    
    Plane leftWall(Vector3D(-10, 0, 0), Vector3D(1, 0, 0));
    Shape<Plane> leftWallShape(leftWall, RGBA_Color(0.2, 0.8, 0.8, 1.0));
    world.addObject(leftWallShape);
    
    Plane rightWall(Vector3D(10, 0, 0), Vector3D(-1, 0, 0));
    Shape<Plane> rightWallShape(rightWall, RGBA_Color(0.8, 0.8, 0.2, 1.0));
    world.addObject(rightWallShape);
    
    Plane topWall(Vector3D(0, 10, 0), Vector3D(0, -1, 0));
    Shape<Plane> topWallShape(topWall, RGBA_Color(0.8, 0.8, 0.8, 1.0));
    world.addObject(topWallShape);
    
    Plane bottomWall(Vector3D(0, -10, 0), Vector3D(0, 1, 0));
    Shape<Plane> bottomWallShape(bottomWall, RGBA_Color(0.2, 0.2, 0.8, 1.0));
    world.addObject(bottomWallShape);
    
    // Render scene
    Image lightImage = world.renderScene3DLight(720, 720);
    
    // Basic checks
    assert(lightImage.getWidth() == 720);
    assert(lightImage.getHeight() == 720);
    
    // Check for rendered pixels
    assert(has_non_debug_pixel(lightImage)); 
       
    lightImage.toBitmapFile("test_world_3d_light_output", "./test/test_by_product/world/");
    std::cout << "Note: World 3D light render test completed - check output manually if needed" << std::endl;
}
