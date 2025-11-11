#ifndef RENDER_LOGGER_H
#define RENDER_LOGGER_H

// Internal libraries
#include "../../Lib/Rendering/Camera.h"
#include "../../Lib/Geometry/Vector3D.h"
#include "../../Lib/Rendering/Light.h"

// External libraries
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <string>

using namespace rendering;
using Vector3D = ::geometry::Vector3D;



// Logger class for render information
class RenderLogger {
private:
    std::ofstream logFile;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    
    std::string getCurrentTime();

public:
    RenderLogger(const std::string& testName);
    ~RenderLogger();

    // Scene logging methods
    void logSceneObjects(const math::Vector<Camera::ShapeVariant>& shapes);
    void logScene(const math::Vector<Camera::ShapeVariant>& shapes, const math::Vector<Light>& lights);
    void logCameraSettings(const Camera& camera, const Vector3D& origin);
    
    // Timing and output methods
    void logRenderTime();
    void logImageInfo(int width, int height);
    void logMessage(const std::string& message);
};

#endif // RENDER_LOGGER_H