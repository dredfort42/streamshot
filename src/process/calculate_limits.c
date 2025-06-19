/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | calculate_limits.c
    ::  ::          ::  ::    Created  | 2025-06-16
          ::::  ::::          Modified | 2025-06-19

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "process.h"
#include "stream.h"
#include "utilities.h"

/**
 * @brief Calculates and sets the number of frames to read from a video stream based on the exposure
 * time and stream properties.
 *
 * This function determines the number of frames to read from the provided stream, using the
 * exposure time specified in the options. It uses the stream's average frame rate (or real frame
 * rate as a fallback) to compute the frame count. If the exposure time is zero, only one frame is
 * read.
 *
 * @param stream   Pointer to the stream_t structure containing stream information.
 * @param options  Pointer to the options_t structure containing configuration options.
 *
 * @return 0 on success, -1 on failure.
 *
 * @note
 * The function also ensures that the calculated frame count does not exceed a safe upper limit to
 * prevent overflow.
 */
short _number_of_frames_to_read(stream_t* stream, const options_t* options)
{
    if (!stream || !options || options->exposure_sec < 0)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _number_of_frames_to_read | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    if (options->exposure_sec == 0)
        stream->number_of_frames_to_read = 1;
    else
    {
        double fps = DEFAULT_FPS;
        AVStream* video_stream = stream->format_context->streams[stream->video_stream_index];
        if (video_stream->avg_frame_rate.den && video_stream->avg_frame_rate.num)
            fps = av_q2d(video_stream->avg_frame_rate);
        else if (video_stream->r_frame_rate.den && video_stream->r_frame_rate.num)
            fps = av_q2d(video_stream->r_frame_rate);

        if (fps <= 0)
        {
            write_msg_to_fd(STDERR_FILENO,
                            "(f) _calculate_number_of_frames_to_read | " ERROR_INVALID_FPS "\n");
            return RTN_ERROR;
        }

        stream->number_of_frames_to_read = (unsigned int)(options->exposure_sec * fps + 0.5);

        unsigned long long safe_frame_limit = ULLONG_MAX / UINT8_MAX;
        if (stream->number_of_frames_to_read > safe_frame_limit)
            stream->number_of_frames_to_read = safe_frame_limit;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Frames to read: %u\n",
               stream->number_of_frames_to_read);

    return RTN_SUCCESS;
}

/**
 * @brief Calculates and sets the timestamp at which reading from the stream should stop.
 *
 * This function determines the stop time for reading from the given stream based on the provided
 * options. If the exposure time is zero, it uses a default timeout (I_FRAME_TIMEOUT_SEC). If the
 * exposure time is positive, it adds the exposure time and network jitter to the timeout. If the
 * exposure time is negative, it writes an error message to STDERR.
 *
 * @param stream   Pointer to the stream_t structure containing stream information.
 * @param options  Pointer to the options_t structure containing configuration options.
 *
 * @return 0 on success, -1 on failure.
 */
short _stop_reading_at(stream_t* stream, const options_t* options)
{
    if (!stream || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _stop_reading_at | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    double fps = DEFAULT_FPS;
    unsigned int number_of_frames_to_read = 0;
    if (stream->number_of_frames_to_read > 0)
        number_of_frames_to_read = stream->number_of_frames_to_read;
    else
        number_of_frames_to_read = (unsigned int)(options->exposure_sec * fps + 0.5);

    if (!options->exposure_sec)
        stream->stop_reading_at =
            time_now_in_microseconds() + (long long)(I_FRAME_TIMEOUT_SEC * 1000000);
    else if (options->exposure_sec > 0)
        stream->stop_reading_at =
            time_now_in_microseconds() +
            (long long)((I_FRAME_TIMEOUT_SEC + options->exposure_sec +
                         FRAME_DELIVERY_LATENCY_SEC * number_of_frames_to_read) *
                        1000000);
    else
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _stop_reading_at | " ERROR_INVALID_EXPOSURE "\n");
        return RTN_ERROR;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Stop reading at: %lld microseconds\n",
               stream->stop_reading_at);

    return RTN_SUCCESS;
}

/**
 * @brief Calculates and sets stream processing limits based on provided options.
 *
 * @param stream   Pointer to the stream_t structure containing stream information.
 * @param options  Pointer to the options_t structure containing configuration options.
 *
 * @return 0 on success, -1 on failure.
 */
short _calculate_limits(stream_t* stream, const options_t* options)
{
    if (!stream || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _calculate_limits | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    if (_number_of_frames_to_read(stream, options) || _stop_reading_at(stream, options))
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _calculate_limits | " ERROR_FAILED_TO_CALCULATE_LIMITS "\n");
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}