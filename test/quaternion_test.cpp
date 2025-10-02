#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include "../Lib/Quaternion.h"
#include "../Lib/Vector3.h"
#include "../Lib/math_common.h"

using namespace math;

// Helper function for floating-point comparison
bool isEqual(double a, double b, double epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}

bool isQuaternionEqual(const Quaternion& q1, const Quaternion& q2, double epsilon = 1e-6) {
    return isEqual(q1.w(), q2.w(), epsilon) && 
           isEqual(q1.x(), q2.x(), epsilon) && 
           isEqual(q1.y(), q2.y(), epsilon) && 
           isEqual(q1.z(), q2.z(), epsilon);
}

bool isVector3Equal(const Vector3& v1, const Vector3& v2, double epsilon = 1e-6) {
    return isEqual(v1.x(), v2.x(), epsilon) && 
           isEqual(v1.y(), v2.y(), epsilon) && 
           isEqual(v1.z(), v2.z(), epsilon);
}

// Test function declarations
void testQuaternionConstructors();
void testQuaternionAccessors();
void testQuaternionOperators();
void testQuaternionMethods();
void testQuaternionRotations();
void testQuaternionInterpolation();
void testQuaternionConversions();
void testQuaternionErrorHandling();
void testQuaternionStaticMethods();

int main() {
    std::cout << "=== Quaternion Test Suite ===" << std::endl;
    
    try {
        testQuaternionConstructors();
        std::cout << "✓ Quaternion constructors test passed" << std::endl;
        
        testQuaternionAccessors();
        std::cout << "✓ Quaternion accessors test passed" << std::endl;
        
        testQuaternionOperators();
        std::cout << "✓ Quaternion operators test passed" << std::endl;
        
        testQuaternionMethods();
        std::cout << "✓ Quaternion methods test passed" << std::endl;
        
        testQuaternionRotations();
        std::cout << "✓ Quaternion rotations test passed" << std::endl;
        
        testQuaternionInterpolation();
        std::cout << "✓ Quaternion interpolation test passed" << std::endl;
        
        testQuaternionConversions();
        std::cout << "✓ Quaternion conversions test passed" << std::endl;
        
        testQuaternionStaticMethods();
        std::cout << "✓ Quaternion static methods test passed" << std::endl;
        
        testQuaternionErrorHandling();
        std::cout << "✓ Quaternion error handling test passed" << std::endl;
        
        std::cout << "\n🎉 All quaternion tests passed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void testQuaternionConstructors() {
    // Test default constructor (identity)
    Quaternion q1;
    assert(isEqual(q1.w(), 1.0));
    assert(isEqual(q1.x(), 0.0));
    assert(isEqual(q1.y(), 0.0));
    assert(isEqual(q1.z(), 0.0));
    
    // Test component constructor
    Quaternion q2(0.5, 0.5, 0.5, 0.5);
    assert(isEqual(q2.w(), 0.5));
    assert(isEqual(q2.x(), 0.5));
    assert(isEqual(q2.y(), 0.5));
    assert(isEqual(q2.z(), 0.5));
    
    // Test axis-angle constructor
    Vector3 axis(0, 0, 1);  // Z-axis
    double angle = math::pi / 2;  // 90 degrees
    Quaternion q3(axis, angle);
    
    // For 90° rotation around Z-axis: w = cos(45°), z = sin(45°)
    double expected = std::sqrt(2.0) / 2.0;  // cos(π/4) = sin(π/4)
    assert(isEqual(q3.w(), expected, 1e-5));
    assert(isEqual(q3.x(), 0.0, 1e-5));
    assert(isEqual(q3.y(), 0.0, 1e-5));
    assert(isEqual(q3.z(), expected, 1e-5));
    
    // Test Vector constructor
    Vector v({1.0, 0.0, 0.0, 0.0});
    Quaternion q4(v);
    assert(isEqual(q4.w(), 1.0));
    assert(isEqual(q4.x(), 0.0));
    assert(isEqual(q4.y(), 0.0));
    assert(isEqual(q4.z(), 0.0));
}

void testQuaternionAccessors() {
    Quaternion q(0.6, 0.8, 0.0, 0.0);
    
    // Test getters
    assert(isEqual(q.w(), 0.6));
    assert(isEqual(q.x(), 0.8));
    assert(isEqual(q.y(), 0.0));
    assert(isEqual(q.z(), 0.0));
    
    // Test setters
    q.setW(0.1);
    q.setX(0.2);
    q.setY(0.3);
    q.setZ(0.4);
    
    assert(isEqual(q.w(), 0.1));
    assert(isEqual(q.x(), 0.2));
    assert(isEqual(q.y(), 0.3));
    assert(isEqual(q.z(), 0.4));
    
    // Test bulk setter
    q.set(1.0, 2.0, 3.0, 4.0);
    assert(isEqual(q.w(), 1.0));
    assert(isEqual(q.x(), 2.0));
    assert(isEqual(q.y(), 3.0));
    assert(isEqual(q.z(), 4.0));
}

void testQuaternionOperators() {
    Quaternion q1(1.0, 2.0, 3.0, 4.0);
    Quaternion q2(0.5, 1.0, 1.5, 2.0);
    
    // Test addition
    Quaternion sum = q1 + q2;
    assert(isEqual(sum.w(), 1.5));
    assert(isEqual(sum.x(), 3.0));
    assert(isEqual(sum.y(), 4.5));
    assert(isEqual(sum.z(), 6.0));
    
    // Test subtraction
    Quaternion diff = q1 - q2;
    assert(isEqual(diff.w(), 0.5));
    assert(isEqual(diff.x(), 1.0));
    assert(isEqual(diff.y(), 1.5));
    assert(isEqual(diff.z(), 2.0));
    
    // Test scalar multiplication
    Quaternion scaled = q1 * 2.0;
    assert(isEqual(scaled.w(), 2.0));
    assert(isEqual(scaled.x(), 4.0));
    assert(isEqual(scaled.y(), 6.0));
    assert(isEqual(scaled.z(), 8.0));
    
    // Test quaternion multiplication (Hamilton product)
    Quaternion identity = Quaternion::identity();
    Quaternion result = q1 * identity;
    assert(isQuaternionEqual(result, q1));
}

void testQuaternionMethods() {
    // Test conjugate
    Quaternion q(1.0, 2.0, 3.0, 4.0);
    Quaternion conj = q.conjugate();
    assert(isEqual(conj.w(), 1.0));
    assert(isEqual(conj.x(), -2.0));
    assert(isEqual(conj.y(), -3.0));
    assert(isEqual(conj.z(), -4.0));
    
    // Test normalization
    Quaternion normalized = q.normalize();
    double length = normalized.length();
    assert(isEqual(length, 1.0, 1e-6));
    
    // Test vector part
    Vector3 vectorPart = q.vectorPart();
    assert(isEqual(vectorPart.x(), 2.0));
    assert(isEqual(vectorPart.y(), 3.0));
    assert(isEqual(vectorPart.z(), 4.0));
    
    // Test dot product
    Quaternion q2(0.5, 1.0, 1.5, 2.0);
    double dot = q.dot(q2);
    // 1*0.5 + 2*1 + 3*1.5 + 4*2 = 0.5 + 2 + 4.5 + 8 = 15
    assert(isEqual(dot, 15.0));
    
    // Test isUnit
    Quaternion unit = Quaternion::identity();
    assert(unit.isUnit());
    assert(!q.isUnit());
}

void testQuaternionRotations() {
    // Test 90° rotation around Z-axis
    Vector3 zAxis(0, 0, 1);
    Quaternion rotZ90(zAxis, math::pi / 2);
    
    // Rotate X-axis vector (1,0,0) by 90° around Z should give (0,1,0)
    Vector3 xAxis(1, 0, 0);
    Vector3 rotated = rotZ90 * xAxis;
    Vector3 expected(0, 1, 0);
    assert(isVector3Equal(rotated, expected, 1e-5));
    
    // Test 180° rotation around Y-axis
    Vector3 yAxis(0, 1, 0);
    Quaternion rotY180(yAxis, math::pi);
    
    // Rotate X-axis vector by 180° around Y should give (-1,0,0)
    Vector3 rotated180 = rotY180 * xAxis;
    Vector3 expected180(-1, 0, 0);
    assert(isVector3Equal(rotated180, expected180, 1e-5));
    
    // Test inverse rotation
    Quaternion inverse = rotZ90.inverse();
    Vector3 restored = inverse * rotated;
    assert(isVector3Equal(restored, xAxis, 1e-5));
}

void testQuaternionInterpolation() {
    // Test SLERP
    Quaternion start = Quaternion::identity();
    Vector3 axis(0, 0, 1);
    Quaternion end(axis, math::pi / 2);  // 90° rotation
    
    // Test at t = 0 (should return start)
    Quaternion slerp0 = Quaternion::slerp(start, end, 0.0);
    assert(isQuaternionEqual(slerp0, start, 1e-5));
    
    // Test at t = 1 (should return end)
    Quaternion slerp1 = Quaternion::slerp(start, end, 1.0);
    assert(isQuaternionEqual(slerp1, end, 1e-5));
    
    // Test at t = 0.5 (should be halfway)
    Quaternion slerp05 = Quaternion::slerp(start, end, 0.5);
    assert(slerp05.isUnit(1e-5));  // Should be normalized
    
    // Test NLERP
    Quaternion nlerp0 = Quaternion::nlerp(start, end, 0.0);
    assert(isQuaternionEqual(nlerp0, start, 1e-5));
    
    Quaternion nlerp1 = Quaternion::nlerp(start, end, 1.0);
    assert(isQuaternionEqual(nlerp1, end, 1e-5));
    
    Quaternion nlerp05 = Quaternion::nlerp(start, end, 0.5);
    assert(nlerp05.isUnit(1e-5));  // Should be normalized
}

void testQuaternionConversions() {
    // Test axis-angle conversion roundtrip
    Vector3 originalAxis(1, 1, 0);
    originalAxis = originalAxis.normal();
    double originalAngle = math::pi / 3;  // 60 degrees
    
    Quaternion q(originalAxis, originalAngle);
    
    Vector3 extractedAxis;
    double extractedAngle;
    
    try {
        q.toAxisAngle(extractedAxis, extractedAngle);
        
        // Check if axis and angle are recovered correctly
        assert(isVector3Equal(originalAxis, extractedAxis, 1e-5));
        assert(isEqual(originalAngle, extractedAngle, 1e-5));
    } catch (const std::invalid_argument&) {
        // This should not happen for non-identity quaternions
        assert(false);
    }
    
    // Test Euler angles conversion
    double roll = math::pi / 6;   // 30°
    double pitch = math::pi / 4;  // 45°
    double yaw = math::pi / 3;    // 60°
    
    Quaternion qEuler = Quaternion::fromEulerAngles(roll, pitch, yaw);
    assert(qEuler.isUnit(1e-5));
}

void testQuaternionStaticMethods() {
    // Test identity
    Quaternion identity = Quaternion::identity();
    assert(isEqual(identity.w(), 1.0));
    assert(isEqual(identity.x(), 0.0));
    assert(isEqual(identity.y(), 0.0));
    assert(isEqual(identity.z(), 0.0));
    
    // Test fromAxisAngle
    Vector3 axis(0, 1, 0);
    double angle = math::pi / 2;
    Quaternion q = Quaternion::fromAxisAngle(axis, angle);
    assert(q.isUnit(1e-5));
    
    // Test fromVectorToVector
    Vector3 from(1, 0, 0);
    Vector3 to(0, 1, 0);
    Quaternion rot = Quaternion::fromVectorToVector(from, to);
    
    Vector3 rotated = rot * from;
    assert(isVector3Equal(rotated, to, 1e-5));
    
    // Test IDENTITY constant
    assert(isQuaternionEqual(Quaternion::IDENTITY, identity));
}

void testQuaternionErrorHandling() {
    // Test invalid Vector constructor
    try {
        Vector v({1.0, 2.0, 3.0});  // Only 3 components
        Quaternion q(v);
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    // Test zero quaternion normalization
    try {
        Quaternion zero(0.0, 0.0, 0.0, 0.0);
        zero.normalize();
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    // Test zero quaternion inverse
    try {
        Quaternion zero(0.0, 0.0, 0.0, 0.0);
        zero.inverse();
        assert(false);  // Should not reach here
    } catch (const std::runtime_error&) {
        // Expected
    }
    
    // Test identity quaternion toAxisAngle
    try {
        Vector3 axis;
        double angle;
        Quaternion identity = Quaternion::identity();
        identity.toAxisAngle(axis, angle);
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected - identity quaternion represents no rotation
    }
    
    // Test interpolation with clamped values
    Quaternion q1 = Quaternion::identity();
    Quaternion q2(Vector3(0, 0, 1), math::pi / 2);
    
    // Test t < 0 (should clamp to 0)
    Quaternion result1 = Quaternion::slerp(q1, q2, -0.5);
    assert(isQuaternionEqual(result1, q1, 1e-5));
    
    // Test t > 1 (should clamp to 1)
    Quaternion result2 = Quaternion::slerp(q1, q2, 1.5);
    assert(isQuaternionEqual(result2, q2, 1e-5));
}