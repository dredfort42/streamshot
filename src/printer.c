/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | printer.c
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-05

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "options_reader.h"

void print_help(const char* prog_name)
{
    printf("Usage: %s [OPTIONS]\n\n", prog_name);
    printf("Options:\n");
    printf("  -i, --input           <string>   RTSP URL to connect to (required).\n");
    printf("  -t, --timeout         <uint>     RTSP stream connection timeout in seconds (default: %u, max: %u)\n", DEFAULT_TIMEOUT_SEC, MAX_TIMEOUT_SEC);
    printf("  -o, --output-file     <string>   Output file path. If omitted, no file is saved.\n");
    printf("  -O, --output-fd       <uint>     Output file descriptor. Takes precedence over -o, --output-file if both are specified.\n");
    printf("  -e, --exposure        <uint>     Exposure time in seconds (max: %u).\n", MAX_EXPOSURE_SEC);
    printf("                                   If omitted, snapshot is from the first I-frame; otherwise, averages frames over this time.\n");
    printf("  -f, --output-format   <string>   Output image format: %s, %s, %s, %s, %s, %s (default: %s)\n", image_format_to_string(IMAGE_FORMAT_JPG),
           image_format_to_string(IMAGE_FORMAT_PNG), image_format_to_string(IMAGE_FORMAT_WEBP), image_format_to_string(IMAGE_FORMAT_TIFF),
           image_format_to_string(IMAGE_FORMAT_BMP), image_format_to_string(IMAGE_FORMAT_PPM), image_format_to_string(IMAGE_FORMAT_JPG));
    printf("  -s, --scale           <uint>     Image scale factor (0.1 to 10) (default: %.1f, min: %.1f, max: %.1f)\n", DEFAULT_SCALE_FACTOR, MIN_SCALE_FACTOR,
           MAX_SCALE_FACTOR);
    printf("  -h, --resize-height   <uint>     Resize height (%u to %u pixels)\n", MIN_RESIZE_HEIGHT, MAX_RESIZE_HEIGHT);
    printf("  -w, --resize-width    <uint>     Resize width (%u to %u pixels)\n", MIN_RESIZE_WIDTH, MAX_RESIZE_WIDTH);
    printf("  -q, --image-quality   <uint>     Image quality (%u to %u)\n", MIN_IMAGE_QUALITY, MAX_IMAGE_QUALITY);
    printf("  -d, --debug                      Enable debug mode to print additional information\n");
    printf("      --debug-step      <uint>     Save debug file every N steps (default: %u)\n", DEFAULT_DEBUG_STEP);
    printf("      --debug-dir       <string>   Directory for debug files (default: %s)\n", DEFAULT_DEBUG_DIR);
    printf("  -h, --help                       Show this help message\n");
    printf("  -v, --version                    Show version information\n");
    printf("  -c, --config          <string>   Path to configuration file (default: no config file used)\n");
}

void print_version()
{
    printf("RTSP Stream Image Capture Tool\n");
    printf("Version: %s\n", "1.0.0");
    printf("Author: Dmitry Novikov <dredfort.42@gmail.com>\n");
    printf("GitHub: https://github.com/dredfort42\n");
    printf("For usage details, run with --help.\n");
}

void print_options(const options_t* options)
{
    if (!options)
    {
        write_to_fd(STDERR_FILENO, ERROR_INVALID_ARGUMENTS, NULL);
        return;
    }

    printf("RTSP url: %s\n", options->rtsp_url ? options->rtsp_url : "NULL");
    printf("Timeout: %u seconds\n", options->timeout_sec);
    printf("Output File Path: %s\n", options->output_file_path ? options->output_file_path : "NULL");
    printf("Output File Descriptor: %d\n", options->output_file_fd);
    printf("Exposure Time: %u seconds\n", options->exposure_sec);
    printf("Output Format: %s\n", image_format_to_string(options->output_format));
    printf("Scale Factor: %.1f\n", options->scale_factor);
    printf("Resize Height: %u pixels\n", options->resize_height);
    printf("Resize Width: %u pixels\n", options->resize_width);
    printf("Image Quality: %u\n", options->image_quality);
    printf("Debug Mode: %s\n", options->debug ? "Enabled" : "Disabled");
    printf("Debug Step Interval: %u steps\n", options->debug_step);
    printf("Debug Directory: %s\n", options->debug_dir ? options->debug_dir : "NULL");
    printf("Config file: %s\n", options->config_file_path ? options->config_file_path : "NULL");
}