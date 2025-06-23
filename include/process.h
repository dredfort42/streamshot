/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | process.h
    ::  ::          ::  ::    Created  | 2025-06-06
          ::::  ::::          Modified | 2025-06-20

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef PROCESS_H
#define PROCESS_H

#include "libavcodec/avcodec.h"
#include "options.h"

/* Default settings */
#define DEFAULT_FPS 25.0f                // Default frame rate for video streams if not specified.
#define I_FRAME_TIMEOUT_SEC 60           // Maximum timeout for I-frames in seconds (1 minute).
#define FRAME_DELIVERY_LATENCY_SEC 0.3f  // Frame delivery latency in seconds (0.3 seconds).
#define RGB_BYTES_PER_PIXEL 3            // Number of bytes per pixel in RGB format.

/* Quality settings for image scaling */
#define QUALITY_FAST_BILINEAR 20  // Prioritizing speed over quality.
#define QUALITY_BILINEAR 40       // A balance between speed and quality.
#define QUALITY_BICUBIC 60        // Providing higher quality at the cost of performance.
#define QUALITY_GAUSS 80          // Typically used for high-quality smoothing or scaling.
#define QUALITY_LANCZOS 100       // High quality.

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

typedef struct image_s
{
    uint8_t* data;  // Pointer to the image data.
    size_t size;    // Size of the image data.
    int width;      // Width of the image in pixels.
    int height;     // Height of the image in pixels.
} image_t;

image_t* get_raw_image(options_t* options);
image_t* get_converted_image(options_t* options, image_t* raw_image);
image_t* get_ppm_image(const uint8_t* data, size_t size, int width, int height);
image_t* get_jpg_image(const uint8_t* data, size_t size, int width, int height, short quality);
image_t* get_png_image(const uint8_t* data, size_t size, int width, int height, short quality);
void free_process(process_t* process);
void free_image(image_t* image);

#endif  // PROCESS_H
