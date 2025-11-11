#include "Logger.h"
#include "../../Lib/Geometry/Sphere.h"
#include "../../Lib/Geometry/Box.h"
#include "../../Lib/Geometry/Rectangle.h"
#include "../../Lib/Geometry/Circle.h"
#include "../../Lib/Geometry/Plane.h"

RenderLogger::RenderLogger(const std::string& testName) {
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

RenderLogger::~RenderLogger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void RenderLogger::logSceneObjects(const math::Vector<Camera::ShapeVariant>& shapes) {
    logScene(shapes, math::Vector<Light>());
}

void RenderLogger::logScene(const math::Vector<Camera::ShapeVariant>& shapes, const math::Vector<Light>& lights) {
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

void RenderLogger::logCameraSettings(const Camera& camera, const Vector3D& origin) {
    logFile << "Camera Configuration:" << std::endl;
    logFile << "--------------------" << std::endl;
    logFile << "Origin: " << origin << std::endl;
    logFile << "Direction: " << camera.getDirection() << std::endl;
    logFile << "Field of View: " << camera.getFOVAngle() << std::endl;
    logFile << std::endl;
}

void RenderLogger::logRenderTime() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    logFile << "Render Time: " << duration.count() << " ms" << std::endl << std::endl;
    logFile << "-------------" << std::endl;
    logFile << std::endl;
}

void RenderLogger::logImageInfo(int width, int height) {
    logFile << "Output Image:" << std::endl;
    logFile << "-------------" << std::endl;
    logFile << "Dimensions: " << width << "x" << height << " pixels" << std::endl;
    logFile << std::endl;
}

void RenderLogger::logMessage(const std::string& message) {
    logFile << message << std::endl;
}

std::string RenderLogger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}