/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | process.h
    ::  ::          ::  ::    Created  | 2025-06-06
          ::::  ::::          Modified | 2025-06-19

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef PROCESS_H
#define PROCESS_H

#include "libavcodec/avcodec.h"
#include "options.h"

#define DEFAULT_FPS 25.0f                // Default frame rate for video streams if not specified.
#define I_FRAME_TIMEOUT_SEC 60           // Maximum timeout for I-frames in seconds (1 minute).
#define FRAME_DELIVERY_LATENCY_SEC 0.3f  // Frame delivery latency in seconds (0.3 seconds).
#define RGB_BYTES_PER_PIXEL 3            // Number of bytes per pixel in RGB format.

typedef struct process_s
{
    AVPacket* av_packet;                 // Pointer to the AVPacket for the current frame.
    AVFrame* video_frame;                // Pointer to the decoded video video_frame.
    AVFrame* image_frame;                // Pointer to the RGB image video_frame.
    size_t image_size;                   // Calculated size of the image in bytes.
    uint8_t* buffer;                     // Pointer to the buffer for storing the RGB image data.
    unsigned long long* sum_buffer;      // Buffer for summing pixel values across multiple frames.
    unsigned long long received_frames;  // Number of frames received from the stream.
    short got_first_i_frame;             // Flag indicating if the first I-frame has been received.
    int stream_read_status;              // Status of the stream reading (0: success, < 0: error).
} process_t;

typedef struct raw_image_s
{
    uint8_t* data;  // Pointer to the raw image data.
    size_t size;    // Size of the raw image data.
    int width;      // Width of the image in pixels.
    int height;     // Height of the image in pixels.
} raw_image_t;

raw_image_t* get_raw_image(options_t* options);
void free_process(process_t* process);
void free_raw_image(raw_image_t* raw_image);

#endif  // PROCESS_H