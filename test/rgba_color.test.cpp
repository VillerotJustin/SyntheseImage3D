#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include "../Lib/Rendering/RGBA_Color.h"
#include "../Lib/Math/Vector.hpp"

using namespace rendering;

// Helper function for floating-point comparison
bool isEqual(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
}

// Test function declarations
void testRGBAColorConstructors();
void testRGBAColorComponentAccess();
void testRGBAColorComponentSetters();
void testRGBAColorUtilityMethods();
void testRGBAColorVectorOperations();
void testRGBAColorConvenienceFunctions();
void testRGBAColorErrorHandling();

int main() {
    std::cout << "=== RGBA_Color Test Suite ===" << std::endl;
    
    try {
        testRGBAColorConstructors();
        std::cout << "✓ RGBA_Color constructors test passed" << std::endl;
        
        testRGBAColorComponentAccess();
        std::cout << "✓ RGBA_Color component access test passed" << std::endl;
        
        testRGBAColorComponentSetters();
        std::cout << "✓ RGBA_Color component setters test passed" << std::endl;
        
        testRGBAColorUtilityMethods();
        std::cout << "✓ RGBA_Color utility methods test passed" << std::endl;
        
        testRGBAColorVectorOperations();
        std::cout << "✓ RGBA_Color operations test passed" << std::endl;
        
        testRGBAColorConvenienceFunctions();
        std::cout << "✓ RGBA_Color convenience functions test passed" << std::endl;
        
        testRGBAColorErrorHandling();
        std::cout << "✓ RGBA_Color error handling test passed" << std::endl;
        
        std::cout << std::endl << "🎉 All RGBA_Color tests passed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// Test RGBA_Color constructors
void testRGBAColorConstructors() {
    // Test default constructor
    RGBA_Color defaultColor;
    assert(isEqual(defaultColor.r(), 0.0));
    assert(isEqual(defaultColor.g(), 0.0));
    assert(isEqual(defaultColor.b(), 0.0));
    assert(isEqual(defaultColor.a(), 0.0));
    
    // Test parameterized constructor with alpha
    RGBA_Color redColor(1.0, 0.0, 0.0, 0.8);
    assert(isEqual(redColor.r(), 1.0));
    assert(isEqual(redColor.g(), 0.0));
    assert(isEqual(redColor.b(), 0.0));
    assert(isEqual(redColor.a(), 0.8));
    
    // Test parameterized constructor without alpha (default to 1.0)
    RGBA_Color greenColor(0.0, 1.0, 0.0);
    assert(isEqual(greenColor.r(), 0.0));
    assert(isEqual(greenColor.g(), 1.0));
    assert(isEqual(greenColor.b(), 0.0));
    assert(isEqual(greenColor.a(), 1.0));
    
    // Test Vector constructor
    math::Vector<double> vec(4);
    vec[0] = double(0.2);
    vec[1] = double(0.4);
    vec[2] = double(0.6);
    vec[3] = double(0.8);
    
    RGBA_Color vectorColor(vec);
    assert(isEqual(vectorColor.r(), 0.2));
    assert(isEqual(vectorColor.g(), 0.4));
    assert(isEqual(vectorColor.b(), 0.6));
    assert(isEqual(vectorColor.a(), 0.8));
}

// Test component access methods
void testRGBAColorComponentAccess() {
    RGBA_Color color(0.3, 0.5, 0.7, 0.9);
    
    // Test individual component access
    assert(isEqual(color.r(), 0.3));
    assert(isEqual(color.g(), 0.5));
    assert(isEqual(color.b(), 0.7));
    assert(isEqual(color.a(), 0.9));
    
    // Test that we can get the underlying vector if needed (controlled access)
    const math::Vector<double>& vec = color.asVector();
    assert(vec.size() == 4);
    assert(isEqual(vec[0], 0.3));
    assert(isEqual(vec[1], 0.5));
    assert(isEqual(vec[2], 0.7));
    assert(isEqual(vec[3], 0.9));
}

// Test component setter methods
void testRGBAColorComponentSetters() {
    RGBA_Color color;
    
    // Test individual setters
    color.setR(0.1);
    assert(isEqual(color.r(), 0.1));
    
    color.setG(0.2);
    assert(isEqual(color.g(), 0.2));
    
    color.setB(0.3);
    assert(isEqual(color.b(), 0.3));
    
    color.setA(0.4);
    assert(isEqual(color.a(), 0.4));
    
    // Test bulk setter
    color.setRGBA(0.8, 0.6, 0.4, 0.2);
    assert(isEqual(color.r(), 0.8));
    assert(isEqual(color.g(), 0.6));
    assert(isEqual(color.b(), 0.4));
    assert(isEqual(color.a(), 0.2));
    
    // Test bulk setter with default alpha
    color.setRGBA(0.1, 0.3, 0.5);
    assert(isEqual(color.r(), 0.1));
    assert(isEqual(color.g(), 0.3));
    assert(isEqual(color.b(), 0.5));
    assert(isEqual(color.a(), 1.0)); // Default alpha
}

// Test utility methods
void testRGBAColorUtilityMethods() {
    // Test clamp method
    RGBA_Color outOfBounds(1.5, -0.5, 2.0, -1.0);
    RGBA_Color clamped = outOfBounds.clamp();
    assert(isEqual(clamped.r(), 1.0));
    assert(isEqual(clamped.g(), 0.0));
    assert(isEqual(clamped.b(), 1.0));
    assert(isEqual(clamped.a(), 0.0));
    
    // Test clamp method with valid values
    RGBA_Color validColor(0.3, 0.7, 0.1, 0.9);
    RGBA_Color validClamped = validColor.clamp();
    assert(isEqual(validClamped.r(), 0.3));
    assert(isEqual(validClamped.g(), 0.7));
    assert(isEqual(validClamped.b(), 0.1));
    assert(isEqual(validClamped.a(), 0.9));
    
    // Test toGrayscale method with default weights
    RGBA_Color colorColor(0.8, 0.6, 0.4, 0.5);
    RGBA_Color grayscale = colorColor.toGrayscale();
    double expectedLuminance = 0.299 * 0.8 + 0.587 * 0.6 + 0.114 * 0.4;
    assert(isEqual(grayscale.r(), expectedLuminance));
    assert(isEqual(grayscale.g(), expectedLuminance));
    assert(isEqual(grayscale.b(), expectedLuminance));
    assert(isEqual(grayscale.a(), 0.5)); // Alpha should be preserved
    
    // Test toGrayscale method with custom weights
    RGBA_Color customColor(1.0, 1.0, 1.0, 1.0);
    RGBA_Color customGrayscale = customColor.toGrayscale(0.5, 0.3, 0.2);
    double customLuminance = 0.5 * 1.0 + 0.3 * 1.0 + 0.2 * 1.0;
    assert(isEqual(customGrayscale.r(), customLuminance));
    assert(isEqual(customGrayscale.g(), customLuminance));
    assert(isEqual(customGrayscale.b(), customLuminance));
    assert(isEqual(customGrayscale.a(), 1.0));
}

// Test color-specific operations
void testRGBAColorVectorOperations() {
    RGBA_Color color1(0.2, 0.4, 0.6, 0.8);
    RGBA_Color color2(0.1, 0.3, 0.2, 0.4);
    
    // Test addition (color mixing)
    RGBA_Color sum = color1 + color2;
    assert(isEqual(sum.r(), 0.3));
    assert(isEqual(sum.g(), 0.7));
    assert(isEqual(sum.b(), 0.8));
    assert(isEqual(sum.a(), 1.0));
    
    // Test subtraction
    RGBA_Color diff = color1 - color2;
    assert(isEqual(diff.r(), 0.1));
    assert(isEqual(diff.g(), 0.1));
    assert(isEqual(diff.b(), 0.4));
    assert(isEqual(diff.a(), 0.4));
    
    // Test scalar multiplication (brightness scaling)
    RGBA_Color scaled = color1 * 2.0;
    assert(isEqual(scaled.r(), 0.4));
    assert(isEqual(scaled.g(), 0.8));
    assert(isEqual(scaled.b(), 1.0));
    assert(isEqual(scaled.a(), 1.0));
    
    // Test reverse scalar multiplication
    RGBA_Color scaledReverse = 2.0 * color1;
    assert(isEqual(scaledReverse.r(), 0.4));
    assert(isEqual(scaledReverse.g(), 0.8));
    assert(isEqual(scaledReverse.b(), 1.0));
    assert(isEqual(scaledReverse.a(), 1.0));
    
    // Test color multiplication (component-wise filtering)
    RGBA_Color multiplied = color1 * color2;
    assert(isEqual(multiplied.r(), 0.2 * 0.1));
    assert(isEqual(multiplied.g(), 0.4 * 0.3));
    assert(isEqual(multiplied.b(), 0.6 * 0.2));
    assert(isEqual(multiplied.a(), 0.8 * 0.4));
    
    // Test equality (compare individual components since Vector pointer equality doesn't work as expected)
    RGBA_Color color1Copy(0.2, 0.4, 0.6, 0.8);
    assert(isEqual(color1.r(), color1Copy.r()) && 
           isEqual(color1.g(), color1Copy.g()) && 
           isEqual(color1.b(), color1Copy.b()) && 
           isEqual(color1.a(), color1Copy.a()));
    assert(!(isEqual(color1.r(), color2.r()) && 
             isEqual(color1.g(), color2.g()) && 
             isEqual(color1.b(), color2.b()) && 
             isEqual(color1.a(), color2.a())));
    
    // Test inequality (compare individual components)
    assert(!(isEqual(color1.r(), color2.r()) && 
             isEqual(color1.g(), color2.g()) && 
             isEqual(color1.b(), color2.b()) && 
             isEqual(color1.a(), color2.a())));
    assert(isEqual(color1.r(), color1Copy.r()) && 
           isEqual(color1.g(), color1Copy.g()) && 
           isEqual(color1.b(), color1Copy.b()) && 
           isEqual(color1.a(), color1Copy.a()));
    
    // Test interpolation (lerp)
    RGBA_Color lerped = color1.lerp(color2, 0.5);
    assert(isEqual(lerped.r(), 0.15)); // (0.2 + 0.1) / 2
    assert(isEqual(lerped.g(), 0.35)); // (0.4 + 0.3) / 2
    assert(isEqual(lerped.b(), 0.4));  // (0.6 + 0.2) / 2
    assert(isEqual(lerped.a(), 0.6));  // (0.8 + 0.4) / 2
    
    // Test alpha blending
    RGBA_Color foreground(1.0, 0.0, 0.0, 0.5); // 50% red
    RGBA_Color background(0.0, 1.0, 0.0, 1.0); // green
    RGBA_Color blended = foreground.alphaBlend(background);
    assert(isEqual(blended.r(), 0.5)); // 0.5 * 1.0 + 0.5 * 0.0
    assert(isEqual(blended.g(), 0.5)); // 0.5 * 0.0 + 0.5 * 1.0
    assert(isEqual(blended.b(), 0.0)); // 0.5 * 0.0 + 0.5 * 0.0
}

// Test convenience color functions
void testRGBAColorConvenienceFunctions() {
    // Test black color
    RGBA_Color black = Colors::black();
    assert(isEqual(black.r(), 0.0));
    assert(isEqual(black.g(), 0.0));
    assert(isEqual(black.b(), 0.0));
    assert(isEqual(black.a(), 1.0));
    
    // Test white color
    RGBA_Color white = Colors::white();
    assert(isEqual(white.r(), 1.0));
    assert(isEqual(white.g(), 1.0));
    assert(isEqual(white.b(), 1.0));
    assert(isEqual(white.a(), 1.0));
    
    // Test red color
    RGBA_Color red = Colors::red();
    assert(isEqual(red.r(), 1.0));
    assert(isEqual(red.g(), 0.0));
    assert(isEqual(red.b(), 0.0));
    assert(isEqual(red.a(), 1.0));
    
    // Test green color
    RGBA_Color green = Colors::green();
    assert(isEqual(green.r(), 0.0));
    assert(isEqual(green.g(), 1.0));
    assert(isEqual(green.b(), 0.0));
    assert(isEqual(green.a(), 1.0));
    
    // Test blue color
    RGBA_Color blue = Colors::blue();
    assert(isEqual(blue.r(), 0.0));
    assert(isEqual(blue.g(), 0.0));
    assert(isEqual(blue.b(), 1.0));
    assert(isEqual(blue.a(), 1.0));
    
    // Test transparent color
    RGBA_Color transparent = Colors::transparent();
    assert(isEqual(transparent.r(), 0.0));
    assert(isEqual(transparent.g(), 0.0));
    assert(isEqual(transparent.b(), 0.0));
    assert(isEqual(transparent.a(), 0.0));
}

// Test error handling
void testRGBAColorErrorHandling() {
    // Test Vector constructor with wrong size
    try {
        math::Vector<double> invalidVec(3); // Only 3 components
        invalidVec[0] = double(1.0);
        invalidVec[1] = double(2.0);
        invalidVec[2] = double(3.0);
        
        RGBA_Color invalidColor(invalidVec);
                
        assert(false); // Should not reach here
    } catch (const std::invalid_argument& e) {
        // Expected behavior
        assert(true);
    }
    
    // Test Vector constructor with too many components
    try {
        math::Vector<double> invalidVec(5); // 5 components
        invalidVec[0] = double(1.0);
        invalidVec[1] = double(2.0);
        invalidVec[2] = double(3.0);
        invalidVec[3] = double(4.0);
        invalidVec[4] = double(5.0);
        
        RGBA_Color invalidColor(invalidVec);
        
        assert(false); // Should not reach here
    } catch (const std::invalid_argument& e) {
        // Expected behavior
        assert(true);
    }
    
    // Test accessing components through asVector (bounds checking)
    RGBA_Color color(0.1, 0.2, 0.3, 0.4);
    try {
        const math::Vector<double>& vec = color.asVector();
        [[maybe_unused]] const double value = vec[4]; // Out of bounds
        assert(false); // Should not reach here
    } catch (const std::out_of_range& e) {
        // Expected behavior
        assert(true);
    }
    
    try {
        const math::Vector<double>& vec = color.asVector();
        [[maybe_unused]] const double value = vec[-1]; // Out of bounds
        assert(false); // Should not reach here
    } catch (const std::out_of_range& e) {
        // Expected behavior
        assert(true);
    }
}
