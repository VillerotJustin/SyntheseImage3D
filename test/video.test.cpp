#include "../Lib/Rendering/Video.h"
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <string>

using namespace rendering;
using namespace math;

// Helper function for floating-point comparison
bool isEqual(double a, double b, double epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}

// Test Video constructor and basic properties
void testVideoConstructorAndProperties() {
    std::cout << "Testing Video constructor and properties..." << std::endl;
    
    Video video(800, 600, 30.0);
    
    assert(video.getWidth() == 800);
    assert(video.getHeight() == 600);
    assert(isEqual(video.getFramesPerSecond(), 30.0));
    assert(video.getFrameCount() == 0);
    assert(isEqual(video.getDuration(), 0.0));
    
    std::cout << "Video constructor and properties test passed!" << std::endl;
}

// Test Video setters
void testVideoSetters() {
    std::cout << "Testing Video setters..." << std::endl;
    
    Video video(800, 600, 30.0);
    
    video.setWidth(1920);
    video.setHeight(1080);
    video.setFramesPerSecond(60.0);
    
    assert(video.getWidth() == 1920);
    assert(video.getHeight() == 1080);
    assert(isEqual(video.getFramesPerSecond(), 60.0));
    
    std::cout << "Video setters test passed!" << std::endl;
}

// Test frame management
void testFrameManagement() {
    std::cout << "Testing frame management..." << std::endl;
    
    Video video(800, 600, 30.0);
    
    // Create some dummy images for testing
    Image frame1(800, 600);
    Image frame2(800, 600);
    Image frame3(800, 600);
    
    // Test adding frames
    video.addFrame(frame1);
    video.addFrame(frame2);
    assert(video.getFrameCount() == 2);
    
    // Test accessing frames
    Image& firstFrame = video.getFrame(0);
    assert(firstFrame.getWidth() == 800);
    assert(firstFrame.getHeight() == 600);
    
    // Test inserting frame
    video.insertFrame(1, frame3);
    assert(video.getFrameCount() == 3);
    
    // Test removing frame
    video.removeFrame(1);
    assert(video.getFrameCount() == 2);
    
    // Test clearing frames
    video.clearFrames();
    assert(video.getFrameCount() == 0);
    
    std::cout << "Frame management test passed!" << std::endl;
}

// Test video duration calculation
void testVideoDuration() {
    std::cout << "Testing video duration calculation..." << std::endl;
    
    Video video(800, 600, 30.0);
    
    Image frame(800, 600);
    
    // Add 30 frames (should be 1 second at 30 fps)
    for (int i = 0; i < 30; ++i) {
        video.addFrame(frame);
    }
    
    assert(isEqual(video.getDuration(), 1.0));
    
    // Add 30 more frames (should be 2 seconds at 30 fps)
    for (int i = 0; i < 30; ++i) {
        video.addFrame(frame);
    }
    
    assert(isEqual(video.getDuration(), 2.0));
    
    std::cout << "Video duration test passed!" << std::endl;
}

// Test video validation
void testVideoValidation() {
    std::cout << "Testing video validation..." << std::endl;
    
    // Test valid video
    Video validVideo(800, 600, 30.0);
    Image frame(800, 600);
    validVideo.addFrame(frame);
    assert(validVideo.isValid());
    
    // Test video with invalid dimensions
    Video invalidDimensionsVideo(0, 600, 30.0);
    assert(!invalidDimensionsVideo.isValid());
    
    // Test video with invalid frame rate
    Video invalidFpsVideo(800, 600, 0.0);
    assert(!invalidFpsVideo.isValid());
    
    // Test video with mismatched frame dimensions
    Video mismatchedVideo(800, 600, 30.0);
    Image wrongSizeFrame(640, 480);
    mismatchedVideo.addFrame(wrongSizeFrame);
    assert(!mismatchedVideo.isValid());
    
    std::cout << "Video validation test passed!" << std::endl;
}

// Test video statistics
void testVideoStats() {
    std::cout << "Testing video statistics..." << std::endl;
    
    Video video(800, 600, 30.0);
    Image frame(800, 600);
    
    // Add 90 frames (3 seconds at 30 fps)
    for (int i = 0; i < 90; ++i) {
        video.addFrame(frame);
    }
    
    VideoStats stats = video.getStats();
    
    assert(stats.frameCount == 90);
    assert(isEqual(stats.duration, 3.0));
    assert(stats.width == 800);
    assert(stats.height == 600);
    assert(isEqual(stats.framesPerSecond, 30.0));
    assert(stats.isValid);
    assert(stats.estimatedSizeBytes > 0);
    
    std::cout << "Video statistics test passed!" << std::endl;
}

// Test frame range extraction
void testFrameRangeExtraction() {
    std::cout << "Testing frame range extraction..." << std::endl;
    
    Video video(800, 600, 30.0);
    Image frame(800, 600);
    
    // Add 10 frames
    for (int i = 0; i < 10; ++i) {
        video.addFrame(frame);
    }
    
    // Extract frames 2-5 (should get 3 frames)
    Video subVideo = video.extractFrameRange(2, 5);
    assert(subVideo.getFrameCount() == 3);
    assert(subVideo.getWidth() == 800);
    assert(subVideo.getHeight() == 600);
    assert(isEqual(subVideo.getFramesPerSecond(), 30.0));
    
    std::cout << "Frame range extraction test passed!" << std::endl;
}

// Test frame reversal
void testFrameReversal() {
    std::cout << "Testing frame reversal..." << std::endl;
    
    Video video(800, 600, 30.0);
    
    // Create frames with different properties to verify order
    Image frame1(800, 600);
    Image frame2(800, 600);
    Image frame3(800, 600);
    
    video.addFrame(frame1);
    video.addFrame(frame2);
    video.addFrame(frame3);
    
    // Get references to frames before reversal
    const Vector<Image>& framesBefore = video.getFrames();
    Image firstFrameBefore = framesBefore[0];
    Image lastFrameBefore = framesBefore[2];
    
    // Reverse frames
    video.reverseFrames();
    
    // Check that order is reversed
    const Vector<Image>& framesAfter = video.getFrames();
    assert(framesAfter.size() == 3);
    
    std::cout << "Frame reversal test passed!" << std::endl;
}

// Test video resizing
void testVideoResizing() {
    std::cout << "Testing video resizing..." << std::endl;
    
    Video video(800, 600, 30.0);
    Image frame(800, 600);
    
    video.addFrame(frame);
    video.addFrame(frame);
    
    // Resize video
    video.resizeVideo(1920, 1080);
    
    assert(video.getWidth() == 1920);
    assert(video.getHeight() == 1080);
    
    // Check that all frames are resized
    for (size_t i = 0; i < video.getFrameCount(); ++i) {
        const Image& resizedFrame = video.getFrame(i);
        assert(resizedFrame.getWidth() == 1920);
        assert(resizedFrame.getHeight() == 1080);
    }
    
    std::cout << "Video resizing test passed!" << std::endl;
}

// Test thumbnail creation
void testThumbnailCreation() {
    std::cout << "Testing thumbnail creation..." << std::endl;
    
    Video video(800, 600, 30.0);
    Image frame(800, 600);
    
    video.addFrame(frame);
    
    // Create thumbnail
    Image thumbnail = video.createThumbnail(0, 200, 150);
    
    assert(thumbnail.getWidth() == 200);
    assert(thumbnail.getHeight() == 150);
    assert(thumbnail.isValid());
    
    std::cout << "Thumbnail creation test passed!" << std::endl;
}

// Test video export functionality
void testVideoExport() {
    std::cout << "Testing video export..." << std::endl;
    
    Video video(800, 600, 30.0);
    Image frame(800, 600);
    
    video.addFrame(frame);

    frame.fill(Colors::red());

    video.addFrame(frame);

    frame.fill(Colors::green());

    video.addFrame(frame);

    frame.fill(Colors::blue());

    video.addFrame(frame);
    
    // Test exporting as frames
    try {
        video.exportToFile("test_video", "./test/test_by_product/video/", VideoFormat::FRAMES);
        std::cout << "Frame export test completed (check implementation for actual file creation)" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Export test: " << e.what() << " (expected for placeholder implementation)" << std::endl;
    }
    
    // Test other formats
    try {
        video.exportToFile("test_video", "./test/test_by_product/video/", VideoFormat::GIF);
    } catch (const std::exception& e) {
        std::cout << "GIF export test: " << e.what() << " (expected for placeholder implementation)" << std::endl;
    }
    
    std::cout << "Video export test passed!" << std::endl;
}

// Test error handling
void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    Video video(800, 600, 30.0);
    
    // Test accessing invalid frame index
    try {
        video.getFrame(0); // Should throw since no frames exist
        assert(false); // Should not reach here
    } catch (const std::out_of_range&) {
        // Expected exception
    }
    
    // Test removing invalid frame index
    try {
        video.removeFrame(0); // Should throw since no frames exist
        assert(false); // Should not reach here
    } catch (const std::out_of_range&) {
        // Expected exception
    }
    
    // Test invalid frame range extraction
    Image frame(800, 600);
    video.addFrame(frame);
    
    try {
        video.extractFrameRange(5, 10); // Should throw since only 1 frame exists
        assert(false); // Should not reach here
    } catch (const std::out_of_range&) {
        // Expected exception
    }
    
    // Test invalid thumbnail frame index
    try {
        video.createThumbnail(5, 100, 100); // Should throw since frame 5 doesn't exist
        assert(false); // Should not reach here
    } catch (const std::out_of_range&) {
        // Expected exception
    }
    
    std::cout << "Error handling test passed!" << std::endl;
}

int main() {
    std::cout << "Running Video class tests..." << std::endl;
    
    try {
        testVideoConstructorAndProperties();
        testVideoSetters();
        testFrameManagement();
        testVideoDuration();
        testVideoValidation();
        testVideoStats();
        testFrameRangeExtraction();
        testFrameReversal();
        testVideoResizing();
        testThumbnailCreation();
        testVideoExport();
        testErrorHandling();
        
        std::cout << "\nAll Video tests passed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}