/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | connector.c
    ::  ::          ::  ::    Created  | 2025-06-06
          ::::  ::::          Modified | 2025-06-06

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "reader.h"

stream_t* _init_stream()
{
    stream_t* stream = (stream_t*)malloc(sizeof(stream_t));
    if (!stream)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_stream | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
        return NULL;
    }

    memset(stream, 0, sizeof(*stream));

    stream->video_stream_index = -1;  // Initialize video stream index to -1 (not found)

    return stream;
}

short _set_stream_options(stream_t* stream, const options_t* options)
{
    if (!stream || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _set_stream_options | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    short error = 0;
    if (av_dict_set(&stream->options, "rtsp_transport", "tcp", 0) < 0)
        error++;

    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", options->timeout_sec * 1000000);
    if (av_dict_set(&stream->options, "timeout", timeout_str, 0) < 0)
        error++;

    if (options->debug && av_dict_set(&stream->options, "debug", "qp+mv", 0) < 0)
        error++;

    if (error)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _set_stream_options | " ERROR_FAILED_TO_SET_STREAM_OPTIONS);
        return RTN_ERROR;
    }
    else
    {
        printf(ANSI_BLUE "Debug:" ANSI_RESET " RTSP Stream Options:\n");

        AVDictionaryEntry* entry = NULL;
        while ((entry = av_dict_get(stream->options, "", entry, AV_DICT_IGNORE_SUFFIX)))
        {
            printf("%s = %s\n", entry->key, entry->value);
        }
    }

    return RTN_SUCCESS;
}

short _detect_video_stream(stream_t* stream)
{
    for (unsigned int i = 0; i < stream->format_context->nb_streams; ++i)
    {
        if (stream->format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            stream->video_stream_index = i;
            break;
        }
    }

    if (stream->video_stream_index == -1)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _open_stream | No video stream found.\n");
        return RTN_ERROR;
    }

    return RTN_SUCCESS;
}

short _open_stream(stream_t* stream, const options_t* options)
{
    if (!stream || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _open_stream | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    if (avformat_open_input(&stream->format_context, options->rtsp_url, NULL, &stream->options) < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _open_stream | Failed to open RTSP stream.\n");
        return RTN_ERROR;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Successfully opened RTSP stream: %s\n", options->rtsp_url);

    if (avformat_find_stream_info(stream->format_context, NULL) < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _open_stream | Failed to find stream information.\n");
        return RTN_ERROR;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Found information about %d streams in the RTSP stream.\n", stream->format_context->nb_streams);

    if (_detect_video_stream(stream) != RTN_SUCCESS)
        return RTN_ERROR;

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Detected video stream index: %d\n", stream->video_stream_index);

    return RTN_SUCCESS;
}

short _find_decoder(stream_t* stream, const options_t* options)
{
    if (!stream || !options || stream->video_stream_index < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _find_decoder | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    AVStream* video_stream = stream->format_context->streams[stream->video_stream_index];
    AVCodecParameters* codec_params = video_stream->codecpar;

    stream->codec = avcodec_find_decoder(codec_params->codec_id);
    if (!stream->codec)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _find_decoder | Failed to find decoder for video stream.\n");
        return RTN_ERROR;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Found decoder: %s for video stream index: %d\n", stream->codec->name, stream->video_stream_index);

    return RTN_SUCCESS;
}

short _init_codec_context(stream_t* stream, const options_t* options)
{
    if (!stream || !options || !stream->codec)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_codec_context | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    stream->codec_context = avcodec_alloc_context3(stream->codec);
    if (!stream->codec_context)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_codec_context | Failed to allocate codec context.\n");
        return RTN_ERROR;
    }

    if (avcodec_parameters_to_context(stream->codec_context, stream->format_context->streams[stream->video_stream_index]->codecpar) < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_codec_context | Failed to copy codec parameters to context.\n");
        return RTN_ERROR;
    }

    if (options->debug)
    {
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Initialized codec context for video stream index: %d\n", stream->video_stream_index);
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Codec context parameters:\n");
        av_dump_format(stream->format_context, 0, options->rtsp_url, 0);
    }

    return RTN_SUCCESS;
}

float _get_scale_factor(stream_t* stream, const options_t* options)
{
    if (!stream || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _get_scale_factor | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    float scale_factor = options->scale_factor;

    if (scale_factor == DEFAULT_SCALE_FACTOR)
    {
        if (options->resize_height > 0 && options->resize_width > 0)
        {
            float scale_height = (float)options->resize_height / stream->format_context->streams[stream->video_stream_index]->codecpar->height;
            float scale_width = (float)options->resize_width / stream->format_context->streams[stream->video_stream_index]->codecpar->width;
            scale_factor = fminf(scale_height, scale_width);
        }
        else if (options->resize_height > 0)
        {
            scale_factor = (float)options->resize_height / stream->format_context->streams[stream->video_stream_index]->codecpar->height;
        }
        else if (options->resize_width > 0)
        {
            scale_factor = (float)options->resize_width / stream->format_context->streams[stream->video_stream_index]->codecpar->width;
        }
        else if (scale_factor < MIN_SCALE_FACTOR || scale_factor > MAX_SCALE_FACTOR)
        {
            write_msg_to_fd(STDERR_FILENO, "(f) _get_scale_factor | " ERROR_INVALID_SCALE_FACTOR);
            return RTN_ERROR;
        }
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Scale factor calculated: %.2f\n", scale_factor);

    return scale_factor;
}

short _calculate_number_of_frames_to_read(stream_t* stream, const options_t* options)
{
    if (!stream || !options || options->exposure_sec < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _calculate_number_of_frames_to_read | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    if (options->exposure_sec == 0)
        stream->number_of_frames_to_read = 1;
    else
    {
        double fps;
        AVStream* video_stream = stream->format_context->streams[stream->video_stream_index];
        if (video_stream->avg_frame_rate.den && video_stream->avg_frame_rate.num)
            fps = av_q2d(video_stream->avg_frame_rate);
        else if (video_stream->r_frame_rate.den && video_stream->r_frame_rate.num)
            fps = av_q2d(video_stream->r_frame_rate);
        else
            fps = 25.0;

        if (fps <= 0)
        {
            write_msg_to_fd(STDERR_FILENO, "(f) _calculate_number_of_frames_to_read | Invalid frame rate.\n");
            return RTN_ERROR;
        }

        stream->number_of_frames_to_read = (unsigned int)(options->exposure_sec * fps);

        unsigned long long max_uint64 = ULLONG_MAX;
        unsigned long long max_uint8 = UINT8_MAX;
        unsigned long long safe_frame_limit = max_uint64 / max_uint8;
        if (stream->number_of_frames_to_read > safe_frame_limit)
            stream->number_of_frames_to_read = safe_frame_limit;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Calculated frames to read: %u\n", stream->number_of_frames_to_read);

    return RTN_SUCCESS;
}

short _init_sws_context(stream_t* stream, const options_t* options)
{
    if (!stream || !options || stream->video_stream_index < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_sws_context | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    AVStream* video_stream = stream->format_context->streams[stream->video_stream_index];
    enum AVPixelFormat src_pix_fmt = video_stream->codecpar->format;
    enum AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB24;
    float scale_factor = _get_scale_factor(stream, options);
    if (scale_factor < 0)
        return RTN_ERROR;

    stream->sws_context =
        sws_getContext(video_stream->codecpar->width, video_stream->codecpar->height, src_pix_fmt, video_stream->codecpar->width * scale_factor,
                       video_stream->codecpar->height * scale_factor, dst_pix_fmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    if (!stream->sws_context)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_sws_context | Failed to create SwsContext.\n");
        return RTN_ERROR;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Initialized SwsContext for video stream index: %d\n", stream->video_stream_index);

    return RTN_SUCCESS;
}

void _free_stream(stream_t* stream)
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

    memset(stream, 0, sizeof(*stream));

    free(stream);
}