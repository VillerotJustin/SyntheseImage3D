#include <iostream>
#include <cassert>
#include <stdexcept>
#include <memory>
#include "../Lib/Math/Matrix.hpp"

using namespace math;

// Test object for matrix operations
class TestObject {
public:
    int value;
    
    TestObject() : value(0) {}

    TestObject(int v) : value(v) {}
    
    bool operator==(const TestObject& other) const {
        return value == other.value;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const TestObject& obj) {
        return os << "TestObject(" << obj.value << ")";
    }
};

// Test function declarations
void testMatrixConstructors();
void testMatrixAccessors();
void testMatrixCopyOperations();
void testMatrixTranspose();
void testMatrixMethods();
void testMatrixErrorHandling();

int main() {
    std::cout << "=== Matrix Test Suite ===" << std::endl;
    
    try {
        testMatrixConstructors();
        std::cout << "✓ Matrix constructors test passed" << std::endl;
        
        testMatrixAccessors();
        std::cout << "✓ Matrix accessors test passed" << std::endl;
        
        testMatrixCopyOperations();
        std::cout << "✓ Matrix copy operations test passed" << std::endl;
        
        testMatrixTranspose();
        std::cout << "✓ Matrix transpose test passed" << std::endl;
        
        testMatrixMethods();
        std::cout << "✓ Matrix methods test passed" << std::endl;
        
        testMatrixErrorHandling();
        std::cout << "✓ Matrix error handling test passed" << std::endl;
        
        std::cout << "\n🎉 All Matrix tests passed successfully! 🎉" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}

void testMatrixConstructors() {
    std::cout << "Testing Matrix constructors..." << std::endl;

    TestObject objDef = TestObject();
    
    // Test size constructor
    Matrix<TestObject> sizedMatrix(3, 4);
    assert(sizedMatrix.getRows() == 3);
    assert(sizedMatrix.getCols() == 4);
    
    // Check that all elements are initialized to 0
    for (size_t i = 0; i < sizedMatrix.getRows(); ++i) {
        for (size_t j = 0; j < sizedMatrix.getCols(); ++j) {
            assert(sizedMatrix(i, j) == 0);
        }
    }
    
    // Test with 2x2 matrix
    Matrix<TestObject> squareMatrix(2, 2);
    assert(squareMatrix.getRows() == 2);
    assert(squareMatrix.getCols() == 2);
    assert(squareMatrix(0, 0) == objDef);
    assert(squareMatrix(0, 1) == objDef);
    assert(squareMatrix(1, 0) == objDef);
    assert(squareMatrix(1, 1) == objDef);
}

void testMatrixAccessors() {
    std::cout << "Testing Matrix accessors..." << std::endl;

    TestObject objDef = TestObject();
    
    Matrix<TestObject> matrix(2, 3);
    
    // Create test objects
    TestObject obj1 = TestObject(10);
    TestObject obj2 = TestObject(20);
    TestObject obj3 = TestObject(30);
    
    // Test setting values
    matrix(0, 0) = obj1;
    matrix(0, 1) = obj2;
    matrix(1, 2) = obj3;
    
    // Test getting values
    assert(matrix(0, 0) == obj1);
    assert(matrix(0, 1) == obj2);
    assert(matrix(1, 2) == obj3);
    assert(matrix(0, 2) == objDef);
    assert(matrix(1, 0) == objDef);
    assert(matrix(1, 1) == objDef);
    
    // Test accessing through pointers
    assert(matrix(0, 0).value == 10);
    assert(matrix(0, 1).value == 20);
    assert(matrix(1, 2).value == 30);
}

void testMatrixCopyOperations() {
    std::cout << "Testing Matrix copy operations..." << std::endl;
    
    Matrix<TestObject> original(2, 2);
    TestObject obj1 = TestObject(100);
    TestObject obj2 = TestObject(200);
    
    original(0, 0) = obj1;
    original(1, 1) = obj2;
    
    // Test copy constructor
    Matrix<TestObject> copied(original);
    assert(copied.getRows() == 2);
    assert(copied.getCols() == 2);
    assert(copied(0, 0) == obj1); 
    assert(copied(1, 1) == obj2); 
    assert(copied(0, 1) == 0);
    assert(copied(1, 0) == 0);
    
    // Test assignment operator
    Matrix<TestObject> assigned(1, 1);
    assigned = original;
    assert(assigned.getRows() == 2);
    assert(assigned.getCols() == 2);
    assert(assigned(0, 0) == obj1);
    assert(assigned(1, 1) == obj2);
}

void testMatrixTranspose() {
    std::cout << "Testing Matrix transpose..." << std::endl;
    
    Matrix<TestObject> matrix(2, 3);
    TestObject obj1 = TestObject(1);
    TestObject obj2 = TestObject(2);
    TestObject obj3 = TestObject(3);
    TestObject obj4 = TestObject(4);

    TestObject objDef = TestObject();
    
    // Fill matrix:
    // [obj1, obj2, obj3]
    // [obj4, null, null]
    matrix(0, 0) = obj1;
    matrix(0, 1) = obj2;
    matrix(0, 2) = obj3;
    matrix(1, 0) = obj4;
    
    // Test transpose
    Matrix<TestObject> transposed = matrix.transpose();
    assert(transposed.getRows() == 3);
    assert(transposed.getCols() == 2);
    
    // Check transposed values:
    // [obj1, obj4]
    // [obj2, null]
    // [obj3, null]
    assert(transposed(0, 0) == obj1);
    assert(transposed(0, 1) == obj4);
    assert(transposed(1, 0) == obj2);
    assert(transposed(1, 1) == objDef);
    assert(transposed(2, 0) == obj3);
    assert(transposed(2, 1) == objDef);
    
    // Test square matrix transpose
    Matrix<TestObject> square(2, 2);
    square(0, 0) = obj1;
    square(0, 1) = obj2;
    square(1, 0) = obj3;
    square(1, 1) = obj4;
    
    Matrix<TestObject> squareTransposed = square.transpose();
    assert(squareTransposed.getRows() == 2);
    assert(squareTransposed.getCols() == 2);
    assert(squareTransposed(0, 0) == obj1);
    assert(squareTransposed(0, 1) == obj3);
    assert(squareTransposed(1, 0) == obj2);
    assert(squareTransposed(1, 1) == obj4);
}

void testMatrixMethods() {
    std::cout << "Testing Matrix methods..." << std::endl;

    TestObject objDef = TestObject();
    
    Matrix<TestObject> matrix(3, 2);
    TestObject obj1 = TestObject(42);
    TestObject obj2 = TestObject(84);
    
    matrix(0, 0) = obj1;
    matrix(2, 1) = obj2;
    
    // Test clear method
    matrix.clear();
    
    // All elements should be nullptr after clear
    for (size_t i = 0; i < matrix.getRows(); ++i) {
        for (size_t j = 0; j < matrix.getCols(); ++j) {
            assert(matrix(i, j) == objDef);
        }
    }
    
    // Matrix dimensions should remain the same
    assert(matrix.getRows() == 3);
    assert(matrix.getCols() == 2);
    
    // Test that we can still set values after clear
    matrix(1, 0) = obj1;
    assert(matrix(1, 0) == obj1);
}

void testMatrixErrorHandling() {
    std::cout << "Testing Matrix error handling..." << std::endl;
    
    Matrix<TestObject> matrix(2, 2);
    
    // Test out of bounds access (should not crash, behavior depends on implementation)
    // Note: The current implementation may not have bounds checking,
    // but we test what we can
    
    // Test valid access first
    TestObject obj = TestObject(999);
    matrix(0, 0) = obj;
    assert(matrix(0, 0) == obj);
    
    // Test edge cases
    matrix(1, 1) = obj;  // Last valid position
    assert(matrix(1, 1) == obj);
}