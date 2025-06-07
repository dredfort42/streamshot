/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | parse_args.c
    ::  ::          ::  ::    Created  | 2025-06-07
          ::::  ::::          Modified | 2025-06-07

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "options.h"

char* _trim_flag_value(const char* str)
{
    if (!str || strlen(str) == 0)
        return NULL;

    char* result = strdup(str);
    if (!result)
        return NULL;

    char* end = result + strlen(result) - 1;
    while (end > result && (*end == ' ' || *end == '\"' || *end == '\'' || *end == '`')) end--;

    *(end + 1) = '\0';

    char* start = result;
    while (*start && (*start == ' ' || *start == '\"' || *start == '\'' || *start == '`')) start++;

    if (start != result)
        memmove(result, start, end - start + 2);

    return result;
}

short parse_args(int argc, char* argv[], options_t* options)
{
    if (argc < 2 || !options)
    {
        print_help(argv[0]);
        return RTN_ERROR;
    }

    int i = 1;
    while (i < argc)
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
            options->rtsp_url = _trim_flag_value(argv[++i]);
        else if (MATCH_PREFIX("--input"))
            options->rtsp_url = _trim_flag_value(strchr(arg, '=') + 1);
        else if (MATCH("-t", "--timeout"))
            options->timeout_sec = atoi(argv[++i]);
        else if (MATCH_PREFIX("--timeout"))
            options->timeout_sec = atoi(strchr(arg, '=') + 1);
        else if (MATCH("-o", "--output-file"))
            options->output_file_path = _trim_flag_value(argv[++i]);
        else if (MATCH_PREFIX("--output-file"))
            options->output_file_path = _trim_flag_value(strchr(arg, '=') + 1);
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
            char* format_arg = _trim_flag_value(argv[++i]);
            options->output_format = string_to_image_format(format_arg);
            free(format_arg);
        }
        else if (MATCH_PREFIX("--output-format"))
        {
            char* format_arg = _trim_flag_value(strchr(arg, '=') + 1);
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
            options->debug_dir = _trim_flag_value(argv[++i]);
        else if (MATCH_PREFIX("--debug-dir"))
            options->debug_dir = _trim_flag_value(strchr(arg, '=') + 1);
        else
        {
            write_msg_to_fd(STDERR_FILENO, "(f) parse_args | " ERROR_INVALID_ARGUMENTS);
            return RTN_ERROR;
        }

        i++;
    }

    return RTN_SUCCESS;
}
