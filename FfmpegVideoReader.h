#ifndef FFMPEG_VIDEO_READER_H
#define FFMPEG_VIDEO_READER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <string>
#include <vector>

struct VideoFrame {
    int width;
    int height;
    int pts;
    std::vector<uint8_t> rgbData; // RGB data, size = width * height * 3
};

class FfmpegVideoReader {
public:
    FfmpegVideoReader();
    ~FfmpegVideoReader();

    bool open(const std::string& filename);
    VideoFrame* readFrame();
    bool reopen();
    void close();

private:
    AVFormatContext* fmt_ctx;
    AVCodecContext* codec_ctx;
    const AVCodec* codec; // Sửa kiểu này!
    SwsContext* sws_ctx;
    int video_stream_index;
    AVPacket* pkt;
    AVFrame* frame;
    AVFrame* rgb_frame;
    uint8_t* rgb_buffer;
    int rgb_buffer_size;
    bool eof;

    std::string mVideoPath;
};

#endif // FFMPEG_VIDEO_READER_H
