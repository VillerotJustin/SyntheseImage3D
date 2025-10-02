#include <iostream>
#include <cmath>
#include "Lib/Vector3.h"
#include "Lib/Quaternion.h"

using namespace math;

int main() {
    // Create Z-axis and 90° rotation
    Vector3 zAxis(0, 0, 1);
    double angle = pi / 2;
    Quaternion rotZ90(zAxis, angle);
    
    std::cout << "Quaternion components (w, x, y, z): " 
              << rotZ90.w() << ", " << rotZ90.x() << ", " 
              << rotZ90.y() << ", " << rotZ90.z() << std::endl;
    
    // Test rotation of X-axis
    Vector3 xAxis(1, 0, 0);
    Vector3 rotated = rotZ90 * xAxis;
    
    std::cout << "Original vector: (" << xAxis.x() << ", " << xAxis.y() << ", " << xAxis.z() << ")" << std::endl;
    std::cout << "Rotated vector: (" << rotated.x() << ", " << rotated.y() << ", " << rotated.z() << ")" << std::endl;
    std::cout << "Expected: (0, 1, 0)" << std::endl;
    
    return 0;
}