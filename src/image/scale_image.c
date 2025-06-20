/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | scale_image.c
    ::  ::          ::  ::    Created  | 2025-06-19
          ::::  ::::          Modified | 2025-06-20

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "libswscale/swscale.h"
#include "process.h"
#include "utilities.h"

/**
 * Calculates the scale factor for resizing an image based on the provided source dimensions and
 * options.
 *
 * The function determines the appropriate scale factor to use for resizing an image, considering
 * the requested resize width, height, and/or a direct scale factor from the options. It ensures the
 * scale factor stays within the allowed minimum and maximum bounds. If both resize width and height
 * are specified, the smaller scale factor is chosen to maintain aspect ratio.
 *
 * @param src_width      The width of the source image (must be > 0).
 * @param src_height     The height of the source image (must be > 0).
 * @param options        Pointer to an options_t structure containing resize and scaling parameters.
 *
 * @return               The calculated scale factor as a float, or RTN_ERROR on invalid arguments.
 */
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

    double scale_w = DEFAULT_SCALE_FACTOR, scale_h = DEFAULT_SCALE_FACTOR;
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

/**
 * @brief Determines the appropriate scaling flags for the software scaler based on the provided
 * options.
 *
 * This function selects the scaling algorithm flags for image processing depending on the
 * image quality specified in the options. It also enables debug information if requested.
 *
 * @param options Pointer to an options_t structure containing image quality and debug settings.
 *
 * @return The selected scaling flags, or -1 if the options pointer is NULL.
 */
int _get_sws_flags(const options_t* options)
{
    if (!options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _get_sws_flags | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    int flags = 0;

    if (options->image_quality < QUALITY_FAST_BILINEAR)
        flags = SWS_FAST_BILINEAR;
    else if (options->image_quality < QUALITY_BILINEAR)
        flags = SWS_BILINEAR;
    else if (options->image_quality < QUALITY_BICUBIC)
        flags = SWS_BICUBIC;
    else if (options->image_quality < QUALITY_GAUSS)
        flags = SWS_GAUSS;
    else if (options->image_quality < QUALITY_LANCZOS)
        flags = SWS_LANCZOS;

    if (options->debug)
        flags |= SWS_PRINT_INFO;

    return flags;
}

/**
 * @brief Scales a raw RGB image according to the specified options.
 *
 * This function resizes a raw RGB image in-place using the scale factor or target dimensions
 * provided in the options structure. It uses libswscale for high-quality scaling and updates
 * the image_t structure with the new image data, dimensions, and size.
 *
 * @param raw_image Pointer to a image_t structure containing the image data to be scaled.
 * @param options   Pointer to an options_t structure specifying scaling parameters and debug
 * options.
 *
 * @return 0 on success, or -1 on failure.
 */
short _scale_image(image_t* raw_image, const options_t* options)
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
    else if (scale_factor == DEFAULT_SCALE_FACTOR)
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

    int sws_flags = _get_sws_flags(options);
    if (sws_flags < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _scale_image | " ERROR_INVALID_ARGUMENTS "\n");
        goto error;
    }

    struct SwsContext* scale_context =
        sws_getContext(raw_image->width, raw_image->height, AV_PIX_FMT_RGB24, dst_width, dst_height,
                       AV_PIX_FMT_RGB24, sws_flags, NULL, NULL, NULL);
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