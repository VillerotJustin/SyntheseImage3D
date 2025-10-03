#include <iostream>
#include "Lib/Math/Vector.hpp"

using namespace math;

int main() {
    std::cout << "Testing Vector with empty initialization..." << std::endl;
    
    try {
        // Test 1: Create empty vector
        Vector<double> v(0);
        std::cout << "Empty vector created, size: " << v.size() << std::endl;
        
        // Test 2: Append to empty vector
        double* val1 = new double(1.0);
        std::cout << "About to append to empty vector..." << std::endl;
        v.append(val1);
        std::cout << "Append successful, new size: " << v.size() << std::endl;
        
        // Test 3: Access the appended element
        std::cout << "About to access v[0]..." << std::endl;
        std::cout << "v[0] = " << v[0] << " (should be " << val1 << ")" << std::endl;
        
        // Test 4: Append another element
        double* val2 = new double(2.0);
        std::cout << "About to append second element..." << std::endl;
        v.append(val2);
        std::cout << "Second append successful, new size: " << v.size() << std::endl;
        
        // Test 5: Access both elements
        std::cout << "Accessing elements: v[0] = " << v[0] << ", v[1] = " << v[1] << std::endl;
        
        delete val1;
        delete val2;
        
        std::cout << "All tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}