/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | printer.c
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-06

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "options.h"
#include "utilities.h"

/**
 * @brief Prints the current configuration options.
 *
 * This function outputs the values of the provided options structure.
 * It is typically used for debugging or logging purposes to display
 * the current state of all configurable options.
 *
 * @param options Pointer to an options_t structure containing the
 *                configuration settings to be printed.
 */
void print_options(const options_t* options)
{
    if (!options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) print_options | " ERROR_INVALID_ARGUMENTS "\n");
        return;
    }

    printf("RTSP url: %s\n", options->rtsp_url ? options->rtsp_url : "NULL");
    printf("Timeout: %d seconds\n", options->timeout_sec);
    printf("Output File Path: %s\n",
           options->output_file_path ? options->output_file_path : "NULL");
    printf("Output File Descriptor: %d\n", options->output_file_fd);
    printf("Exposure Time: %d seconds\n", options->exposure_sec);
    printf("Output Format: %s\n", image_format_to_string(options->output_format));
    printf("Scale Factor: %.1f\n", options->scale_factor);
    printf("Resize Height: %d pixels\n", options->resize_height);
    printf("Resize Width: %d pixels\n", options->resize_width);
    printf("Image Quality: %d\n", options->image_quality);
    printf("Debug Mode: %s\n", options->debug ? "Enabled" : "Disabled");
    printf("Debug Step Interval: %d steps\n", options->debug_step);
    printf("Debug Directory: %s\n", options->debug_dir ? options->debug_dir : "NULL");
}