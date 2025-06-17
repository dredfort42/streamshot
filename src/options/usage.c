/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | usage.c
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-07

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdio.h>
#include <unistd.h>

#include "errors.h"
#include "options.h"
#include "utilities.h"

/**
 * @brief Prints usage information for the application.
 *
 * This function outputs a detailed help message describing all
 * available command-line options, their parameters, and default values.
 *
 * @param prog_name The name of the executable (typically argv[0]).
 */
void print_help(const char* prog_name)
{
    if (!prog_name)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) print_help | " ERROR_INVALID_ARGUMENTS "\n");
        return;
    }

    printf("Usage: %s [OPTIONS]\n\n", prog_name);

    printf("Options:\n");

    printf("  -i, --input           <string>   RTSP URL to connect to (required).\n");

    printf(
        "  -t, --timeout         <uint>     RTSP stream connection timeout in seconds (default: "
        "%u, max: %u)\n",
        DEFAULT_TIMEOUT_SEC, MAX_TIMEOUT_SEC);

    printf("  -o, --output-file     <string>   Output file path. If omitted, no file is saved.\n");

    printf("  -O, --output-fd       <uint>     Output file descriptor. (min: %d)\n", MIN_OUTPUT_FD);

    printf("  -e, --exposure        <uint>     Exposure time in seconds (max: %u).\n",
           MAX_EXPOSURE_SEC);

    printf(
        "                                   If omitted, snapshot is from the first I-frame; "
        "otherwise, averages frames over this time.\n");

    printf("  -f, --output-format   <string>   Output image format: %s, %s, %s (default: %s)\n",
           image_format_to_string(IMAGE_FORMAT_JPG), image_format_to_string(IMAGE_FORMAT_PNG),
           image_format_to_string(IMAGE_FORMAT_PPM), image_format_to_string(IMAGE_FORMAT_JPG));

    printf(
        "  -s, --scale           <uint>     Image scale factor (default: %.1f, min: %.1f, max: "
        "%.1f)\n",
        DEFAULT_SCALE_FACTOR, MIN_SCALE_FACTOR, MAX_SCALE_FACTOR);

    printf(
        "                                   If `--scale` is set, then `--resize-height` and "
        "`--resize-width` are ignored.\n");

    printf(
        "  -h, --resize-height   <uint>     Resize to fit specified height, maintaining aspect "
        "ratio (%u to %u pixels)\n",
        MIN_RESIZE_HEIGHT, MAX_RESIZE_HEIGHT);

    printf(
        "  -w, --resize-width    <uint>     Resize to fit specified width, maintaining aspect "
        "ratio (%u to %u pixels)\n",
        MIN_RESIZE_WIDTH, MAX_RESIZE_WIDTH);

    printf(
        "                                   If both `--resize-height` and `--resize-width` are set "
        "image is resized to fit within these dimensions with "
        "maintaining aspect ratio.\n");

    printf("  -q, --image-quality   <uint>     Image quality (%u to %u)\n", MIN_IMAGE_QUALITY,
           MAX_IMAGE_QUALITY);

    printf(
        "  -d, --debug                      Enable debug mode to print additional information\n");

    printf("      --debug-step      <uint>     Save debug file every N steps (default: %u)\n",
           DEFAULT_DEBUG_STEP);

    printf("      --debug-dir       <string>   Directory for debug files (default: %s)\n",
           DEFAULT_DEBUG_DIR);

    printf("  -h, --help                       Show this help message\n");

    printf("  -v, --version                    Show version information\n");
}