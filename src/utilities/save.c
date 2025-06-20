/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | save.c
    ::  ::          ::  ::    Created  | 2025-06-17
          ::::  ::::          Modified | 2025-06-20

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "process.h"
#include "utilities.h"

/**
 * @brief Saves raw RGB image data as a PPM (Portable Pixmap) file.
 *
 * This function writes the provided RGB image data to a file in the P6 PPM format.
 * It constructs the appropriate PPM header and combines it with the image data before writing.
 *
 * @param path   The file path where the PPM image will be saved.
 * @param data   Pointer to the raw RGB image data.
 * @param size   The size of the image data in bytes.
 * @param width  The width of the image in pixels.
 * @param height The height of the image in pixels.
 *
 * @return 0 on success, -1 on failure.
 */
short save_ppm(const char* path, const uint8_t* data, size_t size, int width, int height)
{
    if (!path || !data || size == 0 || width <= 0 || height <= 0 ||
        size != (size_t)(width * height * RGB_BYTES_PER_PIXEL))
    {
        write_msg_to_fd(STDERR_FILENO, "(f) save_ppm | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    image_t* ppm_image = get_ppm_image(data, size, width, height);

    short ret =
        (write_data_to_file(path, ppm_image->data, ppm_image->size) < 0) ? RTN_ERROR : RTN_SUCCESS;

    free_image(ppm_image);
    return ret;
}
