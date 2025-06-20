/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | options.h
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-20

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef OPTIONS_H
#define OPTIONS_H

#define DEFAULT_TIMEOUT_SEC 10                  // Default timeout for RTSP stream.
#define MAX_TIMEOUT_SEC 300                     // Maximum timeout for RTSP stream.
#define MIN_OUTPUT_FD 3                         // Minimum output file descriptor
#define DEFAULT_EXPOSURE_SEC 0                  // Default exposure time in seconds.
#define MAX_EXPOSURE_SEC 86400                  // Maximum exposure time in seconds.
#define DEFAULT_OUTPUT_FORMAT IMAGE_FORMAT_JPG  // Default output image format.
#define DEFAULT_SCALE_FACTOR 1.0f               // Default image scale factor.
#define MIN_SCALE_FACTOR 0.1f                   // Minimum image scale factor.
#define MAX_SCALE_FACTOR 10.0f                  // Maximum image scale factor.
#define DEFAULT_IMAGE_QUALITY 95                // Default image quality (0 to 100).
#define MIN_IMAGE_QUALITY 0                     // Minimum image quality.
#define MAX_IMAGE_QUALITY 100                   // Maximum image quality.
#define MIN_RESIZE_HEIGHT 108                   // Minimum resize height.
#define MAX_RESIZE_HEIGHT 10800                 // Maximum resize height.
#define MIN_RESIZE_WIDTH 192                    // Minimum resize width.
#define MAX_RESIZE_WIDTH 19200                  // Maximum resize width.
#define DEFAULT_DEBUG_STEP 100                  // Default interval for saving debug files.
#define DEFAULT_DEBUG_DIR "./debug_files"       // Default directory for debug files.

/* Enum for supported image formats */
typedef enum image_format_e
{
    IMAGE_FORMAT_JPG = 0,
    IMAGE_FORMAT_JPEG,
    IMAGE_FORMAT_PNG,
    IMAGE_FORMAT_PPM,
    IMAGE_FORMAT_UNKNOWN
} image_format_t;

const char* image_format_to_string(image_format_t format);
image_format_t string_to_image_format(const char* str);

/**
 * @brief Structure to hold configuration options for the application.
 *
 * This structure encapsulates various settings and parameters
 * that control the behavior and features of the application.
 */
typedef struct options_s
{
    char* rtsp_url;                // RTSP URL to connect to.
    int timeout_sec;               // RTSP stream connection timeout in seconds.
    char* output_file_path;        // Output file path. If omitted, no file is saved.
    int output_file_fd;            // Output file descriptor.
    int exposure_sec;              // Exposure time in seconds.
    image_format_t output_format;  // Image format for output file.
    float scale_factor;            // Image scale factor.
    int resize_height;             // Resize to fit specified height.
    int resize_width;              // Resize to fit specified width.
    int image_quality;             // Image quality (0 to 100).
    char debug;                    // Debug mode: print debug information (0: off, 1: on).
    int debug_step;                // Save debug file every N steps.
    char* debug_dir;               // Directory for debug files (default: ./debug_files).
    char help;                     // Help flag: print usage information (0: off, 1: on).
    char version;                  // Version flag: print version information (0: off, 1: on).
} options_t;

options_t* get_options(int argc, char* argv[]);
short parse_args(int argc, char* argv[], options_t* options);
short validate_options(const options_t* options);
void free_options(options_t* options);
void print_options(const options_t* options);
void print_help(const char* program_name);

#endif  // OPTIONS_H
