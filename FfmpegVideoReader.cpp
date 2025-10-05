#include "FfmpegVideoReader.h"
#include <iostream>
#include <cstring>

FfmpegVideoReader::FfmpegVideoReader()
    : fmt_ctx(nullptr),
    codec_ctx(nullptr),
    codec(nullptr),
    sws_ctx(nullptr),
    video_stream_index(-1),
    pkt(nullptr),
    frame(nullptr),
    rgb_frame(nullptr),
    rgb_buffer(nullptr),
    rgb_buffer_size(0),
    eof(false)
{
    // FFmpeg >= 4.x, 5.x, 8.x: KHÔNG cần av_register_all()
    // Xóa dòng này!
}

FfmpegVideoReader::~FfmpegVideoReader() {
    close();
}

bool FfmpegVideoReader::open(const std::string& filename) {
    mVideoPath = filename;
    if (avformat_open_input(&fmt_ctx, filename.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Không mở được file: " << filename << std::endl;
        return false;
    }
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        std::cerr << "Không lấy được thông tin stream.\n";
        avformat_close_input(&fmt_ctx);
        fmt_ctx = nullptr;
        return false;
    }

    video_stream_index = -1;
    AVCodecParameters* codecpar = nullptr;
    for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            codecpar = fmt_ctx->streams[i]->codecpar;
            break;
        }
    }
    if (video_stream_index == -1) {
        std::cerr << "Không tìm thấy stream video.\n";
        avformat_close_input(&fmt_ctx);
        fmt_ctx = nullptr;
        return false;
    }

    // Sửa kiểu const AVCodec*
    codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        std::cerr << "Không tìm thấy decoder phù hợp.\n";
        avformat_close_input(&fmt_ctx);
        fmt_ctx = nullptr;
        return false;
    }
    codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codecpar);

    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        std::cerr << "Không mở được codec.\n";
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        codec_ctx = nullptr;
        fmt_ctx = nullptr;
        return false;
    }

    sws_ctx = sws_getContext(
        codec_ctx->width,
        codec_ctx->height,
        codec_ctx->pix_fmt,
        codec_ctx->width,
        codec_ctx->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        nullptr, nullptr, nullptr
        );

    frame = av_frame_alloc();
    rgb_frame = av_frame_alloc();
    rgb_buffer_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 1);
    rgb_buffer = (uint8_t*)av_malloc(rgb_buffer_size * sizeof(uint8_t));
    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, rgb_buffer, AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 1);

    pkt = av_packet_alloc();
    eof = false;

    return true;
}

VideoFrame* FfmpegVideoReader::readFrame() {
    if (!fmt_ctx || !codec_ctx || eof) return nullptr;

    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_stream_index) {
            if (avcodec_send_packet(codec_ctx, pkt) == 0) {
                while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    sws_scale(sws_ctx, frame->data, frame->linesize, 0, codec_ctx->height,
                              rgb_frame->data, rgb_frame->linesize);

                    VideoFrame* vf = new VideoFrame();
                    vf->width = codec_ctx->width;
                    vf->height = codec_ctx->height;
                    vf->pts = frame->pts;
                    vf->rgbData.resize(rgb_buffer_size);
                    std::memcpy(vf->rgbData.data(), rgb_frame->data[0], rgb_buffer_size);

                    av_packet_unref(pkt);
                    return vf;
                }
            }
        }
        av_packet_unref(pkt);
    }

    eof = true;
    return nullptr;
}

bool FfmpegVideoReader::reopen()
{
    close();
    return open(mVideoPath);
}

void FfmpegVideoReader::close() {
    if (rgb_buffer) av_free(rgb_buffer);
    if (frame) av_frame_free(&frame);
    if (rgb_frame) av_frame_free(&rgb_frame);
    if (pkt) av_packet_free(&pkt);
    if (sws_ctx) sws_freeContext(sws_ctx);
    if (codec_ctx) avcodec_free_context(&codec_ctx);
    if (fmt_ctx) avformat_close_input(&fmt_ctx);

    fmt_ctx = nullptr;
    codec_ctx = nullptr;
    codec = nullptr;
    sws_ctx = nullptr;
    pkt = nullptr;
    frame = nullptr;
    rgb_frame = nullptr;
    rgb_buffer = nullptr;
    rgb_buffer_size = 0;
    eof = false;
}
