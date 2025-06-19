/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | parse_args.c
    ::  ::          ::  ::    Created  | 2025-06-07
          ::::  ::::          Modified | 2025-06-19

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "errors.h"
#include "options.h"
#include "utilities.h"

typedef struct argument_s
{
    char* key;    // Argument key (e.g., -i, --input)
    char* value;  // Argument value (e.g., rtsp://example.com/stream)
} _argument_t;

/**
 * @brief Parses a command-line argument at the specified index and returns it as an _argument_t
 * structure.
 *
 * This function processes the argument at the given index in the argv array, extracting the key and
 * value if present. It supports arguments in the form of "key=value" as well as "key value" pairs.
 * Special flags such as "-v", "--version", "-h", "--help", "-d", and "--debug" are handled as
 * standalone keys without values.
 *
 * @param argc   The count of command-line arguments.
 * @param argv   The array of command-line argument strings.
 * @param index  The index of the argument to parse (must be >= 1 and < argc).
 *
 * @return       Pointer to a dynamically allocated _argument_t structure containing the parsed key
 * and value, or NULL if parsing fails or memory allocation fails.
 *
 * @note The returned _argument_t structure must be freed by the caller to avoid memory leaks.
 */
_argument_t* _get_argument(int argc, char* argv[], int* index)
{
    if (!index || *index < 1 || *index >= argc)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) create_argument | " ERROR_INVALID_ARGUMENTS);
        return NULL;
    }

    _argument_t* argument = (_argument_t*)malloc(sizeof(_argument_t));
    if (!argument)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) create_argument | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
        goto error;
    }

    argument->key = NULL;
    argument->value = NULL;

    if (!argv[*index] || strlen(argv[*index]) == 0)
        goto error;

    char* eq = strchr(argv[*index], '=');
    if (eq)
    {
        *eq = '\0';
        argument->key = argv[*index];
        argument->value = eq + 1;
        if (!argument->value || strlen(argument->value) == 0)
            goto error;
    }
    else
    {
        argument->key = argv[*index];
        if (strcmp(argument->key, "-v") != 0 && strcmp(argument->key, "--version") != 0 &&
            strcmp(argument->key, "-h") != 0 && strcmp(argument->key, "--help") != 0 &&
            strcmp(argument->key, "-d") != 0 && strcmp(argument->key, "--debug") != 0)
        {
            if (*index + 1 < argc && argv[*index + 1][0] != '-')
                argument->value = argv[++(*index)];
            else
                goto error;
        }
    }

    return argument;

error:
    write_msg_to_fd(STDERR_FILENO, "(f) create_argument | " ERROR_INVALID_ARGUMENTS);
    free(argument);
    return NULL;
}

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
    {
        write_msg_to_fd(STDERR_FILENO, "(f) parse_args | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    int i = 1;
    while (i < argc)
    {
        _argument_t* argument = _get_argument(argc, argv, &i);
        if (!argument)
            return RTN_ERROR;

        char* key = argument->key;
        char* value = argument->value;

#define MATCH(opt, longopt) (strcmp(key, opt) == 0 || strcmp(key, longopt) == 0)

        if (MATCH("-v", "--version"))
        {
            options->version = 1;
            free(argument);
            return RTN_SUCCESS;
        }
        else if (MATCH("-h", "--help"))
        {
            options->help = 1;
            free(argument);
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
            write_msg_to_fd(STDERR_FILENO, "(f) parse_args | " ERROR_INVALID_ARGUMENTS "\n");
            free(argument);
            return RTN_ERROR;
        }

        free(argument);
        i++;
    }

    return RTN_SUCCESS;
}
