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
            write_msg_to_fd(STDERR_FILENO, "(f) _calculate_number_of_frames_to_read | Invalid video_frame rate.\n");
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
        stream->stop_reading_at = _time_now_microseconds() + (long long)(options->exposure_sec * 2 * 1000000);
    else
        stream->stop_reading_at = _time_now_microseconds() + (long long)(options->timeout_sec * 2 * 1000000);

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Stop reading at: %lld microseconds\n", stream->stop_reading_at);

    return RTN_SUCCESS;
}

typedef struct image_process_s
{
    AVFrame* video_frame;                  // Pointer to the decoded video video_frame.
    AVFrame* image_frame;                  // Pointer to the RGB image video_frame.
    size_t image_size;                     // Size of the image in bytes (calculated based on width, height, and pixel format).
    uint8_t* buffer;                       // Pointer to the buffer for storing the RGB image data.
    unsigned long long* pixel_sum_buffer;  // Buffer for summing pixel values across multiple frames.

} image_process_t;

void free_image_process(image_process_t* img_proc)
{
    if (!img_proc)
        return;

    if (img_proc->video_frame)
        av_frame_free(&img_proc->video_frame);
    if (img_proc->image_frame)
        av_frame_free(&img_proc->image_frame);
    if (img_proc->buffer)
        av_free(img_proc->buffer);
    if (img_proc->pixel_sum_buffer)
        free(img_proc->pixel_sum_buffer);

    free(img_proc);
}

static image_process_t* _init_image_process(stream_t* stream, const options_t* options)
{
    if (!stream || !stream->codec_context || stream->video_stream_index < 0 || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_INVALID_ARGUMENTS);
        return NULL;
    }

    image_process_t* img_proc = (image_process_t*)malloc(sizeof(image_process_t));
    if (!img_proc)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
        return NULL;
    }

    img_proc->video_frame = av_frame_alloc();
    if (!img_proc->video_frame)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
        goto error;
    }

    img_proc->image_frame = av_frame_alloc();
    if (!img_proc->image_frame)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
        goto error;
    }

    img_proc->image_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, stream->codec_context->width, stream->codec_context->height, 1);
    if (img_proc->image_size < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_FAILED_TO_GET_IMAGE_SIZE);
    }

    img_proc->buffer = (uint8_t*)av_malloc(img_proc->image_size * sizeof(uint8_t));
    if (!img_proc->buffer)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
        goto error;
    }

    if ((ssize_t)img_proc->image_size != (ssize_t)av_image_fill_arrays(img_proc->image_frame->data, img_proc->image_frame->linesize, img_proc->buffer,
                                                                       AV_PIX_FMT_RGB24, stream->codec_context->width, stream->codec_context->height, 1))
        goto error;

    img_proc->pixel_sum_buffer = (unsigned long long*)calloc(img_proc->image_size, sizeof(unsigned long long));
    if (!img_proc->pixel_sum_buffer)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
        goto error;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Initialized image processing structures for video stream index: %d\n", stream->video_stream_index);

    return img_proc;

error:
    free_image_process(img_proc);
    return NULL;
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

    image_process_t* img_proc = _init_image_process(stream, options);
    (void)img_proc;
    // -------------

    AVPacket* av_packet = av_packet_alloc();

    unsigned long long frame_counter = 0;
    short got_first_i_frame = 0;

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Starting to read %u frames from RTSP stream...\n", stream->number_of_frames_to_read);

    if (_set_stop_reading_at(stream, options) != RTN_SUCCESS)
    {
        return RTN_ERROR;
    }
    // Main loop: read and process frames
    int read_status = 0;
    // while ((read_result = av_read_frame(stream->format_context, av_packet)) >= 0 && frame_counter < stream->number_of_frames_to_read)
    while ((read_status = av_read_frame(stream->format_context, av_packet)) >= 0 && frame_counter < stream->number_of_frames_to_read &&
           _time_now_microseconds() < stream->stop_reading_at)
    {
        if (av_packet->stream_index == stream->video_stream_index)
        {
            if (avcodec_send_packet(stream->codec_context, av_packet) == 0)
            {
                while (avcodec_receive_frame(stream->codec_context, img_proc->video_frame) == 0)
                {
                    // Wait for first I-frame (key frame)
                    if (!got_first_i_frame)
                    {
                        if (img_proc->video_frame->pict_type == AV_PICTURE_TYPE_I)
                            got_first_i_frame = 1;
                        else
                            continue;
                    }

                    // Convert video_frame to RGB
                    sws_scale(stream->sws_context, (const uint8_t* const*)img_proc->video_frame->data, img_proc->video_frame->linesize, 0,
                              stream->codec_context->height, img_proc->image_frame->data, img_proc->image_frame->linesize);

                    // Sum pixel values
                    // uint8_t* data = image_frame->data[0];
                    for (size_t i = 0; i < img_proc->image_size; ++i) img_proc->pixel_sum_buffer[i] += img_proc->image_frame->data[0][i];

                    if (options->debug && (frame_counter == 0 || (frame_counter + 1) % 100 == 0 || frame_counter == stream->number_of_frames_to_read - 1))
                    {
                        char file_name[1024];
                        snprintf(file_name, sizeof(file_name), "%s/frame_%010llu.ppm", options->debug_dir, frame_counter + 1);

                        // Save current video_frame to PPM file (for debugging)
                        FILE* f = fopen(file_name, "wb");
                        if (f)
                        {
                            char header[32];
                            snprintf(header, sizeof(header), "P6\n%d %d\n255\n", stream->codec_context->width, stream->codec_context->height);
                            fwrite(header, 1, strlen(header), f);
                            fwrite(img_proc->image_frame->data[0], 1, img_proc->image_size, f);
                            fclose(f);
                            printf("Saved %s\n", file_name);
                        }
                    }

                    // If last video_frame, compute average and write result
                    if (frame_counter == stream->number_of_frames_to_read - 1)
                    {
                        if (options->debug)
                            printf("Computing average video_frame...\n");

                        uint8_t* avg_buffer = (uint8_t*)malloc(img_proc->image_size);
                        if (!avg_buffer)
                        {
                            write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
                            break;
                        }
                        for (size_t i = 0; i < img_proc->image_size; ++i)
                            avg_buffer[i] = (uint8_t)(img_proc->pixel_sum_buffer[i] / stream->number_of_frames_to_read);

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
                            fwrite(avg_buffer, 1, img_proc->image_size, f);
                            fclose(f);
                            if (options->debug)
                                printf("Saved %s\n", file_name);
                        }
                        // }

                        // // Write average video_frame to output fd
                        // write_data_to_fd(data_fd, (const uint8_t*)header, strlen(header));
                        // write_data_to_fd(data_fd, avg_buffer, image_size);
                        // if (options->debug)
                        //     printf("Average video_frame written to data_fd %d\n", data_fd);

                        free(avg_buffer);
                    }
                    frame_counter++;
                }
            }
        }
        av_packet_unref(av_packet);
    }

    // Check if connection was interrupted
    if (frame_counter < stream->number_of_frames_to_read)
    {
        char error_msg[256];
        if (read_status < 0)
        {
            snprintf(error_msg, sizeof(error_msg),
                     "Camera connection lost or error occurred (av_read_frame "
                     "returned %d; processed %llu out of %u frames).\n",
                     read_status, frame_counter, stream->number_of_frames_to_read);
        }
        else
        {
            snprintf(error_msg, sizeof(error_msg),
                     "Stream ended before enough frames were processed (%llu out of "
                     "%u frames).\n",
                     frame_counter, stream->number_of_frames_to_read);
        }
        write_msg_to_fd(STDERR_FILENO, error_msg);
    }

    // Cleanup
    free_image_process(img_proc);
    av_packet_free(&av_packet);
    sws_freeContext(stream->sws_context);
    avformat_close_input(&stream->format_context);
    av_dict_free(&stream->options);

    if (frame_counter < stream->number_of_frames_to_read)
        return 1;

    return 0;
}