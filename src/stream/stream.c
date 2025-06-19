/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | stream.c
    ::  ::          ::  ::    Created  | 2025-06-14
          ::::  ::::          Modified | 2025-06-19

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "stream.h"

#include <unistd.h>

#include "errors.h"
#include "utilities.h"

short _set_stream_options(stream_t* stream, const options_t* options);
short _open_stream(stream_t* stream, const options_t* options);
short _init_codec_context(stream_t* stream, const options_t* options);
short _init_sws_context(stream_t* stream, const options_t* options, float scale_factor);

/**
 * @brief Initializes and allocates a new stream_t structure.
 *
 * Allocates memory for a new stream_t object and initializes its members to default values.
 * - Sets pointers to NULL.
 * - Sets video_stream_index to -1 to indicate it is not found.
 * - Initializes frame counters to 0.
 *
 * @return Pointer to the newly allocated stream_t structure, or NULL if memory allocation fails.
 */
stream_t* _init_stream()
{
    stream_t* stream = (stream_t*)malloc(sizeof(stream_t));
    if (!stream)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_stream | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        return NULL;
    }

    stream->options = NULL;
    stream->format_context = NULL;
    stream->video_stream_index = -1;
    stream->codec_context = NULL;
    stream->sws_context = NULL;
    stream->number_of_frames_to_read = 0;
    stream->stop_reading_at = 0;

    return stream;
}

/**
 * @brief Initializes and configures a new stream based on the provided options.
 *
 * This function allocates and initializes a new stream object, sets its options,
 * opens the stream, and initializes the codec and scaling contexts. If any step fails,
 * the function returns NULL.
 *
 * @param options Pointer to an options_t structure containing stream configuration parameters.
 *
 * @return Pointer to an initialized stream_t structure on success, or NULL on failure.
 */
stream_t* get_stream(options_t* options)
{
    stream_t* stream = _init_stream();

    if (_set_stream_options(stream, options) || _open_stream(stream, options) ||
        _init_codec_context(stream, options) ||
        _init_sws_context(stream, options, DEFAULT_SCALE_FACTOR))
        return NULL;

    return stream;
}

/**
 * @brief Frees all resources associated with a stream_t object.
 *
 * This function releases memory and closes any contexts associated with the given
 * stream_t pointer, including options, format context, codec context, and sws context.
 * After freeing all internal resources, it also frees the stream_t structure itself.
 *
 * @param stream Pointer to the stream_t object to be freed. If NULL, the function does nothing.
 */
void free_stream(stream_t* stream)
{
    if (!stream)
        return;

    if (stream->options)
        av_dict_free(&stream->options);
    if (stream->format_context)
        avformat_close_input(&stream->format_context);
    if (stream->codec_context)
        avcodec_free_context(&stream->codec_context);
    if (stream->sws_context)
        sws_freeContext(stream->sws_context);

    free(stream);
    stream = NULL;
}
