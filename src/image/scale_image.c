/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | get_scaled_image.c
    ::  ::          ::  ::    Created  | 2025-06-19
          ::::  ::::          Modified | 2025-06-19

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "libswscale/swscale.h"
#include "options.h"
#include "process.h"
#include "utilities.h"

float _get_scale_factor(int src_width, int src_height, const options_t* options)
{
    if (src_width <= 0 || src_height <= 0 || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _get_scale_factor | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    if (options->resize_width <= 0 && options->resize_height <= 0)
    {
        float sf = options->scale_factor;
        if (sf > MAX_SCALE_FACTOR)
            sf = MAX_SCALE_FACTOR;
        if (sf < MIN_SCALE_FACTOR)
            sf = MIN_SCALE_FACTOR;

        if (options->debug)
            printf(ANSI_BLUE "Debug:" ANSI_RESET " Scale factor: %.2f\n", sf);

        return sf;
    }

    double scale_w = 1.0, scale_h = 1.0;
    if (options->resize_width > 0)
        scale_w = (double)options->resize_width / src_width;
    if (options->resize_height > 0)
        scale_h = (double)options->resize_height / src_height;

    float sf = (options->resize_width > 0 && options->resize_height > 0)
                   ? (scale_w < scale_h ? scale_w : scale_h)
                   : (options->resize_width > 0 ? scale_w : scale_h);

    if (sf > MAX_SCALE_FACTOR)
        sf = MAX_SCALE_FACTOR;
    if (sf < MIN_SCALE_FACTOR)
        sf = MIN_SCALE_FACTOR;

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Scale factor: %.2f\n", sf);

    return sf;
}

short _scale_image(raw_image_t* raw_image, const options_t* options)
{
    if (!raw_image || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _scale_image | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    if (raw_image->width <= 0 || raw_image->height <= 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _scale_image | " ERROR_INVALID_IMAGE_SIZE "\n");
        return RTN_ERROR;
    }

    float scale_factor = _get_scale_factor(raw_image->width, raw_image->height, options);
    if (scale_factor < 0 || scale_factor > MAX_SCALE_FACTOR)
        return RTN_ERROR;
    else if (scale_factor == 1.0)
        return RTN_SUCCESS;

    int dst_width = (int)(raw_image->width * scale_factor);
    int dst_height = (int)(raw_image->height * scale_factor);
    if (dst_width <= 0 || dst_height <= 0)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _scale_image | " ERROR_INVALID_DESTINATION_DIMENSIONS "\n");
        return RTN_ERROR;
    }

    if (dst_width < MIN_RESIZE_WIDTH || dst_width > MAX_RESIZE_WIDTH ||
        dst_height < MIN_RESIZE_HEIGHT || dst_height > MAX_RESIZE_HEIGHT)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _scale_image | " ERROR_INVALID_DESTINATION_DIMENSIONS "\n");
        return RTN_ERROR;
    }

    size_t dst_size = dst_width * dst_height * RGB_BYTES_PER_PIXEL;

    if (options->debug)
    {
        printf(ANSI_BLUE "Debug:" ANSI_RESET
                         " Scaled image to size: %zu bytes, width: %d pixels, height: %d pixels\n",
               dst_size, dst_width, dst_height);
    }

    uint8_t* dst_data = (uint8_t*)malloc(dst_width * dst_height * RGB_BYTES_PER_PIXEL);
    if (!dst_data)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _scale_image | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        goto error;
    }

    struct SwsContext* scale_context =
        sws_getContext(raw_image->width, raw_image->height, AV_PIX_FMT_RGB24, dst_width, dst_height,
                       AV_PIX_FMT_RGB24, SWS_LANCZOS, NULL, NULL, NULL);
    if (!scale_context)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _scale_image | " ERROR_FAILED_TO_CREATE_SWS_CONTEXT "\n");
        goto error;
    }

    const uint8_t* src_slices[1] = {raw_image->data};
    int src_strides[1] = {raw_image->width * RGB_BYTES_PER_PIXEL};
    uint8_t* dst_slices[1] = {dst_data};
    int dst_strides[1] = {dst_width * RGB_BYTES_PER_PIXEL};
    int scaled = sws_scale(scale_context, src_slices, src_strides, 0, raw_image->height, dst_slices,
                           dst_strides);
    sws_freeContext(scale_context);
    if (scaled != dst_height)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _scale_image | " ERROR_FAILED_TO_SCALE_IMAGE "\n");
        goto error;
    }

    free(raw_image->data);

    raw_image->data = dst_data;
    raw_image->width = dst_width;
    raw_image->height = dst_height;
    raw_image->size = dst_size;

    if (options->debug)
    {
        char path[256];
        snprintf(path, sizeof(path), "%s/raw_scaled_image.ppm", options->debug_dir);
        if (!save_ppm(path, raw_image->data, raw_image->size, raw_image->width, raw_image->height))
            printf(ANSI_BLUE "Debug:" ANSI_RESET " Saved scaled image to: %s\n", path);
    }

    return RTN_SUCCESS;

error:
    if (dst_data)
        free(dst_data);

    return RTN_ERROR;
}