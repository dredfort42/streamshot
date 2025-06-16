/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | stream.h
    ::  ::          ::  ::    Created  | 2025-06-06
          ::::  ::::          Modified | 2025-06-16

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef PROCESS_H
#define PROCESS_H

// #include <stdint.h>

// #include "errors.h"
#include "libavcodec/avcodec.h"
// #include "libavformat/avformat.h"
// #include "libavutil/imgutils.h"
// #include "libswscale/swscale.h"
#include "options.h"
// #include "utilities.h"

#define DEFAULT_FPS 25.0f       // Default frame rate for video streams if not specified.
#define I_FRAME_TIMEOUT_SEC 60  // Maximum timeout for I-frames in seconds (1 minute).
#define NETWORK_JITTER_SEC 5    // Network jitter in seconds (5 seconds).

typedef struct process_s
{
    AVFrame* video_frame;                  // Pointer to the decoded video video_frame.
    AVFrame* image_frame;                  // Pointer to the RGB image video_frame.
    size_t image_size;                     // Size of the image in bytes (calculated based on width, height, and pixel format).
    uint8_t* buffer;                       // Pointer to the buffer for storing the RGB image data.
    unsigned long long* pixel_sum_buffer;  // Buffer for summing pixel values across multiple frames.

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

#endif  // PROCESS_H