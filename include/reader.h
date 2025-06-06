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

#include "errors.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "options.h"

typedef struct stream_s
{
    AVDictionary* options;                  // Options for the RTSP stream (e.g., timeout).
    AVFormatContext* format_context;        // Format context for the RTSP stream.
    int video_stream_index;                 // Index of the video stream in the format context.
    const AVCodec* codec;                   // Codec for decoding the video stream.
    AVCodecContext* codec_context;          // Codec context for decoding the video stream.
    unsigned int number_of_frames_to_read;  // Number of frames to read from the stream.
    // struct SwsContext* sws_context;       // SwsContext for scaling and converting pixel formats.
    // AVPacket* packet;                     // Packet for reading frames from the stream.
    // AVFrame* frame;                       // Frame for storing decoded video frames.
    // size_t frame_size;                    // Size of the frame in bytes.
    // uint8_t* frame_data;                  // Pointer to the frame data buffer.
    // AVStream* video_stream;               // Pointer to the video stream in the format context.
    // double fps;                           // Frame rate of the video stream (frames per second).

} stream_t;

uint8_t* get_streamshot(const options_t* options);

#endif  // READER_H