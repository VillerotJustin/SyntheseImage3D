#include <iostream>
#include <stdexcept>
#include <cassert>
#include <vector>
#include <utility>
#include <string>
#include "../Lib/Rendering/Image.h"
#include "../Lib/Rendering/RGBA_Color.h"

using namespace rendering;

// Test helper function
void assertEqual(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << "✓ " << testName << " passed" << std::endl;
    } else {
        std::cout << "✗ " << testName << " FAILED" << std::endl;
        throw std::runtime_error("Test failed: " + testName);
    }
}

// Test Image constructors
void testImageConstructors() {
    std::cout << "\n=== Testing Image Constructors ===" << std::endl;
    
    // Test basic constructor
    Image img(100, 50);
    assertEqual(img.getWidth() == 100, "Basic constructor width");
    assertEqual(img.getHeight() == 50, "Basic constructor height");
    assertEqual(img.getNumPixels() == 5000, "Basic constructor pixel count");
    assertEqual(img.isValid(), "Basic constructor validity");
    
    // Test invalid dimensions
    try {
        Image invalidImg(-10, 20);
        assertEqual(false, "Invalid dimensions should throw exception");
    } catch (const std::invalid_argument&) {
        assertEqual(true, "Invalid dimensions exception handling");
    }
}

// Test pixel operations
void testPixelOperations() {
    std::cout << "\n=== Testing Pixel Operations ===" << std::endl;
    
    Image img(10, 10);
    
    // Test setting and getting pixels
    RGBA_Color red(1.0, 0.0, 0.0, 1.0);
    img.setPixel(5, 5, red);
    
    const RGBA_Color* pixel = img.getPixel(5, 5);
    assertEqual(pixel != nullptr, "Pixel retrieval non-null");
    assertEqual(pixel->r() == 1.0, "Red component correct");
    assertEqual(pixel->g() == 0.0, "Green component correct");
    assertEqual(pixel->b() == 0.0, "Blue component correct");
    assertEqual(pixel->a() == 1.0, "Alpha component correct");
    
    // Test out of bounds
    try {
        img.setPixel(15, 15, red);
        assertEqual(false, "Out of bounds should throw exception");
    } catch (const std::out_of_range&) {
        assertEqual(true, "Out of bounds exception handling");
    }
}

// Test image operations
void testImageOperations() {
    std::cout << "\n=== Testing Image Operations ===" << std::endl;
    
    Image img(20, 20);
    
    // Test fill
    RGBA_Color blue(0.0, 0.0, 1.0, 0.8);
    img.fill(blue);
    
    const RGBA_Color* corner = img.getPixel(0, 0);
    const RGBA_Color* center = img.getPixel(10, 10);
    assertEqual(corner->b() == 1.0, "Fill operation corner");
    assertEqual(center->b() == 1.0, "Fill operation center");
    assertEqual(center->a() == 0.8, "Fill operation alpha");
    
    // Test clear
    img.clear();
    const RGBA_Color* clearedPixel = img.getPixel(5, 5);
    assertEqual(clearedPixel->r() == 0.0 && clearedPixel->g() == 0.0 && 
                clearedPixel->b() == 0.0 && clearedPixel->a() == 1.0, "Clear operation");
}

// Create test pattern for bitmap output
Image createTestPattern(int width, int height) {
    Image img(width, height);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double r = static_cast<double>(x) / width;
            double g = static_cast<double>(y) / height;
            double b = 0.5;
            img.setPixel(x, y, RGBA_Color(r, g, b, 1.0));
        }
    }
    
    return img;
}

// Create checkerboard pattern
Image createCheckerboard(int width, int height, int squareSize) {
    Image img(width, height);
    
    RGBA_Color white(1.0, 1.0, 1.0, 1.0);
    RGBA_Color black(0.0, 0.0, 0.0, 1.0);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            bool isWhite = ((x / squareSize) + (y / squareSize)) % 2 == 0;
            img.setPixel(x, y, isWhite ? white : black);
        }
    }
    
    return img;
}

// Create color gradient
Image createColorGradient(int width, int height) {
    Image img(width, height);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double t = static_cast<double>(x) / (width - 1);
            RGBA_Color color;
            
            if (t < 0.33) {
                // Red to Green
                double localT = t / 0.33;
                color = RGBA_Color(1.0 - localT, localT, 0.0, 1.0);
            } else if (t < 0.66) {
                // Green to Blue
                double localT = (t - 0.33) / 0.33;
                color = RGBA_Color(0.0, 1.0 - localT, localT, 1.0);
            } else {
                // Blue to Red
                double localT = (t - 0.66) / 0.34;
                color = RGBA_Color(localT, 0.0, 1.0 - localT, 1.0);
            }
            
            img.setPixel(x, y, color);
        }
    }
    
    return img;
}

// Test bitmap output functionality
void testBitmapOutput() {
    std::cout << "\n=== Testing Bitmap Output ===" << std::endl;
    
    const std::string outputPath = "test/test_by_product/";
    
    try {
        // Test 1: Basic gradient pattern
        std::cout << "Creating gradient test pattern..." << std::endl;
        Image gradientImg = createTestPattern(200, 150);
        gradientImg.toBitmapFile("test_gradient", outputPath);
        assertEqual(true, "Gradient bitmap creation");
        
        // Test 2: Checkerboard pattern
        std::cout << "Creating checkerboard pattern..." << std::endl;
        Image checkerImg = createCheckerboard(160, 160, 20);
        checkerImg.toBitmapFile("test_checkerboard", outputPath);
        assertEqual(true, "Checkerboard bitmap creation");
        
        // Test 3: Color gradient
        std::cout << "Creating color gradient..." << std::endl;
        Image colorGradient = createColorGradient(300, 100);
        colorGradient.toBitmapFile("test_color_gradient", outputPath);
        assertEqual(true, "Color gradient bitmap creation");
        
        // Test 4: Small image
        std::cout << "Creating small test image..." << std::endl;
        Image smallImg(50, 50);
        smallImg.fill(RGBA_Color(0.8, 0.2, 0.6, 1.0)); // Purple
        smallImg.toBitmapFile("test_small", outputPath);
        assertEqual(true, "Small image bitmap creation");
        
        // Test 5: Image transformations
        std::cout << "Testing image transformations..." << std::endl;
        Image transformImg = createColorGradient(120, 80);
        
        // Save original
        transformImg.toBitmapFile("test_original", outputPath);
        
        // Test grayscale conversion
        Image grayscaleImg = transformImg.copy();
        grayscaleImg.toGrayscale();
        grayscaleImg.toBitmapFile("test_grayscale", outputPath);
        assertEqual(true, "Grayscale conversion bitmap");
        
        // Test black and white conversion
        Image bwImg = transformImg.copy();
        bwImg.toBlackAndWhite(0.5);
        bwImg.toBitmapFile("test_blackwhite", outputPath);
        assertEqual(true, "Black and white conversion bitmap");
        
        // Test color inversion
        Image invertedImg = transformImg.copy();
        invertedImg.invertColors();
        invertedImg.toBitmapFile("test_inverted", outputPath);
        assertEqual(true, "Color inversion bitmap");
        
        // Test 6: Different sizes
        std::cout << "Testing various image sizes..." << std::endl;
        std::vector<std::pair<int, int>> sizes = {{10, 10}, {64, 64}, {100, 200}, {256, 128}};
        
        for (size_t i = 0; i < sizes.size(); ++i) {
            int w = sizes[i].first;
            int h = sizes[i].second;
            
            Image sizeTestImg = createTestPattern(w, h);
            std::string filename = "test_size_" + std::to_string(w) + "x" + std::to_string(h) + "";
            sizeTestImg.toBitmapFile(filename, outputPath);
        }
        assertEqual(true, "Various size bitmap creation");
        
    } catch (const std::exception& e) {
        std::cout << "Bitmap output error: " << e.what() << std::endl;
        assertEqual(false, "Bitmap output operations");
    }
}

// Test copy operations
void testCopyOperations() {
    std::cout << "\n=== Testing Copy Operations ===" << std::endl;
    
    Image original = createCheckerboard(40, 40, 5);
    
    // Test copy constructor
    Image copied = original.copy();
    assertEqual(copied.getWidth() == original.getWidth(), "Copy width");
    assertEqual(copied.getHeight() == original.getHeight(), "Copy height");
    
    // Verify pixels are the same
    const RGBA_Color* origPixel = original.getPixel(10, 10);
    const RGBA_Color* copyPixel = copied.getPixel(10, 10);
    assertEqual(origPixel->r() == copyPixel->r(), "Copy pixel data");
    
    // Test that modifying copy doesn't affect original
    copied.setPixel(10, 10, RGBA_Color(1.0, 0.0, 1.0, 1.0));
    const RGBA_Color* newOrigPixel = original.getPixel(10, 10);
    assertEqual(origPixel->r() == newOrigPixel->r(), "Copy independence");
}

int main() {
    std::cout << "=== Image Class Test Suite ===" << std::endl;
    
    try {
        testImageConstructors();
        testPixelOperations();
        testImageOperations();
        testCopyOperations();
        testBitmapOutput();
        
        std::cout << "\n🎉 All Image tests passed successfully!" << std::endl;
        std::cout << "\n📁 Test output files saved to: test/test_by_product/" << std::endl;
        std::cout << "   - test_gradient (gradient pattern)" << std::endl;
        std::cout << "   - test_checkerboard (checkerboard pattern)" << std::endl;
        std::cout << "   - test_color_gradient (RGB color gradient)" << std::endl;
        std::cout << "   - test_small (small purple image)" << std::endl;
        std::cout << "   - test_original (original color image)" << std::endl;
        std::cout << "   - test_grayscale (grayscale conversion)" << std::endl;
        std::cout << "   - test_blackwhite (black & white threshold)" << std::endl;
        std::cout << "   - test_inverted (color inversion)" << std::endl;
        std::cout << "   - test_size_* (various size tests)" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "❌ Test suite failed: " << e.what() << std::endl;
        return 1;
    }
}
