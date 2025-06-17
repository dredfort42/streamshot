/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | camera_connecor.c
    ::  ::          ::  ::    Created  | 2025-06-14
          ::::  ::::          Modified | 2025-06-14

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
    stream->video_stream_index = -1;  // Initialize video stream index to -1 (not found)
    stream->codec_context = NULL;
    stream->sws_context = NULL;
    stream->number_of_frames_to_read = 0;
    stream->stop_reading_at = 0;

    return stream;
}

stream_t* get_stream(options_t* options)
{
    stream_t* stream = _init_stream();

    if (_set_stream_options(stream, options) || _open_stream(stream, options) ||
        _init_codec_context(stream, options) ||
        _init_sws_context(stream, options, DEFAULT_SCALE_FACTOR))
        return NULL;

    return stream;
}

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
}
