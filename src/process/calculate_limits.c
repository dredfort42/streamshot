/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | calculate_limits.c
    ::  ::          ::  ::    Created  | 2025-06-16
          ::::  ::::          Modified | 2025-06-16

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "stream.h"
#include "utilities.h"

short _number_of_frames_to_read(stream_t* stream, const options_t* options)
{
    if (!stream || !options || options->exposure_sec < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _number_of_frames_to_read | " ERROR_INVALID_ARGUMENTS "\n");

        return RTN_ERROR;
    }

    if (options->exposure_sec == 0)
        stream->number_of_frames_to_read = 1;
    else
    {
        double fps;
        AVStream* video_stream = stream->format_context->streams[stream->video_stream_index];
        if (video_stream->avg_frame_rate.den && video_stream->avg_frame_rate.num)
            fps = av_q2d(video_stream->avg_frame_rate);
        else if (video_stream->r_frame_rate.den && video_stream->r_frame_rate.num)
            fps = av_q2d(video_stream->r_frame_rate);
        else
            fps = 25.0;

        if (fps <= 0)
        {
            write_msg_to_fd(STDERR_FILENO,
                            "(f) _calculate_number_of_frames_to_read | Invalid video_frame rate.\n"
                            "\n");

            return RTN_ERROR;
        }

        stream->number_of_frames_to_read = (unsigned int)(options->exposure_sec * fps);

        unsigned long long max_uint64 = ULLONG_MAX;
        unsigned long long max_uint8 = UINT8_MAX;
        unsigned long long safe_frame_limit = max_uint64 / max_uint8;
        if (stream->number_of_frames_to_read > safe_frame_limit)
            stream->number_of_frames_to_read = safe_frame_limit;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Calculated frames to read: %u\n", stream->number_of_frames_to_read);

    return RTN_SUCCESS;
}

short _stop_reading_at(stream_t* stream, const options_t* options)
{
    if (!stream || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _stop_reading_at | " ERROR_INVALID_ARGUMENTS "\n");

        return RTN_ERROR;
    }

    if (options->exposure_sec > 0)
        stream->stop_reading_at = time_now_in_microseconds() + (long long)(options->exposure_sec * 2 * 1000000);
    else
        stream->stop_reading_at = time_now_in_microseconds() + (long long)(options->timeout_sec * 2 * 1000000);

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Stop reading at: %lld microseconds\n", stream->stop_reading_at);

    return RTN_SUCCESS;
}

short _calculate_limits(stream_t* stream, const options_t* options)
{
    if (!stream || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _calculate_limits | " ERROR_INVALID_ARGUMENTS "\n");

        return RTN_ERROR;
    }

    if (_number_of_frames_to_read(stream, options) || _stop_reading_at(stream, options))
    {
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}