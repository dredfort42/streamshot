/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | validate_options.c
    ::  ::          ::  ::    Created  | 2025-06-09
          ::::  ::::          Modified | 2025-06-09

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "options.h"

short _validate_rtsp_url(const char* rtsp_url)
{
    if (!rtsp_url || strlen(rtsp_url) < 8 || strncmp(rtsp_url, "rtsp://", 7) != 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) validate_rtsp_url | " ERROR_INVALID_RTSP_URL);
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

short _validate_timeout_sec(int timeout_sec)
{
    if (timeout_sec < 1 || timeout_sec > MAX_TIMEOUT_SEC)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) validate_timeout_sec | " ERROR_INVALID_TIMEOUT);
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

short _validate_output_file_path(const char* output_file_path)
{
    if (output_file_path && strlen(output_file_path) < 3)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) validate_output_file_path | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

short _validate_output_file_fd(int output_file_fd)
{
    if (output_file_fd != -1 && output_file_fd < MIN_OUTPUT_FD)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) validate_output_file_fd | " ERROR_INVALID_OUTPUT_FD);
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

short _validate_exposure_sec(int exposure_sec)
{
    if (exposure_sec < 0 || exposure_sec > MAX_EXPOSURE_SEC)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) validate_exposure_sec | " ERROR_INVALID_EXPOSURE);
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

short _validate_output_format(image_format_t output_format)
{
    if (output_format == IMAGE_FORMAT_UNKNOWN)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) validate_output_format | " ERROR_INVALID_OUTPUT_FORMAT);
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

short _validate_scale_factor(float scale_factor)
{
    if (scale_factor < MIN_SCALE_FACTOR || scale_factor > MAX_SCALE_FACTOR)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) validate_scale_factor | " ERROR_INVALID_SCALE_FACTOR);
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

short _validate_resize_height(int resize_height)
{
    if (resize_height && (resize_height < MIN_RESIZE_HEIGHT || resize_height > MAX_RESIZE_HEIGHT))
    {
        write_msg_to_fd(STDERR_FILENO, "(f) validate_resize_height | " ERROR_INVALID_RESIZE_HEIGHT);
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

short _validate_resize_width(int resize_width)
{
    if (resize_width && (resize_width < MIN_RESIZE_WIDTH || resize_width > MAX_RESIZE_WIDTH))
    {
        write_msg_to_fd(STDERR_FILENO, "(f) validate_resize_width | " ERROR_INVALID_RESIZE_WIDTH);
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

short _validate_image_quality(int image_quality)
{
    if (image_quality < MIN_IMAGE_QUALITY || image_quality > MAX_IMAGE_QUALITY)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) validate_image_quality | " ERROR_INVALID_IMAGE_QUALITY);
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

short _validate_debug_step(int debug_step)
{
    if (debug_step < 1)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) validate_debug_step | " ERROR_INVALID_DEBUG_STEP);
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

short _validate_debug_dir(const char* debug_dir)
{
    if (!debug_dir || strlen(debug_dir) < 3)
        goto error;

    struct stat st = {0};
    if (stat(debug_dir, &st) == -1)
    {
        if (strcmp(debug_dir, DEFAULT_DEBUG_DIR) == 0)
        {
            if (mkdir(debug_dir, 0755) == -1)
            {
                write_msg_to_fd(STDERR_FILENO, "(f) validate_debug_dir | " ERROR_FAILED_TO_CREATE_DEBUG_DIR);
                return RTN_ERROR;
            }
        }
        else
            goto error;
    }

    return RTN_SUCCESS;

error:
    write_msg_to_fd(STDERR_FILENO, "(f) validate_debug_dir | " ERROR_INVALID_DEBUG_DIR);
    return RTN_ERROR;
}

/**
 * @brief Validates the provided options structure.
 *
 * This function checks the integrity and correctness of the fields within the
 * given options_t structure. It ensures that all required options are set and
 * that their values are within acceptable ranges or formats.
 *
 * @param options Pointer to the options_t structure to be validated.
 *
 * @return 0 if the options are valid, or -1 indicating
 *         the type of validation error encountered.
 */
short validate_options(const options_t* options)
{
    if (!options)
        return RTN_ERROR;

    short result = 0;

    result |= _validate_rtsp_url(options->rtsp_url);
    result |= _validate_timeout_sec(options->timeout_sec);
    result |= _validate_output_file_path(options->output_file_path);
    result |= _validate_output_file_fd(options->output_file_fd);
    result |= _validate_exposure_sec(options->exposure_sec);
    result |= _validate_output_format(options->output_format);
    result |= _validate_scale_factor(options->scale_factor);
    result |= _validate_resize_height(options->resize_height);
    result |= _validate_resize_width(options->resize_width);
    result |= _validate_image_quality(options->image_quality);
    if (options->debug)
    {
        result |= _validate_debug_step(options->debug_step);
        result |= _validate_debug_dir(options->debug_dir);
    }

    return (result) ? RTN_ERROR : RTN_SUCCESS;
}