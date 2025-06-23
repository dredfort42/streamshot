/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | png_image.c
    ::  ::          ::  ::    Created  | 2025-06-20
          ::::  ::::          Modified | 2025-06-20

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>

#include "errors.h"
#include "png.h"
#include "process.h"
#include "utilities.h"

/**
 * @brief Generates a PNG image from raw RGB data.
 *
 * This function creates a PNG image using the provided raw RGB pixel data.
 * It constructs the appropriate PNG header and concatenates it with the
 * pixel data to produce a complete PNG image.
 *
 * @param data     Pointer to the raw RGB pixel data.
 * @param size     Size of the raw data in bytes.
 * @param width    Width of the image in pixels.
 * @param height   Height of the image in pixels.
 * @param quality  PNG compression quality (0-100).
 *
 * @return         Pointer to the newly allocated PNG image on success, or NULL on failure.
 *
 * @note           The caller is responsible for freeing the returned buffer.
 */
image_t* get_png_image(const uint8_t* data, size_t size, int width, int height, short quality)
{
    if (!data || size == 0 || width <= 0 || height <= 0 || quality < 0 || quality > 100)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_png_image | " ERROR_INVALID_ARGUMENTS "\n");
        return NULL;
    }

    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    image_t* img = NULL;
    FILE* memfp = NULL;
    uint8_t* png_buffer = NULL;
    size_t png_size = 0;
    png_bytep* row_pointers = NULL;

    row_pointers = malloc(height * sizeof(png_bytep));
    if (!row_pointers)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_png_image | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        return NULL;
    }

    for (int y = 0; y < height; y++)
        row_pointers[y] = (png_bytep)(data + y * width * RGB_BYTES_PER_PIXEL);

    memfp = open_memstream((char**)&png_buffer, &png_size);
    if (!memfp)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) get_png_image | " ERROR_FAILED_TO_OPEN_MEMORY_STREAM "\n");
        goto error;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) get_png_image | " ERROR_FAILED_TO_CREATE_PNG_WRITE_STRUCT "\n");

        goto error;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) get_png_image | " ERROR_FAILED_TO_CREATE_PNG_INFO_STRUCT "\n");
        goto error;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_png_image | " ERROR_LIBPNG_ERROR "\n");
        goto error;
    }

    png_init_io(png_ptr, memfp);

    int png_compression = (int)((100 - quality) * 9 / 100);
    png_set_compression_level(png_ptr, png_compression);

    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    free(row_pointers);
    row_pointers = NULL;
    fflush(memfp);
    fclose(memfp);

    if (!png_buffer || png_size == 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_png_image | " ERROR_FAILED_TO_ENCODE_IMAGE "\n");
        goto error;
    }

    img = (image_t*)malloc(sizeof(image_t));
    if (!img)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_png_image | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        free(png_buffer);
        goto error;
    }

    img->data = png_buffer;
    img->size = png_size;
    img->width = width;
    img->height = height;

    return img;
error:
    if (row_pointers)
    {
        free(row_pointers);
        row_pointers = NULL;
    }
    // Do not free png_buffer here;
    // it is managed by open_memstream and will be freed by fclose(memfp)
    if (memfp)
        fclose(memfp);

    if (png_ptr)
    {
        if (!info_ptr)
            png_destroy_write_struct(&png_ptr, NULL);
        else
            png_destroy_info_struct(png_ptr, &info_ptr);
    }

    return NULL;
}
