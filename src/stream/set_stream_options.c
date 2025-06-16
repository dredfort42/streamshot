/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | set_stream_options.c
    ::  ::          ::  ::    Created  | 2025-06-16
          ::::  ::::          Modified | 2025-06-16

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "options.h"
#include "stream.h"
#include "utilities.h"

/**
 * @brief Sets various options for the given stream based on the provided options structure.
 *
 * This function configures the stream's options dictionary with parameters such as RTSP transport,
 * timeout, and debug settings. It validates the input pointers, sets the required options using
 * av_dict_set, and handles errors appropriately. If debug mode is enabled, it prints the current
 * stream options to stdout.
 *
 * @param stream   Pointer to the stream_t structure to configure.
 * @param options  Pointer to the options_t structure containing configuration parameters.
 *
 * @return 0 on success, -1 on failure.
 */
short _set_stream_options(stream_t* stream, const options_t* options)
{
    if (!stream || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _set_stream_options | " ERROR_INVALID_ARGUMENTS "\n");
        goto error;
    }

    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", options->timeout_sec * 1000000);

    short error = 0;
    error |= av_dict_set(&stream->options, "rtsp_transport", "tcp", 0) < 0;
    error |= av_dict_set(&stream->options, "timeout", timeout_str, 0) < 0;

    if (options->debug)
        error |= av_dict_set(&stream->options, "debug", "qp+mv", 0) < 0;

    if (error)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _set_stream_options | " ERROR_FAILED_TO_SET_STREAM_OPTIONS "\n");
        goto error;
    }

    if (options->debug)
    {
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Printing RTSP stream options:\n");

        AVDictionaryEntry* entry = NULL;
        while ((entry = av_dict_get(stream->options, "", entry, AV_DICT_IGNORE_SUFFIX))) printf("%s = %s\n", entry->key, entry->value);
    }

    return RTN_SUCCESS;

error:
    if (stream->options)
    {
        av_dict_free(&stream->options);
        stream->options = NULL;
    }

    return RTN_ERROR;
}