//
// Created by villerot on 10/03/25.
//

#include "Video.h"
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <iostream>

namespace rendering {

double Video::getDuration() const {
    if (framesPerSecond <= 0) {
        return 0.0;
    }
    return static_cast<double>(frames.size()) / framesPerSecond;
}

void Video::insertFrame(int index, const Image& img) {
    if (index < 0 || index > static_cast<int>(frames.size())) {
        throw std::out_of_range("Frame index out of range");
    }
    frames.insert(index, new Image(img));
}

void Video::exportToFile(const std::string& filename, const std::string& filepath, VideoFormat format) const {
    if (!isValid()) {
        throw std::runtime_error("Cannot export invalid video");
    }
    
    if (frames.empty()) {
        throw std::runtime_error("Cannot export video with no frames");
    }
    
    // For now, implement a basic frame export (individual images)
    // In a real implementation, you would use a video encoding library like FFmpeg
    switch (format) {
        case VideoFormat::FRAMES:
            exportFrameSequence(filepath, filename);
            break;

        case VideoFormat::MKV:
            exportMKV(filename, filepath);
            break;

        case VideoFormat::MP4:
            exportMP4(filename, filepath);
            break;
    
        case VideoFormat::GIF:
            exportAsGif(filename, filepath);
            break;

        default:
            throw std::runtime_error("Unsupported video format");
            break;
    }
}

void Video::resizeVideo(int newWidth, int newHeight) {
    if (newWidth <= 0 || newHeight <= 0) {
        throw std::invalid_argument("New dimensions must be positive");
    }
    
    for (auto& frame : frames) {
        frame->resize(newWidth, newHeight);
    }
    
    width = newWidth;
    height = newHeight;
}

Video Video::extractFrameRange(int startFrame, int endFrame) const {
    if (startFrame < 0 || endFrame < 0 || startFrame >= static_cast<int>(frames.size()) || 
        endFrame > static_cast<int>(frames.size()) || startFrame >= endFrame) {
        throw std::out_of_range("Invalid frame range");
    }
    
    Video result(width, height, framesPerSecond);
    
    for (int i = startFrame; i < endFrame; ++i) {
        result.addFrame(*frames[i]);
    }
    
    return result;
}

void Video::reverseFrames() {
    std::reverse(frames.begin(), frames.end());
}

Image Video::createThumbnail(int frameIndex, int thumbnailWidth, int thumbnailHeight) const {
    if (frameIndex < 0 || frameIndex >= static_cast<int>(frames.size())) {
        throw std::out_of_range("Frame index out of range");
    }
    
    if (thumbnailWidth <= 0 || thumbnailHeight <= 0) {
        throw std::invalid_argument("Thumbnail dimensions must be positive");
    }

    Image thumbnail = *frames[frameIndex];
    thumbnail.resize(thumbnailWidth, thumbnailHeight);

    return thumbnail;
}

void Video::exportFrameSequence(const std::string& basePath, const std::string& baseFilename) const {
    // Extract directory and create frame files
    std::string directory = basePath.substr(0, basePath.find_last_of("/\\"));
    
    for (int i = 0; i < frames.size(); ++i) {
        std::string frameFilename = baseFilename + "_frame_" + std::to_string(i);
        
        try {
            frames[i]->toBitmapFile(frameFilename, basePath);
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to export frame " + std::to_string(i) + ": " + e.what());
        }
    }
    
    // Create a metadata file with video information
    std::string metadataFile = directory + "/" + baseFilename + "_metadata.txt";
    std::ofstream metadata(metadataFile);
    if (metadata.is_open()) {
        metadata << "Video Metadata\n";
        metadata << "==============\n";
        metadata << "Width: " << width << " pixels\n";
        metadata << "Height: " << height << " pixels\n";
        metadata << "Frame Rate: " << framesPerSecond << " fps\n";
        metadata << "Frame Count: " << frames.size() << "\n";
        metadata << "Duration: " << getDuration() << " seconds\n";
        metadata << "Format: PNG\n";
        metadata.close();
    }
}

void Video::exportMKV(const std::string& filename, const std::string& filepath) const {
    (void)filename; // Suppress unused parameter warning
    (void)filepath; // Suppress unused parameter warning
    // Placeholder for MKV export
    // In a real implementation, you would use a library like FFmpeg or similar
    throw std::runtime_error("MKV export not implemented yet. Use 'frames' format instead.");
}

void Video::exportMP4(const std::string& filename, const std::string& filepath) const {
    (void)filename; // Suppress unused parameter warning
    (void)filepath; // Suppress unused parameter warning
    // Placeholder for MP4 export
    // In a real implementation, you would use a library like FFmpeg or similar
    throw std::runtime_error("MP4 export not implemented yet. Use 'frames' format instead.");
}

void Video::exportAsGif(const std::string& filename, const std::string& filepath) const {
    (void)filename; // Suppress unused parameter warning
    (void)filepath; // Suppress unused parameter warning
    // Placeholder for GIF export
    // In a real implementation, you would use a library like giflib or similar
    throw std::runtime_error("GIF export not implemented yet. Use 'frames' format instead.");
}

// Utility method to validate frame dimensions
bool Video::validateFrameDimensions() const {
    for (const auto& frame : frames) {
        if (frame->getWidth() != width || frame->getHeight() != height) {
            return false;
        }
    }
    return true;
}

// Method to get video statistics
VideoStats Video::getStats() const {
    VideoStats stats;
    stats.frameCount = frames.size();
    stats.duration = getDuration();
    stats.width = width;
    stats.height = height;
    stats.framesPerSecond = framesPerSecond;
    stats.isValid = isValid();
    
    // Calculate file size estimation (assuming 24-bit RGB)
    stats.estimatedSizeBytes = static_cast<size_t>(width) * height * 3 * frames.size();
    
    return stats;
}

} // namespace rendering