
# SyntheseImage3D

A C++ mathematical library for 3D computer graphics and image synthesis, providing essential mathematical structures and operations for 3D rendering, computer vision, and geometric computations.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Library Components](#library-components)
- [Installation](#installation)
- [Usage Examples](#usage-examples)
- [Testing](#testing)
- [API Reference](#api-reference)
- [Contributing](#contributing)
- [License](#license)

## 🎯 Overview

This project provides a comprehensive set of mathematical utilities specifically designed for 3D graphics programming. The library includes optimized implementations of vectors, matrices, quaternions, and image processing utilities that are commonly needed in computer graphics, game development, and computer vision applications.

### Key Design Principles

- **Template-based**: Generic implementations that work with different numeric types
- **Performance-oriented**: Optimized for real-time graphics applications
- **Header-only templates**: Easy integration into existing projects
- **Comprehensive testing**: Extensive test suite ensuring reliability
- **Modern C++**: Uses C++11 features for better performance and safety

## ✨ Features

### Mathematical Structures
- **Vector**: N-dimensional vector operations with SIMD optimization potential
- **Vector3**: Specialized 3D vector class with cross product, angle calculations
- **Matrix**: Template-based matrix class for linear algebra operations
- **Quaternion**: Quaternion operations for 3D rotations
- **Image**: Image processing and manipulation utilities

### Operations Supported
- Vector arithmetic (addition, subtraction, scalar multiplication)
- Matrix operations (multiplication, transpose, determinant)
- Quaternion rotations and interpolation
- Dot and cross products
- Distance and angle calculations
- Normalization and orthogonalization

## 📚 Library Components

### Core Math (`Lib/Math`)

```
Lib/Math
├── math_common.h     # Common mathematical constants and utilities
├── Vector.h/.cpp     # N-dimensional vector implementation
├── Vector3.h/.cpp    # Specialized 3D vector class
├── Matrix.h          # Template matrix class (header-only)
├── Quaternion.h/.cpp # Quaternion implementation
```

### Core Math (`Lib/Rendering`)

```
Lib/Rendering
├── 
```

### Tests (`test/`)

```
test/
├── matrix_test.cpp     # Matrix class comprehensive tests
├── vector_test.cpp     # Vector and Vector3 tests
├── quaternion_test.cpp # Quaternion tests
└── Executables/        # Compiled test binaries
```

## 🚀 Installation

### Prerequisites

- C++ compiler with C++11 support (GCC 4.8+, Clang 3.4+, MSVC 2015+)
- CMake 3.10+ (optional, for build system integration)

### Quick Start

1. **Clone the repository**
   ```bash
   git clone https://github.com/VillerotJustin/SyntheseImage3D.git
   cd SyntheseImage3D
   ```

2. **Include in your project**
   ```cpp
   #include "Lib/Vector3.h"
   #include "Lib/Matrix.h"
   #include "Lib/Quaternion.h"
   
   using namespace math;
   ```

3. **Compile with your project**
   ```bash
   g++ -std=c++11 -I./Lib your_project.cpp Lib/Vector.cpp Lib/Vector3.cpp
   ```

## 📖 Usage Examples

### Vector Operations

```cpp
#include "Lib/Vector3.h"
using namespace math;

// Create vectors
Vector3 v1(1.0, 2.0, 3.0);
Vector3 v2(4.0, 5.0, 6.0);

// Basic operations
Vector3 sum = v1 + v2;           // Addition
Vector3 diff = v2 - v1;          // Subtraction
Vector3 scaled = v1 * 2.0;       // Scalar multiplication

// Vector-specific operations
double dot_product = v1.dot(v2);         // Dot product
Vector3 cross_product = v1.cross(v2);    // Cross product
double angle = v1.angle(v2);             // Angle between vectors
Vector3 normalized = v1.normal();        // Normalized vector

// Length and distance
double length = v1.length();
double distance = v1.distance(v2);
```

### Matrix Operations

```cpp
#include "Lib/Matrix.h"
using namespace math;

// Create a 3x3 matrix of double pointers
Matrix<double> matrix(3, 3);

// Set values
matrix(0, 0) = new double(1.0);
matrix(0, 1) = new double(2.0);
matrix(1, 0) = new double(3.0);

// Matrix operations
Matrix<double> transposed = matrix.transpose();
matrix.clear();  // Set all pointers to nullptr

// Access dimensions
int rows = matrix.getRows();
int cols = matrix.getCols();
```

### Quaternion Operations

```cpp
#include "Lib/Quaternion.h"
using namespace math;

// Create quaternions
Quaternion identity = Quaternion::identity();              // No rotation
Quaternion rotation(Vector3(0,0,1), math::pi/2);         // 90° around Z-axis

// Quaternion operations
Quaternion combined = rotation * identity;                 // Composition
Vector3 rotated = rotation * Vector3(1, 0, 0);           // Rotate vector
Quaternion inverse = rotation.inverse();                  // Opposite rotation

// Conversion methods
Vector3 axis;
double angle;
rotation.toAxisAngle(axis, angle);                        // Extract axis/angle

Matrix<double> rotMatrix = rotation.toRotationMatrix3x3(); // To 3x3 matrix

// Interpolation
Quaternion start = Quaternion::identity();
Quaternion end(Vector3(0,1,0), math::pi/4);
Quaternion interpolated = Quaternion::slerp(start, end, 0.5); // Smooth interpolation

// Factory methods
Quaternion fromEuler = Quaternion::fromEulerAngles(0.1, 0.2, 0.3);
Quaternion fromVectors = Quaternion::fromVectorToVector(
    Vector3(1,0,0), Vector3(0,1,0)
);
```

## 🧪 Testing

The project includes comprehensive test suites for all components.

### Running All Tests

```bash
# Compile and run Matrix tests
g++ -std=c++11 -I. -o test/Executables/matrix_test test/matrix_test.cpp
./test/Executables/matrix_test

# Compile and run Vector tests
g++ -std=c++11 -I. -o test/Executables/vector_test test/vector_test.cpp Lib/Vector.cpp Lib/Vector3.cpp
./test/Executables/vector_test

# Compile and run Quaternion tests
g++ -std=c++11 -I. -o test/Executables/quaternion_test test/quaternion_test.cpp Lib/Quaternion.cpp Lib/Vector.cpp Lib/Vector3.cpp
./test/Executables/quaternion_test
```

### Expected Test Output

#### Matrix Tests
```
Testing Matrix<TestObject> template class:

1. Initial matrix (should be all nullptr):
nullptr nullptr nullptr
nullptr nullptr nullptr

2. Matrix after setting some values:
[memory addresses displayed]

3. Copied matrix:
[memory addresses displayed]

4. Transposed matrix (3x2):
[memory addresses displayed]

Matrix dimensions: 2x3
Test completed successfully!
```

#### Vector Tests
```
=== Vector and Vector3 Test Suite ===
✓ Vector constructors test passed
✓ Vector operators test passed
✓ Vector methods test passed
✓ Vector3 constructors test passed
✓ Vector3 methods test passed
✓ Vector3 operations test passed
✓ Error handling test passed

🎉 All tests passed successfully!
```

#### Quaternion Tests
```
=== Quaternion Test Suite ===
✓ Quaternion constructors test passed
✓ Quaternion accessors test passed
✓ Quaternion operators test passed
✓ Quaternion methods test passed
✓ Quaternion rotations test passed
✓ Quaternion interpolation test passed
✓ Quaternion conversions test passed
✓ Quaternion static methods test passed
✓ Quaternion error handling test passed

🎉 All quaternion tests passed successfully!
```

### Quaternion Class

The `Quaternion` class extends `Vector` to provide complete quaternion operations for 3D rotations.

#### Constructors

- `Quaternion()` - Default constructor (identity quaternion: w=1, x=0, y=0, z=0)
- `Quaternion(double w, double x, double y, double z)` - Component constructor
- `Quaternion(double angle, const Vector3& axis)` - Axis-angle constructor
- `Quaternion(const Vector3& from, const Vector3& to)` - Rotation between vectors

#### Component Access

- `double w(), x(), y(), z()` - Get quaternion components
- `Vector3 vector()` - Get vector part (x, y, z)
- `double scalar()` - Get scalar part (w)

#### Quaternion Operations

- `Quaternion operator*(const Quaternion& other)` - Hamilton product
- `Quaternion& operator*=(const Quaternion& other)` - In-place multiplication
- `bool operator==(const Quaternion& other)` - Equality comparison (with epsilon)
- `Quaternion conjugate()` - Get conjugate quaternion
- `double norm()` - Get quaternion magnitude
- `Quaternion normalize()` - Get normalized quaternion
- `Quaternion& normalize_()` - Normalize in-place
- `bool isUnit()` - Check if quaternion is unit length

#### Rotation Operations

- `Vector3 rotate(const Vector3& v)` - Rotate vector by quaternion
- `Vector3 getAxis()` - Get rotation axis
- `double getAngle()` - Get rotation angle
- `double angleTo(const Quaternion& other)` - Angle between quaternions

#### Interpolation

- `static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, double t)` - Spherical linear interpolation
- `static Quaternion nlerp(const Quaternion& q1, const Quaternion& q2, double t)` - Normalized linear interpolation
- `static Quaternion slerpShortest(const Quaternion& q1, const Quaternion& q2, double t)` - SLERP via shortest path

#### Matrix Conversion

- `Matrix<double> toRotationMatrix()` - Convert to 3x3 rotation matrix
- `static Quaternion fromRotationMatrix(const Matrix<double>& m)` - Create from rotation matrix

#### Static Constants

- `Quaternion::IDENTITY` - Identity quaternion (1, 0, 0, 0)

### Matrix&lt;T&gt; Class
```

## 📖 API Reference

### Vector Class

#### Constructors
- `Vector(int size)` - Create vector of specified size with all elements initialized to 0.0
- `Vector(int size, double value)` - Create vector of specified size with all elements initialized to given value
- `Vector(std::vector<double> data)` - Create vector from std::vector of doubles

#### Operators
- `bool operator==(const Vector& other)` - Equality comparison (element-wise)
- `bool operator!=(const Vector& other)` - Inequality comparison (element-wise)
- `Vector operator+(const Vector& other)` - Element-wise addition
- `Vector operator-(const Vector& other)` - Element-wise subtraction
- `Vector operator*(double scalar)` - Scalar multiplication
- `friend std::ostream& operator<<(std::ostream& os, const Vector& v)` - Output stream operator

#### Methods
- `double at(int index)` - Access element at index with bounds checking
- `int size()` - Get the number of elements in the vector
- `bool zero()` - Check if all elements are zero
- `Vector normal()` - Return normalized vector (unit length)
- `double dot(const Vector& other)` - Calculate dot product with another vector
- `double length()` - Calculate vector magnitude/length
- `double squared_length()` - Calculate squared length (more efficient, no sqrt)
- `double distance(const Vector& other)` - Calculate Euclidean distance to another vector
- `double squared_distance(const Vector& other)` - Calculate squared distance (more efficient)

#### Exceptions
- `std::out_of_range` - Thrown by `at()` when index is out of bounds
- `std::invalid_argument` - Thrown when vector sizes don't match for operations or when normalizing zero vector

### Vector3 Class

#### Constructors
- `Vector3()` - Default constructor (0, 0, 0)
- `Vector3(double x, double y, double z)` - Component constructor
- `Vector3(const Vector& v)` - Convert from general Vector

#### Methods
- `double x(), y(), z()` - Component accessors
- `void setX(double), setY(double), setZ(double)` - Component mutators
- `Vector3 cross(const Vector3& other)` - Cross product
- `double dot(const Vector3& other)` - Dot product
- `double angle(const Vector3& other)` - Angle between vectors
- `Vector3 normal()` - Return normalized vector
- `void normalize()` - Normalize in place
- `double length()` - Vector magnitude
- `bool parallel(const Vector3& other)` - Check if parallel

#### Static Constants
- `Vector3::ZERO` - (0, 0, 0)
- `Vector3::UNIT_X` - (1, 0, 0)
- `Vector3::UNIT_Y` - (0, 1, 0)
- `Vector3::UNIT_Z` - (0, 0, 1)

### Matrix<T> Class

#### Constructors
- `Matrix(int rows, int cols)` - Create matrix with dimensions
- `Matrix(T*** array, int rows, int cols)` - Create from 2D array
- `Matrix()` - Default 1x1 matrix

#### Methods
- `T*& operator()(int x, int y)` - Element access
- `Matrix transpose()` - Return transposed matrix
- `void clear()` - Set all elements to nullptr
- `int getRows(), getCols()` - Dimension accessors
- `void swapRows(int r1, int r2)` - Swap matrix rows

## 🤝 Contributing

We welcome contributions! Please see our contributing guidelines:

1. **Fork the repository**
2. **Create a feature branch**: `git checkout -b feature/new-feature`
3. **Write tests** for your changes
4. **Ensure all tests pass**
5. **Submit a pull request**

### Code Style
- Use consistent indentation (4 spaces)
- Follow C++ naming conventions
- Document public APIs with Doxygen comments
- Include comprehensive error handling

### Reporting Issues
- Use GitHub Issues for bug reports
- Include minimal reproduction examples
- Specify compiler and platform information

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Based on mathematical concepts from computer graphics literature
- Vector implementation inspired by [vraid/earthgen-old](https://github.com/vraid/earthgen-old)
- Matrix implementation inspired by [akalicki/matrix](https://github.com/akalicki/matrix)
- Documentation generated with assistance from Copilot

## 📞 Contact

- **Author**: Justin Villerot
- **GitHub**: [@VillerotJustin](https://github.com/VillerotJustin)
- **Project Link**: [https://github.com/VillerotJustin/SyntheseImage3D](https://github.com/VillerotJustin/SyntheseImage3D)

---

**Note**: This library is actively developed and APIs may change between versions. Please check the changelog for breaking changes when updating.