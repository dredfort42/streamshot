/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | stream.h
    ::  ::          ::  ::    Created  | 2025-06-06
          ::::  ::::          Modified | 2025-06-19

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef STREAM_H
#define STREAM_H

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "options.h"

typedef struct stream_s
{
    AVDictionary* options;                  // Options for the RTSP stream (e.g., timeout).
    AVFormatContext* format_context;        // Format context for the RTSP stream.
    int video_stream_index;                 // Index of the video stream in the format context.
    AVCodecContext* codec_context;          // Codec context for decoding the video stream.
    struct SwsContext* sws_context;         // SwsContext for scaling and converting pixel formats.
    unsigned int number_of_frames_to_read;  // Number of frames to read from the stream.
    long long stop_reading_at;              // Timestamp to stop reading frames (in microseconds).
} stream_t;

stream_t* get_stream(options_t* options);
void free_stream(stream_t* stream);

#endif  // STREAM_H
