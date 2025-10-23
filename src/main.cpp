#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>
#include "../Lib/Geometry/Vector3D.h"
#include "../Lib/Geometry/Plane.h"
#include "../Lib/Geometry/Sphere.h"
#include "../Lib/Geometry/Box.h"
#include "../Lib/Geometry/Rectangle.h"
#include "../Lib/Geometry/Circle.h"
#include "../Lib/Geometry/Quaternion.h"
#include "../Lib/Rendering/Camera.h"
#include "../Lib/Rendering/Shape.hpp"
#include "../Lib/Rendering/Video.h"
#include "../Lib/Rendering/World.h"
#include "../Lib/Math/Vector.hpp"

using namespace geometry;
using namespace rendering;
using namespace math;

/**
 * Test scene rendering with camera
 * The scene is a cube composed of 6 planes and a sphere in the center
 * Other shapes are arranged around the shape in an ellicoidal pattern.
 * The camera is positioned to capture the inside of the cube and the sphere.
 * Camera makes a 360 degree rotation around the scene center while rendering frames and pointing towards the sphere.
 * The frames are saved as images to create an animation via the video object.
 */
int main() {
    try {
        std::cout << "=== 3D Scene Animation Renderer ===" << std::endl;
        std::cout << "Starting scene setup..." << std::endl;
    
        // Scene parameters
        const double cubeSize = 50.0;
        const size_t frameCount = 36; // 36 frames for 360 degrees (10 degrees per frame)
        const size_t imageWidth = 720;
        const size_t imageHeight = 480;

        std::cout << "Scene parameters:" << std::endl;
        std::cout << "  - Cube size: " << cubeSize << " units" << std::endl;
        std::cout << "  - Frame count: " << frameCount << " frames" << std::endl;
        std::cout << "  - Image resolution: " << imageWidth << "x" << imageHeight << " pixels" << std::endl;
        std::cout << "  - Rotation per frame: " << (360.0 / frameCount) << " degrees" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Creating geometric shapes..." << std::endl;
        
        // Create cube from 6 planes
        Vector3D cubeCenter(0, 0, 0);
        double halfSize = cubeSize / 2.0;
        
        std::cout << "Building cube walls:" << std::endl;
        
        // Front plane (z = +halfSize)
        Plane frontPlane(Vector3D(0, 0, halfSize), Vector3D(0, 0, -1));
        Shape<Plane> frontShape(frontPlane, RGBA_Color(1.0, 0.0, 0.0, 1.0)); // Red
        std::cout << "  ✓ Front wall (Red)" << std::endl;
        
        // Back plane (z = -halfSize)
        Plane backPlane(Vector3D(0, 0, -halfSize), Vector3D(0, 0, 1));
        Shape<Plane> backShape(backPlane, RGBA_Color(0.0, 1.0, 0.0, 1.0)); // Green
        std::cout << "  ✓ Back wall (Green)" << std::endl;
        
        // Left plane (x = -halfSize)
        Plane leftPlane(Vector3D(-halfSize, 0, 0), Vector3D(1, 0, 0));
        Shape<Plane> leftShape(leftPlane, RGBA_Color(0.0, 0.0, 1.0, 1.0)); // Blue
        std::cout << "  ✓ Left wall (Blue)" << std::endl;
        
        // Right plane (x = +halfSize)
        Plane rightPlane(Vector3D(halfSize, 0, 0), Vector3D(-1, 0, 0));
        Shape<Plane> rightShape(rightPlane, RGBA_Color(1.0, 1.0, 0.0, 1.0)); // Yellow
        std::cout << "  ✓ Right wall (Yellow)" << std::endl;
        
        // Top plane (y = +halfSize)
        Plane topPlane(Vector3D(0, halfSize, 0), Vector3D(0, -1, 0));
        Shape<Plane> topShape(topPlane, RGBA_Color(1.0, 0.0, 1.0, 1.0)); // Magenta
        std::cout << "  ✓ Top wall (Magenta)" << std::endl;
        
        // Bottom plane (y = -halfSize)
        Plane bottomPlane(Vector3D(0, -halfSize, 0), Vector3D(0, 1, 0));
        Shape<Plane> bottomShape(bottomPlane, RGBA_Color(0.0, 1.0, 1.0, 1.0)); // Cyan
        std::cout << "  ✓ Bottom wall (Cyan)" << std::endl;
        
        // Central sphere
        double sphereRadius = 1.0;
        Sphere centralSphere(Vector3D(0, 0, 0), sphereRadius);
        Shape<Sphere> sphereShape(centralSphere, RGBA_Color(1.0, 1.0, 1.0, 1.0)); // White
        std::cout << "  ✓ Central sphere (White, radius: " << sphereRadius << ")" << std::endl;
        std::cout << std::endl;


        // Other shapes in ellicoidal pattern around the center
        //  --- IGNORE ---
        // This section is intentionally left blank

        std::cout << "Setting up camera system..." << std::endl;
        // Camera setup with new constructor
        double cameraDistance = cubeSize - 1.0; // Distance from center
        // Calculate viewport dimensions to match image aspect ratio (800:600 = 4:3)
        double aspectRatio = static_cast<double>(imageWidth) / static_cast<double>(imageHeight); // 800/600 = 1.333...
        double viewportHeight = 10.0; // Base viewport height
        double viewportWidth = viewportHeight / aspectRatio; // Width = height * aspect ratio
        Vector3D cameraPosition(0, 0, cameraDistance); // Initial position
        Vector3D cameraDirection(0, 0, -1); // Look toward center
        
        std::cout << "Camera configuration:" << std::endl;
        std::cout << "  - Distance from center: " << cameraDistance << " units" << std::endl;
        std::cout << "  - Viewport size: " << viewportWidth << " x " << viewportHeight << " units" << std::endl;
        std::cout << "  - Aspect ratio: " << aspectRatio << " (matching " << imageWidth << "x" << imageHeight << ")" << std::endl;
        std::cout << "  - Initial position: (" << cameraPosition.x() << ", " << cameraPosition.y() << ", " << cameraPosition.z() << ")" << std::endl;
        
        // Create camera with position, viewport dimensions, and direction
        // Build corners so that Length aligns with the X axis and Width aligns with the Y axis in camera-local space
        Vector3D topLeft  = cameraPosition + Vector3D(-viewportWidth / 2.0,  viewportHeight / 2.0, 0.0);
        Vector3D topRight = cameraPosition + Vector3D( viewportWidth / 2.0,  viewportHeight / 2.0, 0.0);
        Vector3D bottomLeft = cameraPosition + Vector3D(-viewportWidth / 2.0, -viewportHeight / 2.0, 0.0);
        Camera camera(Rectangle(topLeft, topRight, bottomLeft));
        
        // Debug: Print actual viewport dimensions and aspect ratio
        std::cout << "  ✓ Camera created successfully" << std::endl;
        std::cout << "  DEBUG: Viewport Length (l): " << camera.getViewport().getLength() << std::endl;
        std::cout << "  DEBUG: Viewport Width (w): " << camera.getViewport().getWidth() << std::endl;
        std::cout << "  DEBUG: Viewport Aspect Ratio (w/l): " << camera.getViewportAspectRatio() << std::endl;
        std::cout << "  DEBUG: Image Aspect Ratio: " << (static_cast<double>(imageWidth) / imageHeight) << std::endl;
        std::cout << std::endl;
        
        std::cout << "Building 3D world..." << std::endl;
        // World setup
        World world;
        world.addObject(frontShape);
        world.addObject(backShape);
        world.addObject(leftShape);
        world.addObject(rightShape);
        world.addObject(topShape);
        world.addObject(bottomShape);
        world.addObject(sphereShape);
        
        // Set camera in world
        world.getCamera() = camera;
        
        std::cout << "  ✓ Added 6 cube walls to world" << std::endl;
        std::cout << "  ✓ Added central sphere to world" << std::endl;
        std::cout << "  ✓ Camera attached to world" << std::endl;
        std::cout << "  Total objects in scene: " << world.getObjectCount() << std::endl;
        std::cout << std::endl;

        // Set up the rendering context
        Video video(imageWidth, imageHeight, 30.0); // 30 FPS

        // Render loop for 360-degree rotation
        for (size_t frame = 0; frame < frameCount; ++frame) {
            // Calculate rotation angle
            double angle = frame * (360.0 / frameCount) * M_PI / 180.0;
            
            // Update camera position in a circle around the center
            Vector3D newPosition(
                cameraDistance * sin(angle),
                0.0,
                cameraDistance * cos(angle)
            );
            
            // Update camera direction to always look at center
            Vector3D newDirection = (Vector3D(0, 0, 0) - newPosition).normal();
            
            // Update camera
            world.getCamera().setPosition(newPosition);
            world.getCamera().setDirection(newDirection);

            std::cout << "  - Viewport aspect ratio: " << world.getCamera().getViewportAspectRatio() << std::endl;
            std::cout << "  - Image aspect ratio: " << aspectRatio << std::endl;

            // Render frame
            video.addFrame(world.renderScene3DDepth(imageWidth, imageHeight));

            // Save frame (you can add video saving logic here)
            std::cout << "Rendered frame " << frame + 1 << "/" << frameCount << std::endl;
        }

        // Save video (you can implement video saving logic here)
        video.exportToFile("TheCubeFrames", "./TheCube", VideoFormat::MKV);

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return 1;
    }
}
