/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | get_raw_image.c
    ::  ::          ::  ::    Created  | 2025-06-19
          ::::  ::::          Modified | 2025-06-19

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "process.h"
#include "stream.h"
#include "utilities.h"

process_t* _init_process(const stream_t* stream, const options_t* options);
raw_image_t* _init_raw_image(const process_t* process, const stream_t* stream,
                             const options_t* options);
short _calculate_limits(stream_t* stream, const options_t* options);
short _read_frame(stream_t* stream, process_t* process, const options_t* options);
short _scale_image(raw_image_t* raw_image, const options_t* options);

/**
 * @brief Checks the status of the image processing operation.
 *
 * This function verifies whether the given process has successfully received
 * the required number of frames from the stream. If the process or stream
 * pointers are invalid, or if the number of received frames is less than
 * the expected number, an error code is returned.
 *
 * @param process Pointer to the process_t structure containing process state.
 * @param stream Pointer to the stream_t structure containing stream parameters.
 *
 * @return 0 if the required number of frames were processed, -1 otherwise.
 */
short _check_process_status(const process_t* process, const stream_t* stream)
{
    if (!process || !stream)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _check_process_status | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    if (process->received_frames < stream->number_of_frames_to_read)
    {
        char error_msg[512];
        if (process->stream_read_status < 0)
            snprintf(error_msg, sizeof(error_msg),
                     "(f) _check_process_status | Camera connection lost or error occurred "
                     "(av_read_frame returned %d; processed %llu out of %u frames).\n",
                     process->stream_read_status, process->received_frames,
                     stream->number_of_frames_to_read);
        else
            snprintf(error_msg, sizeof(error_msg),
                     "(f) _check_process_status | Stream ended before enough frames were processed "
                     "(%llu out of %u frames).\n",
                     process->received_frames, stream->number_of_frames_to_read);

        write_msg_to_fd(STDERR_FILENO, error_msg);
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

/**
 * @brief Retrieves a raw image from a stream based on the provided options.
 *
 * This function initializes the necessary stream and process structures,
 * reads frames from an RTSP stream according to the specified options,
 * and constructs a raw image from the received data.
 *
 * @param options  Pointer to the options_t structure containing configuration options.
 *
 * @return Pointer to a raw_image_t structure containing the image data,
 *         or NULL if an error occurred during the process.
 */
raw_image_t* get_raw_image(options_t* options)
{
    if (!options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | " ERROR_INVALID_ARGUMENTS "\n");
        return NULL;
    }

    stream_t* stream = get_stream(options);
    if (stream == NULL)
        return NULL;

    process_t* process = _init_process(stream, options);
    if (!process)
        goto error;

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Starting to read %u frames from RTSP stream...\n",
               stream->number_of_frames_to_read);

    if (_calculate_limits(stream, options))
        goto error;

    while (process->received_frames < stream->number_of_frames_to_read &&
           time_now_in_microseconds() < stream->stop_reading_at &&
           !_read_frame(stream, process, options));

    if (_check_process_status(process, stream))
        goto error;

    raw_image_t* raw_image = _init_raw_image(process, stream, options);
    if (!raw_image)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_raw_image | " ERROR_FAILED_TO_INIT_RAW_IMAGE "\n");
        goto error;
    }

    if (_scale_image(raw_image, options))
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_raw_image | " ERROR_FAILED_TO_SCALE_IMAGE "\n");
        goto error;
    }

    free_process(process);
    free_stream(stream);
    return raw_image;

error:
    free_process(process);
    free_stream(stream);
    return NULL;
}
