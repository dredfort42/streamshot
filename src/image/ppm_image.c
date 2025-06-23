/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | ppm_image.c
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
 * @brief Generates a PPM (Portable Pixmap) image from raw RGB data.
 *
 * This function creates a PPM image in the "P6" binary format using the provided
 * raw RGB pixel data. It constructs the appropriate PPM header and concatenates
 * it with the pixel data to produce a complete PPM image.
 *
 * @param data     Pointer to the raw RGB pixel data.
 * @param size     Size of the raw data in bytes.
 * @param width    Width of the image in pixels.
 * @param height   Height of the image in pixels.
 *
 * @return         Pointer to the newly allocated PPM image on success, or NULL on failure.
 *
 * @note           The caller is responsible for freeing the returned buffer.
 */
image_t* get_ppm_image(const uint8_t* data, size_t size, int width, int height)
{
    if (!data || width <= 0 || height <= 0 ||
        size != (size_t)(width * height * RGB_BYTES_PER_PIXEL))
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_ppm_image | " ERROR_INVALID_ARGUMENTS "\n");
        return NULL;
    }

    char header[64];
    int header_len = snprintf(header, sizeof(header), "P6\n%d %d\n255\n", width, height);

    if (header_len < 0 || (size_t)header_len >= sizeof(header))
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_ppm_image | " ERROR_FAILED_TO_FORMAT_HEADER "\n");
        return NULL;
    }

    image_t* ppm_image = (image_t*)malloc(sizeof(image_t));
    if (!ppm_image)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_ppm_image | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        return NULL;
    }

    ppm_image->data = (uint8_t*)malloc(header_len + size);
    if (!ppm_image->data)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_ppm_image | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        free(ppm_image);
        return NULL;
    }

    memcpy(ppm_image->data, header, (size_t)header_len);
    memcpy(ppm_image->data + header_len, data, size);

    ppm_image->size = header_len + size;
    ppm_image->width = width;
    ppm_image->height = height;

    return ppm_image;
}
