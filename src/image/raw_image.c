/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | raw_image.c
    ::  ::          ::  ::    Created  | 2025-06-19
          ::::  ::::          Modified | 2025-06-20

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "process.h"
#include "stream.h"
#include "utilities.h"

/**
 * @brief Initializes a image_t structure using the provided process, stream, and options.
 *
 * This function allocates and initializes a image_t object based on the image size
 * specified in the process, and the width and height from the stream's codec context.
 *
 * @param process   Pointer to the process_t structure containing image size and sum buffer.
 * @param stream    Pointer to the stream_t structure containing codec context and frame count.
 * @param options   Pointer to the options_t structure for debug output.
 *
 * @return          Pointer to the initialized image_t on success, or NULL on failure.
 */
image_t* _init_raw_image(const process_t* process, const stream_t* stream, const options_t* options)
{
    if (!process || !stream || !options || !stream->codec_context)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_raw_image | " ERROR_INVALID_ARGUMENTS "\n");
        return NULL;
    }

    image_t* raw_image = (image_t*)malloc(sizeof(image_t));
    if (!raw_image)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _init_raw_image | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        goto error;
    }

    raw_image->data = (uint8_t*)malloc(process->image_size);
    if (!raw_image->data)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _init_raw_image | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        goto error;
    }

    raw_image->size = process->image_size;
    raw_image->width = stream->codec_context->width;
    raw_image->height = stream->codec_context->height;

    if (stream->number_of_frames_to_read == 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_raw_image | " ERROR_NO_FRAMES_TO_READ "\n");
        goto error;
    }

    for (size_t i = 0; i < process->image_size; ++i)
        raw_image->data[i] = (uint8_t)(process->sum_buffer[i] / stream->number_of_frames_to_read);

    if (options->debug)
    {
        printf(ANSI_BLUE "Debug:" ANSI_RESET
                         " Raw image initialized with size: %zu bytes, "
                         "width: %d pixels, height: %d pixels\n",
               raw_image->size, raw_image->width, raw_image->height);

        char path[256];
        snprintf(path, sizeof(path), "%s/raw_average_image.ppm", options->debug_dir);
        if (!save_ppm(path, raw_image->data, raw_image->size, raw_image->width, raw_image->height))
            printf(ANSI_BLUE "Debug:" ANSI_RESET " Saved raw average image to: %s\n", path);
    }

    return raw_image;

error:
    free_image(raw_image);
    return NULL;
}

/**
 * @brief Frees the memory allocated for a image_t structure and its associated data buffer.
 *
 * This function safely deallocates the memory used by a image_t object, including its internal
 * data buffer, if present. If the provided pointer is NULL, the function does nothing.
 *
 * @param image Pointer to the image_t structure to be freed.
 */
void free_image(image_t* image)
{
    if (!image)
        return;

    if (image->data)
    {
        free(image->data);
        image->data = NULL;
    }

    free(image);
    image = NULL;
}