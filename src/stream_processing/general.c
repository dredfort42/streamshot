#include "stream_process.h"

typedef struct stream_s
{
    AVDictionary* options;            // Options for the RTSP stream (e.g., timeout).
    AVFormatContext* format_context;  // Format context for the RTSP stream.
    int video_stream_index;           // Index of the video stream in the format context.

    AVCodecContext* codec_context;          // Codec context for decoding the video stream.
    struct SwsContext* sws_context;         // SwsContext for scaling and converting pixel formats.
    unsigned int number_of_frames_to_read;  // Number of frames to read from the stream.
    long long stop_reading_at;              // Timestamp to stop reading frames (in microseconds).

    // AVFrame* frame;                       // Frame for storing decoded video frames.
    // size_t frame_size;                    // Size of the frame in bytes.
    // uint8_t* frame_data;                  // Pointer to the frame data buffer.
    // AVStream* video_stream;               // Pointer to the video stream in the format context.
    // double fps;                           // Frame rate of the video stream (frames per second).

} stream_t;

stream_t* _init_stream()
{
    stream_t* stream = (stream_t*)malloc(sizeof(stream_t));
    if (!stream)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_stream | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
        return NULL;
    }

    stream->options = NULL;
    stream->format_context = NULL;
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

    char timeout_str[32];
    snprintf(timeout_str, sizeof(timeout_str), "%d", options->timeout_sec * 1000000);

    short error = 0;
    error |= av_dict_set(&stream->options, "rtsp_transport", "tcp", 0) < 0;
    error |= av_dict_set(&stream->options, "timeout", timeout_str, 0) < 0;

    if (options->debug)
        error |= av_dict_set(&stream->options, "debug", "qp+mv", 0) < 0;

    if (error)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _set_stream_options | " ERROR_FAILED_TO_SET_STREAM_OPTIONS);
        return RTN_ERROR;
    }
    else if (options->debug)
    {
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Printing RTSP stream options:\n");

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

short _init_codec_context(stream_t* stream, const options_t* options)
{
    if (!stream || !options || stream->video_stream_index < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_codec_context | " ERROR_INVALID_ARGUMENTS);
        goto error;
    }

    // Find and open video decoder
    AVCodecParameters* codecpar = stream->format_context->streams[stream->video_stream_index]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_codec_context | Decoder not found.\n");
        goto error;
    }

    stream->codec_context = avcodec_alloc_context3(codec);
    if (!stream->codec_context)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_codec_context | Could not allocate codec context.\n");
        goto error;
    }

    if (avcodec_parameters_to_context(stream->codec_context, codecpar) < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_codec_context | Could not copy codec parameters.\n");
        goto error;
    }

    if (avcodec_open2(stream->codec_context, codec, NULL) < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_codec_context | Cannot open codec.\n");
        goto error;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Codec opened successfully: %s\n", codec->name);

    return RTN_SUCCESS;

error:
    if (stream->codec_context)
        avcodec_free_context(&stream->codec_context);
    if (stream->format_context)
        avformat_close_input(&stream->format_context);
    if (stream->options)
        av_dict_free(&stream->options);
    return RTN_ERROR;
}

short _init_sws_context(stream_t* stream, const options_t* options, float scale_factor)
{
    if (!stream || !options || stream->video_stream_index < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_sws_context | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    AVStream* video_stream = stream->format_context->streams[stream->video_stream_index];
    enum AVPixelFormat src_pix_fmt = video_stream->codecpar->format;
    enum AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB24;

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

long long _time_now_microseconds()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec) * 1000000 + (long long)(tv.tv_usec);
}

short _set_stop_reading_at(stream_t* stream, const options_t* options)
{
    if (!stream || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _set_stop_reading_at | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    if (options->exposure_sec > 0)
        stream->stop_reading_at = _time_now_microseconds() + (long long)((options->exposure_sec + options->timeout_sec) * 1000000);
    else
        stream->stop_reading_at = _time_now_microseconds() + (long long)(options->timeout_sec * 2 * 1000000);

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Stop reading at: %lld microseconds\n", stream->stop_reading_at);

    return RTN_SUCCESS;
}

short get_streamshot(options_t* options)
{
    if (!options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    stream_t* stream = _init_stream();
    if (!stream)
        return RTN_ERROR;

    if (_set_stream_options(stream, options) || _open_stream(stream, options) || _init_codec_context(stream, options) ||
        _init_sws_context(stream, options, DEFAULT_SCALE_FACTOR) || _calculate_number_of_frames_to_read(stream, options))
        return RTN_ERROR;

    // Allocate frames and buffers
    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    AVFrame* rgb_frame = av_frame_alloc();
    size_t num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, stream->codec_context->width, stream->codec_context->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t));
    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, buffer, AV_PIX_FMT_RGB24, stream->codec_context->width, stream->codec_context->height, 1);

    unsigned long long frame_count = 0;
    short got_first_i_frame = 0;

    // Allocate buffer for summing pixel values
    unsigned long long* sum_buffer = (unsigned long long*)calloc(num_bytes, sizeof(unsigned long long));
    if (!sum_buffer)
    {
        return 1;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Starting to read %u frames from RTSP stream...\n", stream->number_of_frames_to_read);

    if (_set_stop_reading_at(stream, options) != RTN_SUCCESS)
    {
        return RTN_ERROR;
    }
    // Main loop: read and process frames
    int read_result = 0;
    while ((read_result = av_read_frame(stream->format_context, pkt)) >= 0 && frame_count < stream->number_of_frames_to_read)
    {
        if (pkt->stream_index == stream->video_stream_index)
        {
            if (avcodec_send_packet(stream->codec_context, pkt) == 0)
            {
                while (avcodec_receive_frame(stream->codec_context, frame) == 0)
                {
                    // Wait for first I-frame (key frame)
                    if (!got_first_i_frame)
                    {
                        if (frame->pict_type == AV_PICTURE_TYPE_I)
                            got_first_i_frame = 1;
                        else
                            continue;
                    }

                    // Convert frame to RGB
                    sws_scale(stream->sws_context, (const uint8_t* const*)frame->data, frame->linesize, 0, stream->codec_context->height, rgb_frame->data,
                              rgb_frame->linesize);

                    // Sum pixel values
                    uint8_t* data = rgb_frame->data[0];
                    for (size_t i = 0; i < num_bytes; ++i) sum_buffer[i] += data[i];

                    if (options->debug && (frame_count == 0 || (frame_count + 1) % 100 == 0 || frame_count == stream->number_of_frames_to_read - 1))
                    {
                        char file_name[1024];
                        snprintf(file_name, sizeof(file_name), "%s/frame_%010llu.ppm", options->debug_dir, frame_count + 1);

                        // Save current frame to PPM file (for debugging)
                        FILE* f = fopen(file_name, "wb");
                        if (f)
                        {
                            char header[32];
                            snprintf(header, sizeof(header), "P6\n%d %d\n255\n", stream->codec_context->width, stream->codec_context->height);
                            fwrite(header, 1, strlen(header), f);
                            fwrite(data, 1, num_bytes, f);
                            fclose(f);
                            printf("Saved %s\n", file_name);
                        }
                    }

                    // If last frame, compute average and write result
                    if (frame_count == stream->number_of_frames_to_read - 1)
                    {
                        if (options->debug)
                            printf("Computing average frame...\n");

                        uint8_t* avg_buffer = (uint8_t*)malloc(num_bytes);
                        if (!avg_buffer)
                        {
                            write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
                            break;
                        }
                        for (size_t i = 0; i < num_bytes; ++i) avg_buffer[i] = (uint8_t)(sum_buffer[i] / stream->number_of_frames_to_read);

                        char header[32];
                        snprintf(header, sizeof(header), "P6\n%d %d\n255\n", stream->codec_context->width, stream->codec_context->height);

                        // if (options->debug)
                        // {
                        // Save to PPM file (for debugging)
                        char file_name[1024];
                        snprintf(file_name, sizeof(file_name), "%s/average_frame.ppm", options->debug_dir);
                        FILE* f = fopen(file_name, "wb");
                        if (f)
                        {
                            fwrite(header, 1, strlen(header), f);
                            fwrite(avg_buffer, 1, num_bytes, f);
                            fclose(f);
                            if (options->debug)
                                printf("Saved %s\n", file_name);
                        }
                        // }

                        // // Write average frame to output fd
                        // write_data_to_fd(data_fd, (const uint8_t*)header, strlen(header));
                        // write_data_to_fd(data_fd, avg_buffer, num_bytes);
                        // if (options->debug)
                        //     printf("Average frame written to data_fd %d\n", data_fd);

                        free(avg_buffer);
                    }
                    frame_count++;
                }
            }
        }
        av_packet_unref(pkt);
    }

    // Check if connection was interrupted
    if (frame_count < stream->number_of_frames_to_read)
    {
        char error_msg[256];
        if (read_result < 0)
        {
            snprintf(error_msg, sizeof(error_msg),
                     "Camera connection lost or error occurred (av_read_frame "
                     "returned %d; processed %llu out of %u frames).\n",
                     read_result, frame_count, stream->number_of_frames_to_read);
        }
        else
        {
            snprintf(error_msg, sizeof(error_msg),
                     "Stream ended before enough frames were processed (%llu out of "
                     "%u frames).\n",
                     frame_count, stream->number_of_frames_to_read);
        }
        write_msg_to_fd(STDERR_FILENO, error_msg);
    }

    // Cleanup
    av_free(buffer);
    av_frame_free(&rgb_frame);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    sws_freeContext(stream->sws_context);
    avformat_close_input(&stream->format_context);
    av_dict_free(&stream->options);
    avformat_network_deinit();

    if (frame_count < stream->number_of_frames_to_read)
        return 1;

    return 0;
}