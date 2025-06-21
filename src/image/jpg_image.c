/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | jpg_image.cc
    ::  ::          ::  ::    Created  | 2025-06-20
          ::::  ::::          Modified | 2025-06-20

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "errors.h"
#include "jpeglib.h"
#include "process.h"
#include "utilities.h"

/**
 * @brief Generates a JPEG image from raw RGB data.
 *
 * This function creates a JPEG image using the provided raw RGB pixel data.
 * It constructs the appropriate JPEG header and concatenates it with the
 * pixel data to produce a complete JPEG image.
 *
 * @param data     Pointer to the raw RGB pixel data.
 * @param size     Size of the raw data in bytes.
 * @param width    Width of the image in pixels.
 * @param height   Height of the image in pixels.
 * @param quality  JPEG compression quality (0-100).
 *
 * @return         Pointer to the newly allocated JPEG image on success, or NULL on failure.
 *
 * @note           The caller is responsible for freeing the returned buffer.
 */
image_t* get_jpg_image(const uint8_t* data, size_t size, int width, int height, short quality)
{
    if (!data || size == 0 || width <= 0 || height <= 0 || quality < 0 || quality > 100)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_jpg_image | " ERROR_INVALID_ARGUMENTS "\n");
        return NULL;
    }

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    unsigned char* jpeg_buf = NULL;
    unsigned long jpeg_size = 0;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_mem_dest(&cinfo, &jpeg_buf, &jpeg_size);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = RGB_BYTES_PER_PIXEL;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);

    jpeg_start_compress(&cinfo, TRUE);
    int row_stride = width * RGB_BYTES_PER_PIXEL;
    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = (unsigned char*)&data[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    if (!jpeg_buf || jpeg_size == 0)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) get_jpg_image | " ERROR_FAILED_TO_CREATE_JPEG_COMPRESSION "\n");
        return NULL;
    }

    image_t* jpg_image = (image_t*)malloc(sizeof(image_t));
    if (!jpg_image)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_jpg_image | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        free(jpeg_buf);
        return NULL;
    }

    jpg_image->data = jpeg_buf;
    jpg_image->size = jpeg_size;
    jpg_image->width = width;
    jpg_image->height = height;

    return jpg_image;
}
