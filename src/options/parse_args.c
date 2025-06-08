/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | parse_args.c
    ::  ::          ::  ::    Created  | 2025-06-07
          ::::  ::::          Modified | 2025-06-08

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "options.h"

/**
 * @brief Parses command-line arguments and populates the provided options structure.
 *
 * This function processes command-line arguments to configure application options.
 * It supports both short and long option formats (e.g., -i/--input), as well as
 * key=value and key value argument styles. Special flags for help and version
 * are handled, and the function will print help/version and exit early if those
 * flags are encountered.
 *
 * Supported options:
 *   - -v, --version           : Print version information and exit.
 *   - -h, --help              : Print help message and exit.
 *   - -i, --input             : Set the RTSP input URL.
 *   - -t, --timeout           : Set RTSP stream connection timeout in seconds.
 *   - -o, --output-file       : Set the output file path.
 *   - -O, --output-fd         : Set the output file descriptor.
 *   - -e, --exposure          : Set the exposure time in seconds.
 *   - -f, --output-format     : Set the output image format.
 *   - -s, --scale             : Set the image scale factor.
 *   - -h, --resize-height     : Set the resize height.
 *   - -w, --resize-width      : Set the resize width.
 *   - -q, --image-quality     : Set the image quality.
 *   - -d, --debug             : Enable debug mode.
 *   -   , --debug-step        : Set the debug step value.
 *   -   , --debug-dir         : Set the debug directory.
 *
 * If an invalid argument is encountered, an error message is written to stderr
 * and the function returns an error code.
 *
 * @param argc      The number of command-line arguments.
 * @param argv      The array of command-line argument strings.
 * @param options   Pointer to the options_t structure to populate.
 *
 * @return If arguments were parsed successfully, returns 0.
 *         If an error occurred (e.g., invalid arguments), returns -1.
 */
short parse_args(int argc, char* argv[], options_t* options)
{
    if (argc < 2 || !options)
        return RTN_ERROR;

    int i = 1;
    while (i < argc)
    {
        char* key = NULL;
        char* value = NULL;

        char* eq = strchr(argv[i], '=');
        if (eq)
        {
            *eq = '\0';
            key = argv[i];
            value = eq + 1;
            if (!value || strlen(value) == 0)
                goto invalid_arg;
        }
        else
        {
            key = argv[i];
            if (strcmp(key, "-v") != 0 && strcmp(key, "--version") != 0 && strcmp(key, "-h") != 0 && strcmp(key, "--help") != 0 && strcmp(key, "-d") != 0 &&
                strcmp(key, "--debug") != 0)
            {
                if (i + 1 < argc && argv[i + 1][0] != '-')
                    value = argv[++i];
                else
                    goto invalid_arg;
            }
        }

#define MATCH(opt, longopt) (strcmp(key, opt) == 0 || strcmp(key, longopt) == 0)

        if (MATCH("-v", "--version"))
        {
            options->version = 1;
            return RTN_SUCCESS;
        }
        else if (MATCH("-h", "--help"))
        {
            options->help = 1;
            return RTN_SUCCESS;
        }
        else if (MATCH("-i", "--input"))
            options->rtsp_url = trim_flag_value(value);
        else if (MATCH("-t", "--timeout"))
            options->timeout_sec = atoi(value);
        else if (MATCH("-o", "--output-file"))
            options->output_file_path = trim_flag_value(value);
        else if (MATCH("-O", "--output-fd"))
            options->output_file_fd = atoi(value);
        else if (MATCH("-e", "--exposure"))
            options->exposure_sec = atoi(value);
        else if (MATCH("-f", "--output-format"))
        {
            char* format_arg = trim_flag_value(value);
            options->output_format = string_to_image_format(format_arg);
            free(format_arg);
        }
        else if (MATCH("-s", "--scale"))
            options->scale_factor = atof(value);
        else if (MATCH("-h", "--resize-height"))
            options->resize_height = atoi(value);
        else if (MATCH("-w", "--resize-width"))
            options->resize_width = atoi(value);
        else if (MATCH("-q", "--image-quality"))
            options->image_quality = atoi(value);
        else if (MATCH("-d", "--debug"))
            options->debug = 1;
        else if (MATCH("--debug-step", "--debug-step"))
            options->debug_step = atoi(value);
        else if (MATCH("--debug-dir", "--debug-dir"))
            options->debug_dir = trim_flag_value(value);
        else
        {
        invalid_arg:
            write_msg_to_fd(STDERR_FILENO, "(f) parse_args | " ERROR_INVALID_ARGUMENTS);
            return RTN_ERROR;
        }

        i++;
    }

    return RTN_SUCCESS;
}
