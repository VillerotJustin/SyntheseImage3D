#ifndef VIDEO_H
#define VIDEO_H

#include "../Image.h"
#include "../Math/Vector.hpp"
#include <string>

namespace rendering {
    /**
     * @struct VideoStats
     * @brief Structure containing video statistics and metadata
     */
    struct VideoStats {
        size_t frameCount;           ///< Number of frames in the video
        double duration;             ///< Duration in seconds
        int width;                   ///< Width in pixels
        int height;                  ///< Height in pixels
        double framesPerSecond;      ///< Frame rate
        bool isValid;                ///< Whether the video is valid
        size_t estimatedSizeBytes;   ///< Estimated file size in bytes
    };

    /**
     * @class Video
     * @brief A class representing a video as a sequence of image frames
     * 
     * The Video class manages a collection of Image frames with consistent
     * dimensions and frame rate for video processing and export operations.
     */
    class Video {
    public:
        #pragma region Constructors

        /**
         * @brief Constructor for Video
         * @param w Width of the video frames in pixels
         * @param h Height of the video frames in pixels
         * @param fps Frames per second (frame rate) of the video
         */
        Video(int w, int h, double fps) : width(w), height(h), framesPerSecond(fps) {}

        #pragma endregion

        #pragma region Accessors
        
        /**
         * @brief Get the width of the video frames
         * @return int Width in pixels
         */
        int getWidth() const { return width; }
        
        /**
         * @brief Get the height of the video frames
         * @return int Height in pixels
         */
        int getHeight() const { return height; }
        
        /**
         * @brief Get the frame rate of the video
         * @return double Frames per second
         */
        double getFramesPerSecond() const { return framesPerSecond; }
        
        /**
         * @brief Get all frames in the video
         * @return const Vector<Image>& Reference to the frames vector
         */
        const Vector<Image>& getFrames() const { return frames; }
        
        /**
         * @brief Get a specific frame by index
         * @param index Index of the frame to retrieve
         * @return Image& Reference to the frame at the specified index
         * @throws std::out_of_range if index is invalid
         */
        Image& getFrame(int index) { return frames[index]; }
        
        /**
         * @brief Get the total number of frames in the video
         * @return size_t Number of frames
         */
        size_t getFrameCount() const { return frames.size(); }
        
        /**
         * @brief Get the total duration of the video in seconds
         * @return double Duration in seconds
         */
        double getDuration() const;

        /**
         * @brief Get comprehensive video statistics
         * @return VideoStats Structure containing video metadata
         */
        VideoStats getStats() const;

        #pragma endregion

        #pragma region Setters
        
        /**
         * @brief Set the width of the video frames
         * @param w New width in pixels
         */
        void setWidth(int w) { width = w; }
        
        /**
         * @brief Set the height of the video frames
         * @param h New height in pixels
         */
        void setHeight(int h) { height = h; }
        
        /**
         * @brief Set the frame rate of the video
         * @param fps New frames per second
         */
        void setFramesPerSecond(double fps) { framesPerSecond = fps; }
        
        /**
         * @brief Replace all frames with a new set of frames
         * @param f Vector of images to set as the new frames
         */
        void setFrames(const Vector<Image>& f) { frames = f; }
        
        /**
         * @brief Add a frame to the end of the video
         * @param img Image to add as a new frame
         */
        void addFrame(const Image& img) { frames.push_back(img); }
        
        /**
         * @brief Remove all frames from the video
         */
        void clearFrames() { frames.clear(); }

        /**
         * @brief Remove a specific frame by index
         * @param index Index of the frame to remove
         * @throws std::out_of_range if index is invalid
         */
        void removeFrame(int index) { frames.erase(frames.begin() + index); }

        /**
         * @brief Insert a frame at a specific position
         * @param index Position to insert the frame
         * @param img Image to insert
         * @throws std::out_of_range if index is invalid
         */
        void insertFrame(int index, const Image& img);

        #pragma endregion

        #pragma region Methods

        /**
         * @brief Check if the video is valid
         * @return bool True if video has valid dimensions, frame rate, and all frames match dimensions
         */
        bool isValid() const {
            if (width <= 0 || height <= 0 || framesPerSecond <= 0) {
                return false;
            }
            for (const auto& frame : frames) {
                if (frame.getWidth() != width || frame.getHeight() != height || !frame.isValid()) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Export the video to a file
         * @param filename Name of the output file (without extension)
         * @param filepath Directory path where the file should be saved
         * @param format Video format from VideoFormat enum
         * @throws std::runtime_error if export fails
         */
        void exportToFile(const std::string& filename, const std::string& filepath, VideoFormat format) const;

        /**
         * @brief Resize all frames in the video to new dimensions
         * @param newWidth New width for all frames
         * @param newHeight New height for all frames
         */
        void resizeVideo(int newWidth, int newHeight);

        /**
         * @brief Create a copy of the video with a subset of frames
         * @param startFrame Starting frame index (inclusive)
         * @param endFrame Ending frame index (exclusive)
         * @return Video New video containing the specified frame range
         * @throws std::out_of_range if frame indices are invalid
         */
        Video extractFrameRange(int startFrame, int endFrame) const;

        /**
         * @brief Reverse the order of all frames in the video
         */
        void reverseFrames();

        /**
         * @brief Create a thumbnail image from a specific frame
         * @param frameIndex Index of the frame to use for thumbnail
         * @param thumbnailWidth Width of the thumbnail
         * @param thumbnailHeight Height of the thumbnail
         * @return Image Thumbnail image
         * @throws std::out_of_range if frameIndex is invalid
         */
        Image createThumbnail(int frameIndex, int thumbnailWidth, int thumbnailHeight) const;

        /**
         * @brief Export video as a sequence of individual frame images
         * @param basePath Base file path for the export
         * @param baseFilename Base filename for the frames
         */
        void exportFrameSequence(const std::string& basePath, const std::string& baseFilename) const;

        /**
         * @brief Export video as a mkv file
         * @param basePath Base file path for the export
         * @param baseFilename Base filename for the frames
         */
        void exportMKV(const std::string& filename, const std::string& filepath) const;

        /**
         * @brief Export video as a mp4 file
         * @param basePath Base file path for the export
         * @param baseFilename Base filename for the frames
         */
        void exportMP4(const std::string& filename, const std::string& filepath) const;

        /**
         * @brief Export video as a gif file
         * @param basePath Base file path for the export
         * @param baseFilename Base filename for the frames
         */
        void exportAsGif(const std::string& filename, const std::string& filepath) const;

        #pragma endregion

    private:
        int width;                  ///< Width of video frames in pixels
        int height;                 ///< Height of video frames in pixels
        double framesPerSecond;     ///< Frame rate in frames per second
        Vector<Image> frames;       ///< Sequence of image frames

        /**
         * @brief Validate that all frames have consistent dimensions
         * @return bool True if all frames match the video dimensions
         */
        bool validateFrameDimensions() const;
    };

    /**
     * @enum VideoFormat
     * @brief Enumeration of supported video export formats
     */
    enum class VideoFormat {
        FRAMES,     ///< Export as individual frame images
        MKV,        ///< Export as MKV video file
        MP4,        ///< Export as MP4 video file
        GIF         ///< Export as animated GIF
    };
}

#endif // VIDEO_H