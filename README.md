
# SyntheseImage3D

A comprehensive C++ library for 3D computer graphics and image synthesis, providing essential mathematical structures and operations for 3D rendering, computer vision, and geometric computations.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Library Architecture](#library-architecture)
- [Quick Start](#quick-start)
- [Building and Testing](#building-and-testing)
- [Usage Examples](#usage-examples)
- [API Reference](#api-reference)
- [Contributing](#contributing)
- [License](#license)

## 🎯 Overview

SyntheseImage3D is a modern C++ library that provides a comprehensive set of mathematical utilities specifically designed for 3D graphics programming, computer vision, and geometric computations. The library features a well-organized architecture with separate namespaces for different domains and template-based implementations for maximum flexibility and performance.

### Key Design Principles

- **Modular Architecture**: Organized into logical namespaces (`math`, `geometry`, `rendering`)
- **Template-based Design**: Generic implementations with pointer management for complex data structures
- **Performance-oriented**: Optimized for real-time graphics applications
- **Memory Safety**: Proper RAII and smart pointer usage patterns
- **Comprehensive Testing**: Extensive test suite with multiple execution methods
- **Modern C++17**: Leverages modern C++ features for better performance and safety

## ✨ Features

### Mathematical Structures

- **Vector Template**: Generic N-dimensional pointer-based container for mathematical objects
- **Vector3D**: Specialized 3D vector class with geometric operations (dot product, cross product, normalization)
- **Matrix Template**: Template-based matrix class for linear algebra operations with pointer management
- **Quaternion**: Complete quaternion implementation for 3D rotations with interpolation (SLERP, NLERP)
- **RGBA_Color**: Color manipulation class with blending, interpolation, and utility functions
- **Image**: Advanced image processing and manipulation utilities

### Supported Operations

- **Vector Operations**: Addition, subtraction, scalar multiplication, normalization, distance calculations
- **Matrix Operations**: Transpose, element access, memory management
- **Quaternion Operations**: Rotation composition, axis-angle conversion, interpolation
- **Geometric Calculations**: Dot/cross products, angles, distances, rotations
- **Color Operations**: Blending, interpolation, grayscale conversion, clamping
- **Image Processing**: Pixel manipulation, color matrix operations

## 🏗️ Library Architecture

### Namespace Organization

```cpp
math::           // Core mathematical utilities
├── Vector<T>    // Template pointer-based vector container
├── Matrix<T>    // Template pointer-based matrix container
└── math_common  // Mathematical constants and utility functions

geometry::       // 3D geometry and spatial operations
├── Vector3D     // 3D vector with geometric operations
└── Quaternion   // 3D rotation quaternions

rendering::      // Graphics and image processing
├── RGBA_Color   // Color representation and operations
├── Image        // Image processing and manipulation
└── Colors::     // Predefined color constants
```

### Directory Structure

```
SyntheseImage3D/
├── Lib/
│   ├── Math/           # Core mathematical templates
│   │   ├── Vector.hpp  # Template vector container (pointer-based)
│   │   ├── Matrix.hpp  # Template matrix container (pointer-based)
│   │   └── math_common.h # Mathematical constants and utilities
│   ├── Geometry/       # 3D geometry classes (value-based)
│   │   ├── Vector3D.h/.cpp    # 3D vector implementation
│   │   └── Quaternion.h/.cpp  # Quaternion implementation
│   └── Rendering/      # Graphics and imaging
│       ├── RGBA_Color.h/.cpp  # Color class
│       └── Image.h/.cpp       # Image processing
├── test/               # Comprehensive test suite
│   ├── vector_test.cpp     # Tests Vector<T> and Vector3D
│   ├── quaternion_test.cpp # Tests quaternion operations
│   ├── rgba_color_test.cpp # Tests color manipulation
│   ├── matrix_test.cpp     # Tests Matrix<T> template
│   ├── run_tests.sh        # Comprehensive test runner
│   ├── simple_test_runner.sh # Alternative test runner
│   ├── Makefile.tests      # Test compilation makefile
│   ├── Executables/        # Compiled test binaries
│   └── README.md           # Testing documentation
├── src/                # Example applications
├── Debug/              # Debug utilities
│   ├── debug_quaternion    # Quaternion debugging tool
│   └── debug_quaternion.cpp
└── SConstruct          # SCons build configuration
```

## 🚀 Quick Start

### Prerequisites

- C++17 compatible compiler (GCC, Clang, MSVC)
- Make (optional, for using Makefile)

### Basic Usage

```cpp
#include "Lib/Geometry/Vector3D.h"
#include "Lib/Geometry/Quaternion.h"
#include "Lib/Rendering/RGBA_Color.h"

using namespace geometry;
using namespace rendering;

int main() {
    // 3D vector operations
    Vector3D v1(1.0, 0.0, 0.0);
    Vector3D v2(0.0, 1.0, 0.0);
    Vector3D cross = v1.cross(v2);  // Result: (0, 0, 1)
    
    // Quaternion rotations
    Quaternion q(Vector3D(0, 0, 1), math::pi / 2);  // 90° around Z-axis
    Vector3D rotated = q * v1;  // Rotate v1 by quaternion
    
    // Color operations
    RGBA_Color red = Colors::red();
    RGBA_Color blue = Colors::blue();
    RGBA_Color purple = red.lerp(blue, 0.5);  // Interpolate colors
    
    return 0;
}
```

## 🔧 Building and Testing

### Automated Testing (Recommended)

The project includes comprehensive automated testing with multiple execution methods:

```bash
# Run all tests (compile + execute)
./run_tests.sh

# Or use the simple test runner
./simple_test_runner.sh

# Run individual tests
./run_tests.sh vector_test
./run_tests.sh quaternion_test
./run_tests.sh rgba_color_test
./run_tests.sh matrix_test

# Compilation only
./run_tests.sh compile
```

### Using Makefile

```bash
# Run all tests
make -f Makefile.tests

# Compile only
make -f Makefile.tests compile

# Run individual tests
make -f Makefile.tests run-vector
make -f Makefile.tests run-quaternion
```

### Manual Compilation

```bash
# Define library sources
LIB_SOURCES="Lib/Geometry/Quaternion.cpp Lib/Geometry/Vector3D.cpp Lib/Rendering/RGBA_Color.cpp"

# Compile with library linking
g++ -std=c++17 -Wall -Wextra -g your_program.cpp $LIB_SOURCES -o your_program
```

### Building with SCons (Project Build System)

```bash
# Build the entire project
scons -j 6 platform=linuxbsd dev_build=yes
```

## 🚀 Installation

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Make (optional, for using test Makefiles)
- SCons (optional, for main project builds)

### Quick Start

1. **Clone the repository**

   ```bash
   git clone https://github.com/VillerotJustin/SyntheseImage3D.git
   cd SyntheseImage3D
   ```

2. **Include in your project**

   ```cpp
   #include "Lib/Geometry/Vector3D.h"
   #include "Lib/Geometry/Quaternion.h"
   #include "Lib/Math/Vector.hpp"
   #include "Lib/Math/Matrix.hpp"
   #include "Lib/Rendering/RGBA_Color.h"
   
   using namespace geometry;  // For Vector3D, Quaternion
   using namespace math;      // For Vector<T>, Matrix<T>
   using namespace rendering; // For RGBA_Color
   ```

3. **Compile with your project**

   ```bash
   # For geometry classes (requires linking)
   g++ -std=c++17 -I. your_project.cpp \
       Lib/Geometry/Vector3D.cpp \
       Lib/Geometry/Quaternion.cpp \
       Lib/Rendering/RGBA_Color.cpp
   
   # For template classes only (header-only)
   g++ -std=c++17 -I. your_project.cpp
   ```

## 📖 Usage Examples

### Vector3D Operations

```cpp
#include "Lib/Geometry/Vector3D.h"
using namespace geometry;

// Create vectors
Vector3D v1(1.0, 2.0, 3.0);
Vector3D v2(4.0, 5.0, 6.0);

// Basic operations
Vector3D sum = v1 + v2;           // Addition
Vector3D diff = v2 - v1;          // Subtraction
Vector3D scaled = v1 * 2.0;       // Scalar multiplication

// Vector-specific operations
double dot_product = v1.dot(v2);         // Dot product
Vector3D cross_product = v1.cross(v2);   // Cross product
double angle = v1.angle(v2);             // Angle between vectors
Vector3D normalized = v1.normal();       // Normalized vector

// Length and distance
double length = v1.length();
double distance = v1.distance(v2);
```

### Template Vector<T> Operations

```cpp
#include "Lib/Math/Vector.hpp"
using namespace math;

// Create a vector container (pointer-based)
Vector<double> vec(5);  // 5-element vector

// Set values through pointer allocation
*vec[0] = 1.0;
*vec[1] = 2.0;
*vec[2] = 3.0;

// Vector operations
Vector<double> vec2(5);
*vec2[0] = 4.0; *vec2[1] = 5.0; *vec2[2] = 6.0;

bool equal = (vec == vec2);              // Equality check
Vector<double> sum = vec + vec2;         // Element-wise addition
Vector<double> scaled = vec * 2.0;       // Scalar multiplication

// Clean up memory
vec.clear();
vec2.clear();
```

### Matrix Operations

```cpp
#include "Lib/Math/Matrix.hpp"
using namespace math;

// Create a 3x3 matrix of double pointers
Matrix<double> matrix(3, 3);

// Set values through pointer allocation
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

### Image Processing Operations

```cpp
#include "Lib/Rendering/Image.h"
#include "Lib/Rendering/RGBA_Color.h"
using namespace rendering;

// Create and manipulate images
Image img(200, 150);  // 200x150 pixel image

// Fill with a color
RGBA_Color purple(0.8, 0.2, 0.6, 1.0);
img.fill(purple);

// Set individual pixels
img.setPixel(100, 75, RGBA_Color::white());
img.setPixel(50, 50, RGBA_Color::red());

// Get pixel information
const RGBA_Color* pixel = img.getPixel(100, 75);
std::cout << "Red: " << pixel->r() << std::endl;

// Create gradient pattern
for (int y = 0; y < img.getHeight(); ++y) {
    for (int x = 0; x < img.getWidth(); ++x) {
        double r = static_cast<double>(x) / img.getWidth();
        double g = static_cast<double>(y) / img.getHeight();
        img.setPixel(x, y, RGBA_Color(r, g, 0.5, 1.0));
    }
}

// Image transformations
Image originalImg = img.copy();  // Create backup
img.toGrayscale();              // Convert to grayscale
img.toBlackAndWhite(0.5);       // Black & white with 50% threshold
img.invertColors();             // Invert all colors

// Save to bitmap file
img.toBitmapFile("output.bmp", "images/");  // Saves to images/output.bmp
```

## 🧪 Testing

The project includes comprehensive test suites for all components with multiple execution methods.

### Test Files Overview

| Test File | Coverage | Components Tested |
|-----------|----------|-------------------|
| `vector_test.cpp` | Template Vector<T> & Vector3D | Pointer containers, 3D geometry operations |
| `quaternion_test.cpp` | Quaternion rotations | Mathematical operations, interpolation, conversions |
| `rgba_color_test.cpp` | Color manipulation | Blending, interpolation, utility functions |
| `matrix_test.cpp` | Template Matrix<T> | Pointer management, matrix operations |
| `image_test.cpp` | Image processing & bitmap output | Pixel operations, transformations, file export |

> **📋 For detailed testing documentation, see [`./test/README.md`](./test/README.md)**

## 📖 API Reference

### Template Vector\<T> Class (math namespace)

#### Constructors

- `Vector(int size)` - Create vector container of specified size (all pointers nullptr)
- `Vector(const Vector& other)` - Copy constructor (deep copy of pointers)

#### Operators

- `bool operator==(const Vector& other)` - Equality comparison (pointer-wise)
- `bool operator!=(const Vector& other)` - Inequality comparison
- `Vector operator+(const Vector& other)` - Element-wise addition
- `Vector operator-(const Vector& other)` - Element-wise subtraction
- `Vector operator*(double scalar)` - Scalar multiplication
- `T*& operator[](int index)` - Access pointer at index

#### Methods

- `int size() const` - Get number of elements
- `void clear()` - Set all pointers to nullptr
- `bool empty() const` - Check if vector is empty

### Vector3D Class (geometry namespace)

#### Constructors

- `Vector3D()` - Default constructor (0, 0, 0)
- `Vector3D(double x, double y, double z)` - Component constructor
- `Vector3D(const Vector3D& other)` - Copy constructor

#### Component Access

- `double x(), y(), z() const` - Get components
- `void setX(double), setY(double), setZ(double)` - Set components

#### Operators

- `Vector3D operator+(const Vector3D& other)` - Vector addition
- `Vector3D operator-(const Vector3D& other)` - Vector subtraction
- `Vector3D operator*(double scalar)` - Scalar multiplication
- `bool operator==(const Vector3D& other)` - Equality comparison

#### Methods

- `Vector3D cross(const Vector3D& other) const` - Cross product
- `double dot(const Vector3D& other) const` - Dot product
- `double angle(const Vector3D& other) const` - Angle between vectors
- `Vector3D normal() const` - Return normalized vector
- `void normalize()` - Normalize in place
- `double length() const` - Vector magnitude
- `double distance(const Vector3D& other) const` - Distance to other vector
- `bool parallel(const Vector3D& other) const` - Check if parallel

#### Static Constants

- `Vector3D::ZERO` - (0, 0, 0)
- `Vector3D::UNIT_X` - (1, 0, 0)
- `Vector3D::UNIT_Y` - (0, 1, 0)
- `Vector3D::UNIT_Z` - (0, 0, 1)

### Template Matrix\<T> Class (math namespace)

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

### Quaternion Class (geometry namespace)

#### Constructors

- `Quaternion()` - Default constructor (identity quaternion: w=1, x=0, y=0, z=0)
- `Quaternion(double w, double x, double y, double z)` - Component constructor
- `Quaternion(double angle, const Vector3D& axis)` - Axis-angle constructor
- `Quaternion(const Vector3D& from, const Vector3D& to)` - Rotation between vectors

#### Component Access

- `double w(), x(), y(), z() const` - Get quaternion components
- `Vector3D vector() const` - Get vector part (x, y, z)
- `double scalar() const` - Get scalar part (w)

#### Operators

- `Quaternion operator*(const Quaternion& other) const` - Hamilton product (quaternion composition)
- `Quaternion& operator*=(const Quaternion& other)` - In-place multiplication
- `Vector3D operator*(const Vector3D& v) const` - Rotate vector by quaternion
- `bool operator==(const Quaternion& other) const` - Equality comparison (with epsilon tolerance)
- `bool operator!=(const Quaternion& other) const` - Inequality comparison

#### Mathematical Operations

- `Quaternion conjugate() const` - Get conjugate quaternion
- `double norm() const` - Get quaternion magnitude
- `Quaternion normalize() const` - Get normalized quaternion
- `Quaternion& normalize_()` - Normalize in-place
- `Quaternion inverse() const` - Get inverse quaternion
- `bool isUnit() const` - Check if quaternion is unit length

#### Rotation Operations

- `Vector3D getAxis() const` - Get rotation axis
- `double getAngle() const` - Get rotation angle in radians
- `double angleTo(const Quaternion& other) const` - Angle between quaternions
- `void toAxisAngle(Vector3D& axis, double& angle) const` - Extract axis-angle representation

#### Interpolation

- `static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, double t)` - Spherical linear interpolation
- `static Quaternion nlerp(const Quaternion& q1, const Quaternion& q2, double t)` - Normalized linear interpolation
- `static Quaternion slerpShortest(const Quaternion& q1, const Quaternion& q2, double t)` - SLERP via shortest path

#### Conversion Methods

- `math::Matrix<double> toRotationMatrix3x3() const` - Convert to 3x3 rotation matrix
- `static Quaternion fromRotationMatrix(const math::Matrix<double>& m)` - Create from rotation matrix
- `static Quaternion fromEulerAngles(double roll, double pitch, double yaw)` - Create from Euler angles
- `static Quaternion fromVectorToVector(const Vector3D& from, const Vector3D& to)` - Create rotation between vectors

#### Static Methods

- `static Quaternion identity()` - Get identity quaternion
- `static Quaternion lookRotation(const Vector3D& forward, const Vector3D& up)` - Create look-at rotation

### RGBA_Color Class (rendering namespace)

#### Constructors

- `RGBA_Color()` - Default constructor (transparent black: 0, 0, 0, 0)
- `RGBA_Color(double r, double g, double b, double a = 1.0)` - Component constructor
- `RGBA_Color(const math::Vector<double>& v)` - Create from 4-element vector

#### Component Access

- `double r(), g(), b(), a() const` - Get color components (red, green, blue, alpha)
- `void setR(double), setG(double), setB(double), setA(double)` - Set individual components
- `void setRGBA(double r, double g, double b, double a = 1.0)` - Set all components at once

#### Operators

- `RGBA_Color operator+(const RGBA_Color& other) const` - Color addition
- `RGBA_Color operator-(const RGBA_Color& other) const` - Color subtraction
- `RGBA_Color operator*(double scalar) const` - Scalar multiplication
- `RGBA_Color operator*(const RGBA_Color& other) const` - Component-wise multiplication
- `bool operator==(const RGBA_Color& other) const` - Equality comparison
- `bool operator!=(const RGBA_Color& other) const` - Inequality comparison

#### Color Operations

- `RGBA_Color lerp(const RGBA_Color& other, double t) const` - Linear interpolation between colors
- `RGBA_Color alphaBlend(const RGBA_Color& background) const` - Alpha blending with background
- `RGBA_Color clamp() const` - Clamp components to [0.0, 1.0] range
- `RGBA_Color toGrayscale(double rw = 0.299, double gw = 0.587, double bw = 0.114) const` - Convert to grayscale
- `void invert()` - Invert color components (in-place)

#### Static Color Factory Methods

- `static RGBA_Color black(), white()` - Monochrome colors
- `static RGBA_Color red(), green(), blue()` - Primary colors  
- `static RGBA_Color transparent()` - Fully transparent color

### Image Class (rendering namespace)

#### Constructors

- `Image(int width, int height)` - Create image with dimensions (initialized to black)
- `Image(const math::Matrix<RGBA_Color>& colorMatrix)` - Create from color matrix
- `Image(const Image& other)` - Copy constructor (deep copy)

#### Dimension Access

- `int getWidth() const` - Get image width
- `int getHeight() const` - Get image height
- `int getNumPixels() const` - Get total pixel count
- `bool isValid() const` - Check if image has valid dimensions and non-null pixels

#### Pixel Operations

- `const RGBA_Color* getPixel(int x, int y) const` - Get pixel pointer at coordinates
- `void setPixel(int x, int y, const RGBA_Color& color)` - Set pixel color
- `const math::Matrix<RGBA_Color>& getPixelMatrix() const` - Get internal pixel matrix

#### Image Processing

- `void fill(const RGBA_Color& color)` - Fill entire image with color
- `void clear()` - Clear image (set all pixels to black)
- `void toGrayscale()` - Convert entire image to grayscale (in-place)
- `void toBlackAndWhite(double threshold = 0.5)` - Convert to black & white with threshold
- `void invertColors()` - Invert all colors in the image (in-place)

#### File Operations

- `void toBitmapFile(const std::string& filename, const std::string& filePath = "./") const` - Export to BMP file format

#### Utility Methods

- `Image copy() const` - Create a deep copy of the image
- `Image& operator=(const Image& other)` - Assignment operator (deep copy)

### Mathematical Utilities (math namespace)

#### Constants

- `constexpr double pi` - Mathematical constant π (3.14159...)

#### Utility Functions

- `double square(double x)` - Calculate x²
- `double triangle_area(double a, double b, double c)` - Calculate triangle area using Heron's formula

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
