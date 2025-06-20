/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | convert_image.c
    ::  ::          ::  ::    Created  | 2025-06-20
          ::::  ::::          Modified | 2025-06-20

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdlib.h>
#include <unistd.h>

#include "errors.h"
#include "process.h"
#include "utilities.h"

/**
 * @brief Converts an input image to the specified output format.
 *
 * This function takes an input image and conversion options, checks for valid arguments,
 * and converts the image to the desired output format (JPG, JPEG, PNG, or PPM) with the specified
 * quality.
 *
 * @param options Pointer to options_t structure containing conversion options.
 * @param image Pointer to image_t structure representing the input image.
 *
 * @return Pointer to a newly allocated image_t structure in the desired format, or NULL on error.
 */
image_t* get_converted_image(options_t* options, image_t* image)
{
    if (!options || !image)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_converted_image | " ERROR_INVALID_ARGUMENTS "\n");
        return NULL;
    }

    if (image->width <= 0 || image->height <= 0)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) get_converted_image | " ERROR_INVALID_IMAGE_DIMENSIONS "\n");
        return NULL;
    }

    if (!image->data)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_converted_image | " ERROR_NO_DATA_TO_WRITE "\n");
        return NULL;
    }

    image_t* result = NULL;

    switch (options->output_format)
    {
        case IMAGE_FORMAT_JPG:
        case IMAGE_FORMAT_JPEG:
            result = get_jpg_image(image->data, image->size, image->width, image->height,
                                   options->image_quality);
            break;
        case IMAGE_FORMAT_PNG:
            result = get_png_image(image->data, image->size, image->width, image->height,
                                   options->image_quality);
            break;
        case IMAGE_FORMAT_PPM:
            result = get_ppm_image(image->data, image->size, image->width, image->height);
            break;
        case IMAGE_FORMAT_UNKNOWN:
        default:
            write_msg_to_fd(STDERR_FILENO,
                            "(f) get_converted_image | " ERROR_INVALID_OUTPUT_FORMAT "\n");
            result = NULL;
            break;
    }

    return result;
}
