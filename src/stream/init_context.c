/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | init_context.c
    ::  ::          ::  ::    Created  | 2025-06-16
          ::::  ::::          Modified | 2025-06-16

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "stream.h"
#include "utilities.h"

/**
 * @brief Initializes the codec context for the given stream.
 *
 * This function sets up the codec context for decoding video streams using the provided
 * stream and options structures. It performs validation of input arguments, locates the
 * appropriate decoder, allocates and configures the codec context, and opens the codec.
 * On failure, an error message is written to STDERR and releases any allocated resources.
 *
 * @param stream   Pointer to the stream_t structure containing stream information.
 * @param options  Pointer to the options_t structure containing configuration options.
 *
 * @return 0 on success, -1 on failure.
 */
short _init_codec_context(stream_t* stream, const options_t* options)
{
    if (!stream || !options || stream->video_stream_index < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_codec_context | " ERROR_INVALID_ARGUMENTS "\n");
        goto error;
    }

    if (!stream->format_context || !stream->format_context->nb_streams)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_codec_context | " ERROR_NO_STREAMS_FOUND "\n");
        goto error;
    }

    if ((long)stream->video_stream_index >= (long)stream->format_context->nb_streams)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _init_codec_context | " ERROR_NO_VIDEO_STREAM_FOUND "\n");
        goto error;
    }

    AVStream* av_stream = stream->format_context->streams[stream->video_stream_index];
    AVCodecParameters* codecpar = av_stream->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_codec_context | " ERROR_NO_DECODER_FOUND "\n");
        goto error;
    }

    stream->codec_context = avcodec_alloc_context3(codec);
    if (!stream->codec_context)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _init_codec_context | " ERROR_FAILED_TO_ALLOCATE_CODEC_CONTEXT "\n");
        goto error;
    }

    if (avcodec_parameters_to_context(stream->codec_context, codecpar) < 0)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _init_codec_context | " ERROR_FAILED_TO_COPY_CODEC_PARAMETERS "\n");
        goto error;
    }

    if (avcodec_open2(stream->codec_context, codec, NULL) < 0)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _init_codec_context | " ERROR_FAILED_TO_OPEN_CODEC "\n");
        goto error;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Codec opened successfully: %s\n", codec->name);

    return RTN_SUCCESS;

error:
    if (stream && stream->codec_context)
    {
        avcodec_free_context(&stream->codec_context);
        stream->codec_context = NULL;
    }

    return RTN_ERROR;
}

/**
 * @brief Initializes the SwsContext for scaling and converting pixel formats.
 *
 * This function sets up the SwsContext for converting the video stream's pixel format
 * to RGB24 format, applying a scaling factor if specified. It performs validation of
 * input arguments and handles errors appropriately.
 *
 * @param stream       Pointer to the stream_t structure containing stream information.
 * @param options      Pointer to the options_t structure containing configuration options.
 * @param scale_factor Scaling factor for resizing the video frames.
 *
 * @return 0 on success, -1 on failure.
 */
short _init_sws_context(stream_t* stream, const options_t* options, float scale_factor)
{
    if (!stream || !options || stream->video_stream_index < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_sws_context | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    if (!stream->format_context ||
        (long)stream->video_stream_index >= (long)stream->format_context->nb_streams)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_sws_context | " ERROR_NO_VIDEO_STREAM_FOUND "\n");
        return RTN_ERROR;
    }

    AVStream* av_stream = stream->format_context->streams[stream->video_stream_index];
    AVCodecParameters* codecpar = av_stream->codecpar;
    if (!codecpar)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _init_sws_context | " ERROR_NO_CODEC_PARAMETERS_FOUND "\n");
        return RTN_ERROR;
    }

    enum AVPixelFormat src_pix_fmt = codecpar->format;
    enum AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB24;
    int dst_width = (int)(codecpar->width * scale_factor);
    int dst_height = (int)(codecpar->height * scale_factor);

    if (dst_width <= 0 || dst_height <= 0)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _init_sws_context | " ERROR_INVALID_DESTINATION_DIMENSIONS "\n");
        return RTN_ERROR;
    }

    stream->sws_context =
        sws_getContext(codecpar->width, codecpar->height, src_pix_fmt, dst_width, dst_height,
                       dst_pix_fmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);

    if (!stream->sws_context)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _init_sws_context | " ERROR_FAILED_TO_CREATE_SWS_CONTEXT "\n");
        return RTN_ERROR;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Initialized SwsContext for video stream index: %d\n",
               stream->video_stream_index);

    return RTN_SUCCESS;
}