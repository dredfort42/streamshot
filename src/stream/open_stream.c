/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | open_stream.c
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
 * @brief Detects and sets the index of the first video stream in the given stream object.
 *
 * This function scans through the streams in the provided stream_t structure's format context,
 * searching for a stream of type AVMEDIA_TYPE_VIDEO. If found, it sets the video_stream_index
 * member of the stream_t structure to the index of the video stream. If no video stream is found,
 * or if the input arguments are invalid, an error message is written to STDERR and an error code
 * isreturned.
 *
 * @param stream Pointer to a stream_t structure containing the format context to search.
 *
 * @return 0 if a video stream is found and its index is set; -1 otherwise.
 */
short _detect_video_stream(stream_t* stream)
{
    if (!stream || !stream->format_context)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _detect_video_stream | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    stream->video_stream_index = -1;

    if (!stream->format_context->nb_streams)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _detect_video_stream | " ERROR_NO_STREAMS_FOUND "\n");
        return RTN_ERROR;
    }

    for (unsigned i = 0; i < stream->format_context->nb_streams; ++i)
    {
        if (stream->format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            stream->video_stream_index = i;
            break;
        }
    }

    if (stream->video_stream_index == -1)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _detect_video_stream | " ERROR_NO_VIDEO_STREAM_FOUND "\n");
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

/**
 * @brief Opens an RTSP stream and initializes the stream context.
 *
 * This function attempts to open an RTSP stream using the provided options,
 * retrieves stream information, and detects the video stream index. It also
 * provides debug output if enabled in the options. In case of failure at any
 * step, it performs cleanup and returns an error code.
 *
 * @param stream Pointer to a stream_t structure to be initialized.
 * @param options Pointer to an options_t structure containing stream options and configuration.
 *
 * @return 0 on success, -1 on failure.
 */
short _open_stream(stream_t* stream, const options_t* options)
{
    if (!stream || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _open_stream | " ERROR_INVALID_ARGUMENTS "\n");
        goto error;
    }

    if (avformat_open_input(&stream->format_context, options->rtsp_url, NULL, &stream->options) < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _open_stream | " ERROR_FAILED_TO_OPEN_RTSP_STREAM "\n");
        goto error;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Successfully opened RTSP stream: %s\n",
               options->rtsp_url);

    if (avformat_find_stream_info(stream->format_context, NULL) < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _open_stream | " ERROR_FAILED_TO_GET_STREAM_INFO "\n");
        goto error;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET
                         " Found information about %d streams in the RTSP stream.\n",
               stream->format_context->nb_streams);

    if (_detect_video_stream(stream) != RTN_SUCCESS)
        goto error;

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Detected video stream index: %d\n",
               stream->video_stream_index);

    return RTN_SUCCESS;

error:
    if (stream && stream->format_context)
    {
        avformat_close_input(&stream->format_context);
        stream->format_context = NULL;
    }

    return RTN_ERROR;
}
