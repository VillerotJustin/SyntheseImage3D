#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include "../Lib/Math/Vector.h"
#include "../Lib/Math/Vector3.h"

using namespace math;

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
    std::cout << "=== Vector and Vector3 Test Suite ===" << std::endl;
    
    try {
        testVectorConstructors();
        std::cout << "✓ Vector constructors test passed" << std::endl;
        
        testVectorOperators();
        std::cout << "✓ Vector operators test passed" << std::endl;
        
        testVectorMethods();
        std::cout << "✓ Vector methods test passed" << std::endl;
        
        testVector3Constructors();
        std::cout << "✓ Vector3 constructors test passed" << std::endl;
        
        testVector3Methods();
        std::cout << "✓ Vector3 methods test passed" << std::endl;
        
        testVector3Operations();
        std::cout << "✓ Vector3 operations test passed" << std::endl;
        
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
    Vector v1(3);
    assert(v1.size() == 3);
    assert(isEqual(v1.at(0), 0.0));
    assert(isEqual(v1.at(1), 0.0));
    assert(isEqual(v1.at(2), 0.0));
    
    // Test size with value constructor
    Vector v2(4, 2.5);
    assert(v2.size() == 4);
    assert(isEqual(v2.at(0), 2.5));
    assert(isEqual(v2.at(3), 2.5));
    
    // Test std::vector constructor
    std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0};
    Vector v3(data);
    assert(v3.size() == 5);
    assert(isEqual(v3.at(0), 1.0));
    assert(isEqual(v3.at(4), 5.0));
}

void testVectorOperators() {
    Vector v1({1.0, 2.0, 3.0});
    Vector v2({4.0, 5.0, 6.0});
    
    // Test equality
    Vector v3({1.0, 2.0, 3.0});
    assert(v1 == v3);
    assert(v1 != v2);
    
    // Test addition
    Vector v_add = v1 + v2;
    assert(isEqual(v_add.at(0), 5.0));
    assert(isEqual(v_add.at(1), 7.0));
    assert(isEqual(v_add.at(2), 9.0));
    
    // Test subtraction
    Vector v_sub = v2 - v1;
    assert(isEqual(v_sub.at(0), 3.0));
    assert(isEqual(v_sub.at(1), 3.0));
    assert(isEqual(v_sub.at(2), 3.0));
    
    // Test scalar multiplication
    Vector v_mult = v1 * 2.0;
    assert(isEqual(v_mult.at(0), 2.0));
    assert(isEqual(v_mult.at(1), 4.0));
    assert(isEqual(v_mult.at(2), 6.0));
}

void testVectorMethods() {
    Vector v1(std::vector<double>{3.0, 4.0});  // Length should be 5.0
    
    // Test length calculations
    assert(isEqual(v1.length(), 5.0));
    assert(isEqual(v1.squared_length(), 25.0));
    
    // Test dot product
    Vector v2(std::vector<double>{1.0, 2.0});
    assert(isEqual(v1.dot(v2), 11.0));  // 3*1 + 4*2 = 11
    
    // Test normalization
    Vector v_normal = v1.normal();
    assert(isEqual(v_normal.length(), 1.0));
    assert(isEqual(v_normal.at(0), 0.6));
    assert(isEqual(v_normal.at(1), 0.8));
    
    // Test distance
    Vector v3(std::vector<double>{0.0, 0.0});
    assert(isEqual(v1.distance(v3), 5.0));
    assert(isEqual(v1.squared_distance(v3), 25.0));
    
    // Test zero check
    assert(v3.zero());
    assert(!v1.zero());
}

void testVector3Constructors() {
    // Test default constructor
    Vector3 v1;
    assert(v1.size() == 3);
    assert(isEqual(v1.x(), 0.0));
    assert(isEqual(v1.y(), 0.0));
    assert(isEqual(v1.z(), 0.0));
    
    // Test parameterized constructor
    Vector3 v2(1.0, 2.0, 3.0);
    assert(isEqual(v2.x(), 1.0));
    assert(isEqual(v2.y(), 2.0));
    assert(isEqual(v2.z(), 3.0));
    
    // Test Vector to Vector3 conversion
    Vector v({4.0, 5.0, 6.0});
    Vector3 v3(v);
    assert(isEqual(v3.x(), 4.0));
    assert(isEqual(v3.y(), 5.0));
    assert(isEqual(v3.z(), 6.0));
    
    // Test static constants
    assert(isEqual(Vector3::ZERO.x(), 0.0));
    assert(isEqual(Vector3::UNIT_X.x(), 1.0));
    assert(isEqual(Vector3::UNIT_Y.y(), 1.0));
    assert(isEqual(Vector3::UNIT_Z.z(), 1.0));
}

void testVector3Methods() {
    Vector3 v1(1.0, 2.0, 3.0);
    
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
    
    Vector3 v2(2.0, 3.0, 4.0);
    assert(isEqual(v1.dot(v2), 20.0));  // 1*2 + 2*3 + 3*4 = 20
    
    // Test normalization
    Vector3 v_normal = v1.normal();
    assert(isEqual(v_normal.length(), 1.0));
    
    // Test in-place normalization
    Vector3 v3(3.0, 4.0, 0.0);
    v3.normalize();
    assert(isEqual(v3.length(), 1.0));
    assert(isEqual(v3.x(), 0.6));
    assert(isEqual(v3.y(), 0.8));
}

void testVector3Operations() {
    Vector3 v1(1.0, 0.0, 0.0);
    Vector3 v2(0.0, 1.0, 0.0);
    
    // Test cross product
    Vector3 cross = v1.cross(v2);
    assert(isEqual(cross.x(), 0.0));
    assert(isEqual(cross.y(), 0.0));
    assert(isEqual(cross.z(), 1.0));
    
    // Test parallel check
    Vector3 v3(2.0, 0.0, 0.0);
    assert(v1.parallel(v3));
    assert(!v1.parallel(v2));
    
    // Test angle calculation
    double angle = v1.angle(v2);
    assert(isEqual(angle, M_PI/2.0));  // 90 degrees
    
    // Test operators
    Vector3 v_add = v1 + v2;
    assert(isEqual(v_add.x(), 1.0));
    assert(isEqual(v_add.y(), 1.0));
    assert(isEqual(v_add.z(), 0.0));
    
    Vector3 v_sub = v1 - v2;
    assert(isEqual(v_sub.x(), 1.0));
    assert(isEqual(v_sub.y(), -1.0));
    assert(isEqual(v_sub.z(), 0.0));
    
    Vector3 v_mult = v1 * 3.0;
    assert(isEqual(v_mult.x(), 3.0));
    assert(isEqual(v_mult.y(), 0.0));
    assert(isEqual(v_mult.z(), 0.0));
}

void testErrorHandling() {
    // Test out of bounds access
    Vector v(3);
    try {
        (void)v.at(-1);  // Cast to void to suppress unused return value warning
        assert(false);  // Should not reach here
    } catch (const std::out_of_range&) {
        // Expected
    }
    
    try {
        (void)v.at(3);  // Cast to void to suppress unused return value warning
        assert(false);  // Should not reach here
    } catch (const std::out_of_range&) {
        // Expected
    }
    
    // Test zero vector normalization
    Vector zero_v(3, 0.0);
    try {
        zero_v.normal();
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    Vector3 zero_v3;
    try {
        zero_v3.normalize();
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    // Test different size vector operations
    Vector v1(3);
    Vector v2(4);
    try {
        v1 + v2;
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    // Test Vector3 conversion with wrong size
    Vector wrong_size(2);
    try {
        Vector3 v3(wrong_size);
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    // Test angle with zero vector
    Vector3 zero_v3_test;
    Vector3 unit_x(1.0, 0.0, 0.0);
    try {
        zero_v3_test.angle(unit_x);
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
}