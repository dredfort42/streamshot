/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | init_process.c
    ::  ::          ::  ::    Created  | 2025-06-16
          ::::  ::::          Modified | 2025-06-16

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "libavutil/imgutils.h"
#include "process.h"
#include "stream.h"
#include "utilities.h"

process_t* _init_process(stream_t* stream, const options_t* options)
{
    if (!stream || !stream->codec_context || stream->video_stream_index < 0 || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_process | " ERROR_INVALID_ARGUMENTS "\n");

        return NULL;
    }

    process_t* process = (process_t*)malloc(sizeof(process_t));
    if (!process)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_process | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        goto error;
    }

    process->video_frame = av_frame_alloc();
    if (!process->video_frame)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_process | " ERROR_FAILED_TO_ALLOCATE_VIDEO_FRAME "\n");
        goto error;
    }

    process->image_frame = av_frame_alloc();
    if (!process->image_frame)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_process | " ERROR_FAILED_TO_ALLOCATE_IMAGE_FRAME "\n");
        goto error;
    }

    process->image_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, stream->codec_context->width, stream->codec_context->height, 1);
    if (process->image_size < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_process | " ERROR_FAILED_TO_GET_IMAGE_SIZE "\n");
        goto error;
    }

    process->buffer = (uint8_t*)av_malloc(process->image_size * sizeof(uint8_t));
    if (!process->buffer)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_process | " ERROR_FAILED_TO_ALLOCATE_BUFFER "\n");
        goto error;
    }

    if ((ssize_t)process->image_size != (ssize_t)av_image_fill_arrays(process->image_frame->data, process->image_frame->linesize, process->buffer,
                                                                      AV_PIX_FMT_RGB24, stream->codec_context->width, stream->codec_context->height, 1))
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_process | " ERROR_FAILED_TO_FILL_IMAGE_ARRAYS "\n");
        goto error;
    }

    process->pixel_sum_buffer = (unsigned long long*)calloc(process->image_size, sizeof(unsigned long long));
    if (!process->pixel_sum_buffer)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_process | " ERROR_FAILED_TO_ALLOCATE_BUFFER "\n");
        goto error;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Initialized image processing structures for video stream index: %d\n", stream->video_stream_index);

    return process;

error:
    free_process(process);

    return NULL;
}