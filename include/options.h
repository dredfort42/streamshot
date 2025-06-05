/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | options.h
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-05

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "writer.h"

#define DEFAULT_TIMEOUT_SEC 10                  // Default timeout for RTSP stream connection in seconds.
#define MAX_TIMEOUT_SEC 300                     // Maximum timeout for RTSP stream connection in seconds.
#define DEFAULT_EXPOSURE_SEC 0                  // Default exposure time in seconds (0 means snapshot from the first I-frame).
#define MAX_EXPOSURE_SEC 86400                  // Maximum exposure time in seconds (24 hours).
#define DEFAULT_OUTPUT_FORMAT IMAGE_FORMAT_JPG  // Default output image format.
#define DEFAULT_SCALE_FACTOR 1.0f               // Default image scale factor.
#define MIN_SCALE_FACTOR 0.1f                   // Minimum image scale factor.
#define MAX_SCALE_FACTOR 10.0f                  // Maximum image scale factor.
#define DEFAULT_IMAGE_QUALITY 95                // Default image quality (0 to 100).
#define MIN_IMAGE_QUALITY 0                     // Minimum image quality (0).
#define MAX_IMAGE_QUALITY 100                   // Maximum image quality (100).
#define MIN_RESIZE_HEIGHT 108                   // Minimum resize height (108 pixels).
#define MAX_RESIZE_HEIGHT 10800                 // Maximum resize height (10800 pixels).
#define MIN_RESIZE_WIDTH 192                    // Minimum resize width (192 pixels).
#define MAX_RESIZE_WIDTH 19200                  // Maximum resize width (19200 pixels).
#define DEFAULT_DEBUG_STEP 100                  // Default debug interval for saving debug files (requires debug mode).
#define DEFAULT_DEBUG_DIR "./debug_files"       // Default directory for debug files (requires debug mode).

typedef enum e_image_format
{
    IMAGE_FORMAT_JPG = 0,
    IMAGE_FORMAT_PNG,
    IMAGE_FORMAT_WEBP,
    IMAGE_FORMAT_TIFF,
    IMAGE_FORMAT_BMP,
    IMAGE_FORMAT_PPM,
    IMAGE_FORMAT_UNKNOWN
} image_format_t;

// Helper function to get string representation of image_format_t
static inline const char* image_format_to_string(image_format_t format)
{
    switch (format)
    {
        case IMAGE_FORMAT_JPG:
            return "jpg";
        case IMAGE_FORMAT_PNG:
            return "png";
        case IMAGE_FORMAT_WEBP:
            return "webp";
        case IMAGE_FORMAT_TIFF:
            return "tiff";
        case IMAGE_FORMAT_BMP:
            return "bmp";
        case IMAGE_FORMAT_PPM:
            return "ppm";
        default:
            return "unknown";
    }
}

// Helper function to convert string to image_format_t
static inline image_format_t string_to_image_format(const char* str)
{
    if (!strcmp(str, "jpg"))
        return IMAGE_FORMAT_JPG;
    else if (!strcmp(str, "png"))
        return IMAGE_FORMAT_PNG;
    else if (!strcmp(str, "webp"))
        return IMAGE_FORMAT_WEBP;
    else if (!strcmp(str, "tiff"))
        return IMAGE_FORMAT_TIFF;
    else if (!strcmp(str, "bmp"))
        return IMAGE_FORMAT_BMP;
    else if (!strcmp(str, "ppm"))
        return IMAGE_FORMAT_PPM;
    else
        return IMAGE_FORMAT_UNKNOWN;
}

typedef struct s_options
{
    char* rtsp_url;                // RTSP URL to connect to.
    int timeout_sec;               // RTSP stream connection timeout in seconds.
    char* output_file_path;        // Output file path. If omitted, no file is saved.
    int output_file_fd;            // Output file descriptor. Takes precedence over output_file_path if both are specified.
    int exposure_sec;              // Exposure time in seconds.
    image_format_t output_format;  // Image format for output file.
    float scale_factor;            // Image scale factor.
    int resize_height;             // Resize to fit specified height, maintaining aspect ratio (min: 108, max: 10800).
    int resize_width;              // Resize to fit specified width, maintaining aspect ratio (min: 192, max: 19200).
    int image_quality;             // Image quality (0 to 100).
    char debug;                    // Debug mode: print debug information (0: off, 1: on).
    int debug_step;                // Save debug file every N steps.
    char* debug_dir;               // Directory for debug files (default: ./debug_files).
} options_t;

#define MATCH(opt, longopt) (strcmp(arg, opt) == 0 || strcmp(arg, longopt) == 0)
#define MATCH_PREFIX(opt) (strncmp(arg, opt "=", strlen(opt) + 1) == 0)

short parse_args(int argc, char* argv[], options_t* options);
void print_help(const char* program_name);
void print_version();
void print_options(const options_t* options);
void free_options(options_t* options);

#endif  // OPTIONS_H
