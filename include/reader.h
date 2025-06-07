/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | reader.h
    ::  ::          ::  ::    Created  | 2025-06-06
          ::::  ::::          Modified | 2025-06-06

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef READER_H
#define READER_H

#include <stdint.h>
#include <sys/time.h>

#include "errors.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "options.h"

typedef struct stream_s
{
    AVDictionary* options;                  // Options for the RTSP stream (e.g., timeout).
    AVFormatContext* format_context;        // Format context for the RTSP stream.
    int video_stream_index;                 // Index of the video stream in the format context.
    const AVCodec* codec;                   // Codec for decoding the video stream.
    AVCodecContext* codec_context;          // Codec context for decoding the video stream.
    unsigned int number_of_frames_to_read;  // Number of frames to read from the stream.
    long long stop_reading_at;              // Timestamp to stop reading frames (in microseconds).
    struct SwsContext* sws_context;         // SwsContext for scaling and converting pixel formats.
    // AVFrame* frame;                       // Frame for storing decoded video frames.
    // size_t frame_size;                    // Size of the frame in bytes.
    // uint8_t* frame_data;                  // Pointer to the frame data buffer.
    // AVStream* video_stream;               // Pointer to the video stream in the format context.
    // double fps;                           // Frame rate of the video stream (frames per second).

} stream_t;

// typedef struct conversion_s
// {
//     AVPacket* packet;
//     AVFrame* frame;              // Frame for storing decoded video frames.
//     struct SwsContext* sws_ctx;  // SwsContext for scaling and converting pixel formats.
//     AVPixelFormat src_format;    // Source pixel format.
//     AVPixelFormat dst_format;    // Destination pixel format.
//     int src_width;               // Source width.
//     int src_height;              // Source height.
//     int dst_width;               // Destination width.
//     int dst_height;              // Destination height.
// } conversion_t;

uint8_t* get_streamshot(const options_t* options);

stream_t* _init_stream();
short _set_stream_options(stream_t* stream, const options_t* options);
short _detect_video_stream(stream_t* stream);
short _open_stream(stream_t* stream, const options_t* options);
short _find_decoder(stream_t* stream, const options_t* options);
short _init_codec_context(stream_t* stream, const options_t* options);
short _calculate_number_of_frames_to_read(stream_t* stream, const options_t* options);
short _init_sws_context(stream_t* stream, const options_t* options, float scale_factor);
void _free_stream(stream_t* stream);

uint8_t* _process_streamshot(stream_t* stream, const options_t* options);

#endif  // READER_H