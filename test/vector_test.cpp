#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include "../Lib/Math/Vector.hpp"
#include "../Lib/Geometry/Vector3D.h"

using namespace math;
using namespace geometry;

// Helper function for floating-point comparison
bool isEqual(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
}

// Test function declarations
void testVectorConstructors();
void testVectorOperators();
void testVectorMethods();
void testVector3Constructors();
void testVector3Methods();
void testVector3Operations();
void testErrorHandling();

int main() {
    std::cout << "=== Vector and Vector3D Test Suite ===" << std::endl;
    
    try {
        testVectorConstructors();
        std::cout << "✓ Vector constructors test passed" << std::endl;
        
        testVectorOperators();
        std::cout << "✓ Vector operators test passed" << std::endl;
        
        testVectorMethods();
        std::cout << "✓ Vector methods test passed" << std::endl;
        
        testVector3Constructors();
        std::cout << "✓ Vector3D constructors test passed" << std::endl;
        
        testVector3Methods();
        std::cout << "✓ Vector3D methods test passed" << std::endl;
        
        testVector3Operations();
        std::cout << "✓ Vector3D operations test passed" << std::endl;
        
        testErrorHandling();
        std::cout << "✓ Error handling test passed" << std::endl;
        
        std::cout << "\n🎉 All tests passed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void testVectorConstructors() {
    // Test default size constructor
    Vector<double> v1(3);
    assert(v1.size() == 3);
    assert(v1[0] == nullptr);
    assert(v1[1] == nullptr);
    assert(v1[2] == nullptr);
    
    // Test pointer array constructor
    double* values[3];
    values[0] = new double(1.0);
    values[1] = new double(2.0);
    values[2] = new double(3.0);
    
    Vector<double> v2(values, 3);
    assert(v2.size() == 3);
    assert(*v2[0] == 1.0);
    assert(*v2[1] == 2.0);
    assert(*v2[2] == 3.0);
    
    // Test default constructor
    Vector<double> v3;
    assert(v3.size() == 0);
    
    // Clean up
    delete values[0];
    delete values[1];
    delete values[2];
}

void testVectorOperators() {
    // Create test values
    double* val1 = new double(1.0);
    double* val2 = new double(2.0);
    double* val3 = new double(3.0);
    
    Vector<double> v1(3);
    v1[0] = val1;
    v1[1] = val2;
    v1[2] = val3;
    
    Vector<double> v2(3);
    v2[0] = val1;  // Same pointer
    v2[1] = val2;  // Same pointer
    v2[2] = val3;  // Same pointer
    
    Vector<double> v3(3);
    double* val4 = new double(4.0);
    v3[0] = val4;  // Different pointer
    v3[1] = val2;  // Same pointer
    v3[2] = val3;  // Same pointer
    
    // Test equality (pointer comparison)
    assert(v1 == v2);
    assert(v1 != v3);
    
    // Test inequality
    assert(!(v1 != v2));
    assert(v1 != v3);
    
    // Clean up
    delete val1;
    delete val2;
    delete val3;
    delete val4;
}

void testVectorMethods() {
    Vector<double> v1(3);
    
    // Test clear method
    double* val1 = new double(1.0);
    double* val2 = new double(2.0);
    v1[0] = val1;
    v1[1] = val2;
    
    v1.clear();
    assert(v1[0] == nullptr);
    assert(v1[1] == nullptr);
    assert(v1[2] == nullptr);
    
    // Test empty check
    assert(v1.empty());
    
    v1[0] = val1;
    assert(!v1.empty());
    
    // Test size
    assert(v1.size() == 3);
    
    // Clean up
    delete val1;
    delete val2;
}

void testVector3Constructors() {
    // Test default constructor
    Vector3D v1;
    assert(v1.size() == 3);
    assert(isEqual(v1.x(), 0.0));
    assert(isEqual(v1.y(), 0.0));
    assert(isEqual(v1.z(), 0.0));
    
    // Test parameterized constructor
    Vector3D v2(1.0, 2.0, 3.0);
    assert(isEqual(v2.x(), 1.0));
    assert(isEqual(v2.y(), 2.0));
    assert(isEqual(v2.z(), 3.0));
    
    // Test Vector to Vector3D conversion
    Vector<double> v(3);
    *v[0] = 4.0; *v[1] = 5.0; *v[2] = 6.0;
    Vector3D v3(v);
    assert(isEqual(v3.x(), 4.0));
    assert(isEqual(v3.y(), 5.0));
    assert(isEqual(v3.z(), 6.0));
    
    // Clean up
    delete v[0]; delete v[1]; delete v[2];
    
    // Test static constants
    assert(isEqual(Vector3D::ZERO.x(), 0.0));
    assert(isEqual(Vector3D::UNIT_X.x(), 1.0));
    assert(isEqual(Vector3D::UNIT_Y.y(), 1.0));
    assert(isEqual(Vector3D::UNIT_Z.z(), 1.0));
}

void testVector3Methods() {
    Vector3D v1(1.0, 2.0, 3.0);
    
    // Test getters and setters
    v1.setX(5.0);
    v1.setY(6.0);
    v1.setZ(7.0);
    assert(isEqual(v1.x(), 5.0));
    assert(isEqual(v1.y(), 6.0));
    assert(isEqual(v1.z(), 7.0));
    
    v1.set(1.0, 2.0, 3.0);
    assert(isEqual(v1.x(), 1.0));
    assert(isEqual(v1.y(), 2.0));
    assert(isEqual(v1.z(), 3.0));
    
    // Test inherited methods
    assert(isEqual(v1.length(), std::sqrt(14.0)));
    assert(isEqual(v1.lengthSquared(), 14.0));
    
    Vector3D v2(2.0, 3.0, 4.0);
    assert(isEqual(v1.dot(v2), 20.0));  // 1*2 + 2*3 + 3*4 = 20
    
    // Test normalization
    Vector3D v_normal = v1.normal();
    assert(isEqual(v_normal.length(), 1.0));
    
    // Test in-place normalization
    Vector3D v3(3.0, 4.0, 0.0);
    v3.normalize();
    assert(isEqual(v3.length(), 1.0));
    assert(isEqual(v3.x(), 0.6));
    assert(isEqual(v3.y(), 0.8));
}

void testVector3Operations() {
    Vector3D v1(1.0, 0.0, 0.0);
    Vector3D v2(0.0, 1.0, 0.0);
    
    // Test cross product
    Vector3D cross = v1.cross(v2);
    assert(isEqual(cross.x(), 0.0));
    assert(isEqual(cross.y(), 0.0));
    assert(isEqual(cross.z(), 1.0));
    
    // Test parallel check
    Vector3D v3(2.0, 0.0, 0.0);
    assert(v1.parallel(v3));
    assert(!v1.parallel(v2));
    
    // Test angle calculation
    double angle = v1.angle(v2);
    assert(isEqual(angle, M_PI/2.0));  // 90 degrees
    
    // Test operators
    Vector3D v_add = v1 + v2;
    assert(isEqual(v_add.x(), 1.0));
    assert(isEqual(v_add.y(), 1.0));
    assert(isEqual(v_add.z(), 0.0));
    
    Vector3D v_sub = v1 - v2;
    assert(isEqual(v_sub.x(), 1.0));
    assert(isEqual(v_sub.y(), -1.0));
    assert(isEqual(v_sub.z(), 0.0));
    
    Vector3D v_mult = v1 * 3.0;
    assert(isEqual(v_mult.x(), 3.0));
    assert(isEqual(v_mult.y(), 0.0));
    assert(isEqual(v_mult.z(), 0.0));
}

void testErrorHandling() {
    // Test out of bounds access
    Vector<double> v(3);
    try {
        v[-1];  // Out of bounds
        assert(false);  // Should not reach here
    } catch (const std::out_of_range&) {
        // Expected
    }
    
    try {
        v[3];  // Out of bounds
        assert(false);  // Should not reach here
    } catch (const std::out_of_range&) {
        // Expected
    }
    
    // Test zero vector3D normalization
    Vector3D zero_v3;
    try {
        zero_v3.normalize();
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    // Test Vector3D conversion with wrong size
    Vector<double> wrong_size(2);
    try {
        Vector3D v3(wrong_size);
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    // Test angle with zero vector
    Vector3D zero_v3_test;
    Vector3D unit_x(1.0, 0.0, 0.0);
    try {
        zero_v3_test.angle(unit_x);
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
}