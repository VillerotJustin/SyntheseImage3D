# SyntheseImage3D Test Suite

This directory contains comprehensive tests for the SyntheseImage3D library components.

## Test Files

- **`vector_test.cpp`** - Tests for the template Vector<T> class and Vector3D geometry class
- **`quaternion_test.cpp`** - Tests for the Quaternion class rotation and mathematical operations
- **`rgba_color_test.cpp`** - Tests for the RGBA_Color class and color operations
- **`matrix_test.cpp`** - Tests for the template Matrix<T> class

## Running Tests

### Option 1: Using the Test Runner Script (Recommended)

The `run_tests.sh` script provides a comprehensive way to compile and run all tests:

```bash
# Run all tests (compile + execute)
./run_tests.sh

# Compile all tests only
./run_tests.sh compile

# Run all pre-compiled tests
./run_tests.sh run

# Clean test executables
./run_tests.sh clean

# Run individual tests
./run_tests.sh vector_test
./run_tests.sh quaternion_test
./run_tests.sh rgba_color_test
./run_tests.sh matrix_test

# Show help
./run_tests.sh help
```

### Option 2: Using the Makefile

```bash
# Run all tests (compile + execute)
make -f Makefile.tests

# Compile all tests only
make -f Makefile.tests compile

# Run all pre-compiled tests
make -f Makefile.tests run

# Run individual tests
make -f Makefile.tests run-vector
make -f Makefile.tests run-quaternion
make -f Makefile.tests run-rgba-color
make -f Makefile.tests run-matrix

# Clean executables
make -f Makefile.tests clean

# Show help
make -f Makefile.tests help
```

### Option 3: Manual Compilation

```bash
# Create executable directory
mkdir -p test/Executables

# Define library sources that need to be linked
LIB_SOURCES="Lib/Geometry/Quaternion.cpp Lib/Geometry/Vector3D.cpp Lib/Rendering/RGBA_Color.cpp"

# Compile individual tests
g++ -std=c++17 -Wall -Wextra -g -o test/Executables/vector_test test/vector_test.cpp $LIB_SOURCES
g++ -std=c++17 -Wall -Wextra -g -o test/Executables/quaternion_test test/quaternion_test.cpp $LIB_SOURCES
g++ -std=c++17 -Wall -Wextra -g -o test/Executables/rgba_color_test test/rgba_color_test.cpp $LIB_SOURCES
g++ -std=c++17 -Wall -Wextra -g -o test/Executables/matrix_test test/matrix_test.cpp $LIB_SOURCES

# Run tests
./test/Executables/vector_test
./test/Executables/quaternion_test
./test/Executables/rgba_color_test
./test/Executables/matrix_test
```

## Test Coverage

### Vector Tests
- Template Vector<T> pointer container functionality
- Vector3D mathematical operations (dot product, cross product, normalization)
- Constructor validation and error handling
- Geometric operations (angles, distances)

### Quaternion Tests
- Quaternion constructors and component access
- Mathematical operations (multiplication, conjugate, inverse)
- Rotation operations and vector rotations
- Interpolation (SLERP, NLERP)
- Axis-angle conversions
- Error handling for edge cases

### RGBA Color Tests
- Color component access and manipulation
- Color operations (blending, interpolation)
- Utility functions (grayscale conversion, clamping)
- Convenience color functions
- Vector integration and error handling

### Matrix Tests
- Template Matrix<T> pointer container functionality
- Matrix operations (transpose, element access)
- Constructor validation
- Memory management for pointer-based storage

## Expected Output

When all tests pass, you should see output similar to:

```
=== Vector and Vector3D Test Suite ===
✓ Vector constructors test passed
✓ Vector operators test passed
✓ Vector methods test passed
✓ Vector3D constructors test passed
✓ Vector3D methods test passed
✓ Vector3D operations test passed
✓ Error handling test passed

🎉 All tests passed successfully!
```

## Troubleshooting

If tests fail to compile:
1. Ensure you're in the project root directory
2. Check that all library headers are in place
3. Verify C++17 compiler support
4. Check include paths in test files

If tests fail to run:
1. Check that executables have proper permissions
2. Verify all dependencies are properly linked
3. Look for missing library implementations

## Contributing

When adding new tests:
1. Follow the existing test structure
2. Include comprehensive error checking
3. Add the new test to both the script and Makefile
4. Update this README with new test descriptions