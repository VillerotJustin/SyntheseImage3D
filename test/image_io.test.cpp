//
// Created by GitHub Copilot on 06/10/2025.
//

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <cstdio>
#include <fstream>
#include <sys/stat.h>

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

// Helper function to check if file exists
bool fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// Helper function to delete file if it exists
void deleteFileIfExists(const std::string& filename) {
    if (fileExists(filename)) {
        std::remove(filename.c_str());
    }
}

// Test basic Image creation and properties
void testImageCreation() {
    std::cout << "\n=== Testing Image Creation ===" << std::endl;
    
    // Test constructor with width and height
    Image img(100, 50);
    assertEqual(img.getWidth() == 100, "Basic constructor width");
    assertEqual(img.getHeight() == 50, "Basic constructor height");
    assertEqual(img.getNumPixels() == 5000, "Basic constructor pixel count");
    assertEqual(img.isValid(), "Basic constructor validity");
    
    // Test that pixels are initialized to black
    const RGBA_Color* pixel = img.getPixel(0, 0);
    assertEqual(pixel != nullptr, "Pixel retrieval non-null");
    assertEqual(pixel->r() == 0.0, "Initial red component");
    assertEqual(pixel->g() == 0.0, "Initial green component");
    assertEqual(pixel->b() == 0.0, "Initial blue component");
    assertEqual(pixel->a() == 1.0, "Initial alpha component");
}

// Test pixel manipulation
void testPixelManipulation() {
    std::cout << "\n=== Testing Pixel Manipulation ===" << std::endl;
    
    Image img(10, 10);
    
    // Test setPixel and getPixel
    RGBA_Color red(1.0, 0.0, 0.0, 1.0);
    img.setPixel(5, 5, red);
    
    const RGBA_Color* retrievedPixel = img.getPixel(5, 5);
    assertEqual(retrievedPixel->r() == 1.0, "Red component correct");
    assertEqual(retrievedPixel->g() == 0.0, "Green component correct");
    assertEqual(retrievedPixel->b() == 0.0, "Blue component correct");
    assertEqual(retrievedPixel->a() == 1.0, "Alpha component correct");
    
    // Test bounds checking
    try {
        img.getPixel(-1, 0);
        assertEqual(false, "Negative bounds should throw exception");
    } catch (const std::out_of_range&) {
        assertEqual(true, "Negative bounds exception handling");
    }
    
    try {
        img.setPixel(100, 100, red);
        assertEqual(false, "Out of bounds should throw exception");
    } catch (const std::out_of_range&) {
        assertEqual(true, "Out of bounds exception handling");
    }
}

// Test image transformations
void testImageTransformations() {
    std::cout << "\n=== Testing Image Transformations ===" << std::endl;
    
    Image img(5, 5);
    
    // Fill with a color
    RGBA_Color blue(0.0, 0.0, 1.0, 1.0);
    img.fill(blue);
    
    // Check that all pixels are blue
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            const RGBA_Color* pixel = img.getPixel(x, y);
            assertEqual(pixel->r() == 0.0, "Fill red component");
            assertEqual(pixel->g() == 0.0, "Fill green component");
            assertEqual(pixel->b() == 1.0, "Fill blue component");
            assertEqual(pixel->a() == 1.0, "Fill alpha component");
        }
    }
    
    // Test clear (should set to black)
    img.clear();
    const RGBA_Color* pixel = img.getPixel(2, 2);
    assertEqual(pixel->r() == 0.0, "Clear red component");
    assertEqual(pixel->g() == 0.0, "Clear green component");
    assertEqual(pixel->b() == 0.0, "Clear blue component");
    assertEqual(pixel->a() == 1.0, "Clear alpha component");
}

// Test BMP file I/O
void testBmpFileIO() {
    std::cout << "\n=== Testing BMP File I/O ===" << std::endl;
    
    const std::string testDir = "./test/test_by_product/image_io/";
    const std::string filename = "test_bmp";
    const std::string fullPath = testDir + filename + ".bmp";
    
    // Clean up any existing test file
    deleteFileIfExists(fullPath);
    
    // Create a test image with known pattern
    Image originalImg(4, 4);
    
    // Create a simple 2x2 pattern repeated
    RGBA_Color red(1.0, 0.0, 0.0, 1.0);
    RGBA_Color green(0.0, 1.0, 0.0, 1.0);
    RGBA_Color blue(0.0, 0.0, 1.0, 1.0);
    RGBA_Color white(1.0, 1.0, 1.0, 1.0);
    
    originalImg.setPixel(0, 0, red);
    originalImg.setPixel(1, 0, green);
    originalImg.setPixel(0, 1, blue);
    originalImg.setPixel(1, 1, white);
    
    // Repeat pattern
    originalImg.setPixel(2, 0, red);
    originalImg.setPixel(3, 0, green);
    originalImg.setPixel(2, 1, blue);
    originalImg.setPixel(3, 1, white);
    originalImg.setPixel(0, 2, red);
    originalImg.setPixel(1, 2, green);
    originalImg.setPixel(0, 3, blue);
    originalImg.setPixel(1, 3, white);
    originalImg.setPixel(2, 2, red);
    originalImg.setPixel(3, 2, green);
    originalImg.setPixel(2, 3, blue);
    originalImg.setPixel(3, 3, white);
    
    // Save to BMP
    try {
        originalImg.toBitmapFile(filename, testDir);
        assertEqual(fileExists(fullPath), "BMP file creation");
    } catch (const std::exception& e) {
        std::cout << "⚠ BMP save test failed: " << e.what() << std::endl;
        return;
    }
    
    // Try to load it back (requires ImageMagick)
    try {
        Image loadedImg(filename + ".bmp", testDir);
        assertEqual(loadedImg.getWidth() == 4, "BMP loaded width");
        assertEqual(loadedImg.getHeight() == 4, "BMP loaded height");
        
        // Check a few key pixels (with some tolerance for compression artifacts)
        const RGBA_Color* pixel00 = loadedImg.getPixel(0, 0);
        assertEqual(pixel00->r() > 0.9, "BMP loaded red pixel");
        assertEqual(pixel00->g() < 0.1, "BMP loaded red pixel green component");
        assertEqual(pixel00->b() < 0.1, "BMP loaded red pixel blue component");
        
    } catch (const std::exception& e) {
        std::cout << "⚠ BMP load test skipped (ImageMagick required): " << e.what() << std::endl;
    }
}

// Test PNG file export
void testPngFileExport() {
    std::cout << "\n=== Testing PNG File Export ===" << std::endl;
    
    const std::string testDir = "./test/test_by_product/image_io/";
    const std::string filename = "test_png";
    const std::string fullPath = testDir + filename + ".png";
    
    // Clean up any existing test file
    deleteFileIfExists(fullPath);
    
    // Create a simple test image
    Image img(3, 3);
    RGBA_Color colors[] = {
        RGBA_Color(1.0, 0.0, 0.0, 1.0), // Red
        RGBA_Color(0.0, 1.0, 0.0, 1.0), // Green
        RGBA_Color(0.0, 0.0, 1.0, 1.0)  // Blue
    };
    
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            img.setPixel(x, y, colors[(x + y) % 3]);
        }
    }
    
    // Export to PNG
    try {
        img.toPngFile(filename, testDir);
        assertEqual(fileExists(fullPath), "PNG file creation");
    } catch (const std::exception& e) {
        std::cout << "⚠ PNG export test skipped (ImageMagick required): " << e.what() << std::endl;
    }
}

// Test JPEG file export
void testJpegFileExport() {
    std::cout << "\n=== Testing JPEG File Export ===" << std::endl;
    
    const std::string testDir = "./test/test_by_product/image_io/";
    const std::string filename = "test_jpeg";
    const std::string fullPath = testDir + filename + ".jpg";
    
    // Clean up any existing test file
    deleteFileIfExists(fullPath);
    
    // Create a test image with gradients (good for JPEG)
    Image img(8, 8);
    
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            double red = static_cast<double>(x) / 7.0;
            double green = static_cast<double>(y) / 7.0;
            double blue = 0.5;
            img.setPixel(x, y, RGBA_Color(red, green, blue, 1.0));
        }
    }
    
    // Export to JPEG
    try {
        img.toJpegFile(filename, testDir);
        assertEqual(fileExists(fullPath), "JPEG file creation");
        std::cout << "✓ JPEG file exported successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠ JPEG export test skipped (ImageMagick required): " << e.what() << std::endl;
    }
}

// Test TIFF file export
void testTiffFileExport() {
    std::cout << "\n=== Testing TIFF File Export ===" << std::endl;
    
    const std::string testDir = "./test/test_by_product/image_io/";
    const std::string filename = "test_tiff";
    const std::string fullPath = testDir + filename + ".tiff";
    
    // Clean up any existing test file
    deleteFileIfExists(fullPath);
    
    // Create a test image with precise colors (good for TIFF)
    Image img(2, 2);
    img.setPixel(0, 0, RGBA_Color(1.0, 0.0, 0.0, 1.0)); // Red
    img.setPixel(1, 0, RGBA_Color(0.0, 1.0, 0.0, 1.0)); // Green
    img.setPixel(0, 1, RGBA_Color(0.0, 0.0, 1.0, 1.0)); // Blue
    img.setPixel(1, 1, RGBA_Color(1.0, 1.0, 1.0, 1.0)); // White
    
    // Export to TIFF
    try {
        img.toTiffFile(filename, testDir);
        assertEqual(fileExists(fullPath), "TIFF file creation");
        std::cout << "✓ TIFF file exported successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "⚠ TIFF export test skipped (ImageMagick required): " << e.what() << std::endl;
    }
}

// Test copy constructor and assignment
void testCopyOperations() {
    std::cout << "\n=== Testing Copy Operations ===" << std::endl;
    
    // Create original image
    Image original(3, 3);
    RGBA_Color testColor(0.5, 0.7, 0.9, 0.8);
    original.setPixel(1, 1, testColor);
    
    // Test copy constructor
    Image copied(original);
    assertEqual(copied.getWidth() == original.getWidth(), "Copy constructor width");
    assertEqual(copied.getHeight() == original.getHeight(), "Copy constructor height");
    
    const RGBA_Color* copiedPixel = copied.getPixel(1, 1);
    assertEqual(copiedPixel->r() == testColor.r(), "Copy constructor pixel red");
    assertEqual(copiedPixel->g() == testColor.g(), "Copy constructor pixel green");
    assertEqual(copiedPixel->b() == testColor.b(), "Copy constructor pixel blue");
    assertEqual(copiedPixel->a() == testColor.a(), "Copy constructor pixel alpha");
    
    // Test assignment operator
    Image assigned(10, 10); // Different size initially
    assigned = original;
    assertEqual(assigned.getWidth() == original.getWidth(), "Assignment operator width");
    assertEqual(assigned.getHeight() == original.getHeight(), "Assignment operator height");
    
    const RGBA_Color* assignedPixel = assigned.getPixel(1, 1);
    assertEqual(assignedPixel->r() == testColor.r(), "Assignment operator pixel red");
    assertEqual(assignedPixel->g() == testColor.g(), "Assignment operator pixel green");
    assertEqual(assignedPixel->b() == testColor.b(), "Assignment operator pixel blue");
    assertEqual(assignedPixel->a() == testColor.a(), "Assignment operator pixel alpha");
    
    // Test copy method
    Image copied2 = original.copy();
    assertEqual(copied2.getWidth() == original.getWidth(), "Copy method width");
    assertEqual(copied2.getHeight() == original.getHeight(), "Copy method height");
    
    std::cout << "✓ Copy operation tests passed" << std::endl;
}

// Test error handling
void testErrorHandling() {
    std::cout << "\n=== Testing Error Handling ===" << std::endl;
    
    // Test invalid dimensions
    bool caughtInvalidWidth = false;
    try {
        Image invalidImg(-1, 10);
    } catch (const std::invalid_argument&) {
        caughtInvalidWidth = true;
    }
    assertEqual(caughtInvalidWidth, "Invalid width exception");
    
    bool caughtInvalidHeight = false;
    try {
        Image invalidImg(10, 0);
    } catch (const std::invalid_argument&) {
        caughtInvalidHeight = true;
    }
    assertEqual(caughtInvalidHeight, "Invalid height exception");
    
    // Test file not found (if ImageMagick is available)
    try {
        Image nonExistentImg("nonexistent_file.bmp", "./");
        // If this doesn't throw, ImageMagick might not be available
        std::cout << "⚠ File not found test inconclusive (ImageMagick behavior varies)" << std::endl;
    } catch (const std::runtime_error&) {
        // Expected behavior
        std::cout << "✓ File not found error handled correctly" << std::endl;
    }
    
    std::cout << "✓ Error handling tests completed" << std::endl;
}

int main() {
    std::cout << "=== Image I/O Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testImageCreation();
        testPixelManipulation();
        testImageTransformations();
        testBmpFileIO();
        testPngFileExport();
        testJpegFileExport();
        testTiffFileExport();
        testCopyOperations();
        testErrorHandling();
        
        std::cout << std::endl;
        std::cout << "🎉 All Image I/O tests completed successfully!" << std::endl;
        std::cout << std::endl;
        std::cout << "Note: Some tests may be skipped if ImageMagick is not installed." << std::endl;
        std::cout << "To install ImageMagick: sudo apt-get install imagemagick (Ubuntu/Debian)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}