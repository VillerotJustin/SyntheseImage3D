#include <iostream>
#include "../Lib/Math/Matrix.hpp"

// Example class to demonstrate the Matrix template
class TestObject {
public:
    int value;
    
    TestObject(int v) : value(v) {}
    
    friend std::ostream& operator<<(std::ostream& os, const TestObject& obj) {
        return os << "TestObject(" << obj.value << ")";
    }
};

int main() {
    std::cout << "Testing Matrix<TestObject> template class:" << std::endl;
    
    // Create a 2x3 matrix of TestObject pointers
    math::Matrix<TestObject> matrix(2, 3);
    
    std::cout << "\n1. Initial matrix (should be all nullptr):" << std::endl;
    std::cout << matrix << std::endl;
    
    // Create some test objects
    TestObject* obj1 = new TestObject(10);
    TestObject* obj2 = new TestObject(20);
    TestObject* obj3 = new TestObject(30);
    
    // Set some values in the matrix
    matrix(0, 0) = obj1;
    matrix(0, 1) = obj2;
    matrix(1, 2) = obj3;
    
    std::cout << "2. Matrix after setting some values:" << std::endl;
    std::cout << matrix << std::endl;
    
    // Test copy constructor
    math::Matrix<TestObject> copy_matrix(matrix);
    std::cout << "3. Copied matrix:" << std::endl;
    std::cout << copy_matrix << std::endl;
    
    // Test transpose
    math::Matrix<TestObject> transposed = matrix.transpose();
    std::cout << "4. Transposed matrix (3x2):" << std::endl;
    std::cout << transposed << std::endl;
    
    // Test accessing objects through pointers
    std::cout << "5. Accessing objects through matrix pointers:" << std::endl;
    if (matrix(0, 0) != nullptr) {
        std::cout << "matrix(0,0) points to: " << *matrix(0, 0) << std::endl;
    }
    if (matrix(0, 1) != nullptr) {
        std::cout << "matrix(0,1) points to: " << *matrix(0, 1) << std::endl;
    }
    if (matrix(1, 2) != nullptr) {
        std::cout << "matrix(1,2) points to: " << *matrix(1, 2) << std::endl;
    }
    
    // Test clear
    matrix.clear();
    std::cout << "\n6. Matrix after clear():" << std::endl;
    std::cout << matrix << std::endl;
    
    // Clean up dynamically allocated objects
    delete obj1;
    delete obj2;
    delete obj3;
    
    std::cout << "\nMatrix dimensions: " << matrix.getRows() << "x" << matrix.getCols() << std::endl;
    std::cout << "Test completed successfully!" << std::endl;
    
    return 0;
}