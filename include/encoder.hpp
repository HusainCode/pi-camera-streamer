#pragma once
/**
 * @file encoder.hpp
 * @brief Real-time video encoder interface for pi-camera-streamer.
 *
 * This module handles frame compression using either MJPEG or H.264 codecs.
 * Designed for multithreaded, low-latency encoding on Raspberry Pi 5.
 */

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "buffer.hpp"  // SharedFrameBuffer, FrameData

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace pcs { // pi-camera-streamer namespace

enum class CodecType {
    MJPEG,
    H264
};

struct EncoderConfig {
    CodecType codec{CodecType::H264};
    int width{1280};
    int height{720};
    int fps{30};
    int bitrate{4000000}; // bits per second
    std::string hw_accel{"auto"}; // "v4l2m2m", "omx", or "auto"
};

/**
 * @brief Encoded frame container for transmission.
 */
struct EncodedFrame {
    std::vector<uint8_t> data;
    int64_t pts{0};
    int64_t dts{0};
    bool keyframe{false};
};

/**
 * @class Encoder
 * @brief Wraps FFmpeg/libav encoder setup and frame compression pipeline.
 *
 * Thread-safe encode interface for use in a producer-consumer pipeline.
 */
class Encoder {
public:
    explicit Encoder(const EncoderConfig& config);
    ~Encoder();

    /**
     * @brief Initialize the encoder (select codec, allocate context, etc.).
     */
    bool init();

    /**
     * @brief Encode a raw frame to the chosen codec.
     * @param frame Input frame (YUV420, RGB24, etc.)
     * @return EncodedFrame if successful, std::nullopt otherwise.
     */
    std::optional<EncodedFrame> encode(const FrameData& frame);

    /**
     * @brief Flush any remaining frames (for H.264 GOP completion).
     */
    void flush(std::vector<EncodedFrame>& outFrames);

    /**
     * @brief Release all allocated resources.
     */
    void close();

private:
    EncoderConfig config_;
    const AVCodec* codec_{nullptr};
    AVCodecContext* ctx_{nullptr};
    AVFrame* avFrame_{nullptr};
    AVPacket* avPacket_{nullptr};
    SwsContext* swsCtx_{nullptr};

    int frameIndex_{0};
    std::mutex mtx_;

    bool configure_codec();
    void setup_frame_buffer();
    bool convert_to_yuv(const FrameData& src);
};

} // namespace pcs
