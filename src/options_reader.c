/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | options_reader.c
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-05

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "options_reader.h"

void _options_init(options_t* options)
{
    memset(options, 0, sizeof(*options));
    options->timeout_sec = DEFAULT_TIMEOUT_SEC;
    options->exposure_sec = DEFAULT_EXPOSURE_SEC;
    options->output_format = DEFAULT_OUTPUT_FORMAT;
    options->scale_factor = DEFAULT_SCALE_FACTOR;
    options->image_quality = DEFAULT_IMAGE_QUALITY;
    options->debug_step = DEFAULT_DEBUG_STEP;
    options->debug_dir = strdup(DEFAULT_DEBUG_DIR);
    if (!options->debug_dir)
    {
        write_to_fd(STDERR_FILENO, ERROR_FAILED_TO_ALLOCATE_MEMORY, NULL);
        exit(RTN_ERROR);
    }
}

short _starts_with(const char* str, const char* prefix) { return str && prefix && strncmp(str, prefix, strlen(prefix)) == 0; }

char* _trim_string(const char* str)
{
    if (!str || strlen(str) == 0)
        return NULL;

    char* result = strdup(str);
    if (!result)
        return NULL;

    char* end = result + strlen(result) - 1;
    while (end > result && (*end == ' ' || *end == '\"' || *end == '\'' || *end == '`' || *end == '\n' || *end == '\r' || *end == '\t')) end--;

    *(end + 1) = '\0';

    char* start = result;
    while (*start && (*start == ' ' || *start == '\"' || *start == '\'' || *start == '`' || *start == '\n' || *start == '\r' || *start == '\t')) start++;

    if (start != result)
        memmove(result, start, end - start + 2);

    return result;
}

short _validate_options(const options_t* options)
{
    if (!options)
        return RTN_ERROR;

    if (!options->rtsp_url || strlen(options->rtsp_url) < 8 || !_starts_with(options->rtsp_url, "rtsp://"))
    {
        write_to_fd(STDERR_FILENO, ERROR_INVALID_RTSP_URL, NULL);
        return RTN_ERROR;
    }

    if (options->timeout_sec < 1 || options->timeout_sec > MAX_TIMEOUT_SEC)
    {
        write_to_fd(STDERR_FILENO, ERROR_INVALID_TIMEOUT, NULL);
        return RTN_ERROR;
    }

    if (options->exposure_sec < 0 || options->exposure_sec > MAX_EXPOSURE_SEC)
    {
        write_to_fd(STDERR_FILENO, ERROR_INVALID_EXPOSURE, NULL);
        return RTN_ERROR;
    }

    if (options->output_format == IMAGE_FORMAT_UNKNOWN)
    {
        write_to_fd(STDERR_FILENO, ERROR_INVALID_OUTPUT_FORMAT, NULL);
        return RTN_ERROR;
    }

    if (options->scale_factor < MIN_SCALE_FACTOR || options->scale_factor > MAX_SCALE_FACTOR)
    {
        write_to_fd(STDERR_FILENO, ERROR_INVALID_SCALE_FACTOR, NULL);
        return RTN_ERROR;
    }

    if (options->resize_height && (options->resize_height < MIN_RESIZE_HEIGHT || options->resize_height > MAX_RESIZE_HEIGHT))
    {
        write_to_fd(STDERR_FILENO, ERROR_INVALID_RESIZE_HEIGHT, NULL);
        return RTN_ERROR;
    }

    if (options->resize_width && (options->resize_width < MIN_RESIZE_WIDTH || options->resize_width > MAX_RESIZE_WIDTH))
    {
        write_to_fd(STDERR_FILENO, ERROR_INVALID_RESIZE_WIDTH, NULL);
        return RTN_ERROR;
    }

    if (options->image_quality < MIN_IMAGE_QUALITY || options->image_quality > MAX_IMAGE_QUALITY)
    {
        write_to_fd(STDERR_FILENO, ERROR_INVALID_IMAGE_QUALITY, NULL);
        return RTN_ERROR;
    }

    if (options->debug)
    {
        if (options->debug_step < 1)
        {
            write_to_fd(STDERR_FILENO, ERROR_INVALID_DEBUG_STEP, NULL);
            return RTN_ERROR;
        }

        if (!options->debug_dir || strlen(options->debug_dir) < 3)
        {
            write_to_fd(STDERR_FILENO, ERROR_INVALID_DEBUG_DIR, NULL);
            return RTN_ERROR;
        }

        if (strcmp(options->debug_dir, DEFAULT_DEBUG_DIR) == 0)
        {
            struct stat st = {0};
            if (stat(options->debug_dir, &st) == -1)
            {
                if (mkdir(options->debug_dir, 0755) == -1)
                {
                    write_to_fd(STDERR_FILENO, ERROR_FAILED_TO_CREATE_DEBUG_DIR, NULL);
                    return RTN_ERROR;
                }
            }
        }
        else
        {
            struct stat st = {0};
            if (stat(options->debug_dir, &st) == -1)
            {
                write_to_fd(STDERR_FILENO, ERROR_INVALID_DEBUG_DIR, NULL);
                return RTN_ERROR;
            }
        }
    }

    return RTN_SUCCESS;
}

short parse_args(int argc, char* argv[], options_t* options)
{
    if (argc < 2 || !options)
    {
        print_help(argv[0]);
        return RTN_ERROR;
    }

    _options_init(options);

    for (int i = 1; i < argc; i++)
    {
        char* arg = argv[i];

        if (MATCH("-v", "--version"))
        {
            print_version();
            return RTN_COMPLETE;
        }
        else if (MATCH("-h", "--help"))
        {
            print_help(argv[0]);
            return RTN_COMPLETE;
        }
        else if (MATCH("-i", "--input"))
            options->rtsp_url = _trim_string(argv[++i]);
        else if (MATCH_PREFIX("--input"))
            options->rtsp_url = _trim_string(strchr(arg, '=') + 1);
        else if (MATCH("-t", "--timeout"))
            options->timeout_sec = atoi(argv[++i]);
        else if (MATCH_PREFIX("--timeout"))
            options->timeout_sec = atoi(strchr(arg, '=') + 1);
        else if (MATCH("-o", "--output-file"))
            options->output_file_path = _trim_string(argv[++i]);
        else if (MATCH_PREFIX("--output-file"))
            options->output_file_path = _trim_string(strchr(arg, '=') + 1);
        else if (MATCH("-O", "--output-fd"))
            options->output_file_fd = atoi(argv[++i]);
        else if (MATCH_PREFIX("--output-fd"))
            options->output_file_fd = atoi(strchr(arg, '=') + 1);
        else if (MATCH("-e", "--exposure"))
            options->exposure_sec = atoi(argv[++i]);
        else if (MATCH_PREFIX("--exposure"))
            options->exposure_sec = atoi(strchr(arg, '=') + 1);
        else if (MATCH("-f", "--output-format"))
        {
            char* format_arg = _trim_string(argv[++i]);
            options->output_format = string_to_image_format(format_arg);
            free(format_arg);
        }
        else if (MATCH_PREFIX("--output-format"))
        {
            char* format_arg = _trim_string(strchr(arg, '=') + 1);
            options->output_format = string_to_image_format(format_arg);
            free(format_arg);
        }
        else if (MATCH("-s", "--scale"))
            options->scale_factor = atof(argv[++i]);
        else if (MATCH_PREFIX("--scale"))
            options->scale_factor = atof(strchr(arg, '=') + 1);
        else if (MATCH("-h", "--resize-height"))
            options->resize_height = atoi(argv[++i]);
        else if (MATCH_PREFIX("--resize-height"))
            options->resize_height = atoi(strchr(arg, '=') + 1);
        else if (MATCH("-w", "--resize-width"))
            options->resize_width = atoi(argv[++i]);
        else if (MATCH_PREFIX("--resize-width"))
            options->resize_width = atoi(strchr(arg, '=') + 1);
        else if (MATCH("-q", "--image-quality"))
            options->image_quality = atoi(argv[++i]);
        else if (MATCH_PREFIX("--image-quality"))
            options->image_quality = atoi(strchr(arg, '=') + 1);
        else if (MATCH("-d", "--debug"))
            options->debug = 1;
        else if (MATCH("--debug-step", "--debug-step"))
            options->debug_step = atoi(argv[++i]);
        else if (MATCH_PREFIX("--debug-step"))
            options->debug_step = atoi(strchr(arg, '=') + 1);
        else if (MATCH("--debug-dir", "--debug-dir"))
            options->debug_dir = _trim_string(argv[++i]);
        else if (MATCH_PREFIX("--debug-dir"))
            options->debug_dir = _trim_string(strchr(arg, '=') + 1);
        else
        {
            write_to_fd(STDERR_FILENO, ERROR_INVALID_ARGUMENTS, NULL);
            return RTN_ERROR;
        }
    }

    return _validate_options(options);
}

void free_options(options_t* options)
{
    if (!options)
        return;

    if (options->rtsp_url)
        free(options->rtsp_url);
    if (options->output_file_path)
        free(options->output_file_path);
    if (options->debug_dir)
        free(options->debug_dir);

    memset(options, 0, sizeof(*options));
}