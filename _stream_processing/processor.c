/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | processor.c
    ::  ::          ::  ::    Created  | 2025-06-06
          ::::  ::::          Modified | 2025-06-06

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "reader.h"

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

uint8_t* _process_streamshot(stream_t* stream, const options_t* options)
{
    if (!stream || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | " ERROR_INVALID_ARGUMENTS);
        goto end;
    }

    AVPacket* packet = av_packet_alloc();
    // AVFrame* frame = av_frame_alloc();
    // AVFrame* image = av_frame_alloc();
    // size_t image_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, stream->codec_context->width, stream->codec_context->height, 1);
    // uint8_t* buffer = (uint8_t*)av_malloc(image_size * sizeof(uint8_t));
    // av_image_fill_arrays(image->data, image->linesize, buffer, AV_PIX_FMT_RGB24, stream->codec_context->width, stream->codec_context->height, 1);
    // unsigned long long* accumulated_pixel_values = (unsigned long long*)calloc(image_size, sizeof(unsigned long long));

    int8_t read_status = 0;
    // uint8_t first_i_frame_received = 0;
    long long frame_counter = 0;

    if (_set_stop_reading_at(stream, options) != RTN_SUCCESS)
        goto end;

    while ((read_status = av_read_frame(stream->format_context, packet)) >= 0 && frame_counter < stream->number_of_frames_to_read &&
           _time_now_microseconds() < stream->stop_reading_at)
    {
        // printf("Reading frame %lld, stream index: %d\n", frame_counter, packet->stream_index);
        if (packet->stream_index == stream->video_stream_index)
        {
            printf("avcodec_send_packet %d\n", avcodec_send_packet(stream->codec_context, packet));

            // if (avcodec_send_packet(stream->codec_context, packet) == 0)
            // {
            //     printf("Packet sent to codec context.\n");
            //     while (avcodec_receive_frame(stream->codec_context, frame) == 0)
            //     {
            //         printf("Frame received from codec context.\n");
            //         if (!first_i_frame_received)
            //         {
            //             if (frame->pict_type == AV_PICTURE_TYPE_I)
            //             {
            //                 first_i_frame_received = 1;
            //                 if (options->debug)
            //                     printf(ANSI_BLUE "Debug:" ANSI_RESET " First I-frame received.\n");
            //             }
            //             else
            //                 continue;
            //         }

            //         // // Convert frame to RGB
            //         // sws_scale(stream->sws_context, (const uint8_t* const*)frame->data, frame->linesize, 0, stream->codec_context->height, image->data,
            //         //           image->linesize);

            //         // // Sum pixel values
            //         // uint8_t* data = image->data[0];
            //         // for (size_t i = 0; i < image_size; ++i) accumulated_pixel_values[i] += data[i];

            //         // // If last frame, compute average and write result
            //         // if (frame_count == frame_limit - 1)
            //         // {
            //         //     if (debug_flag)
            //         //         printf("Computing average frame...\n");

            //         //     uint8_t* avg_buffer = (uint8_t*)malloc(num_bytes);
            //         //     if (!avg_buffer)
            //         //     {
            //         //         send_data_to_fd(STDERR_FILENO, (const uint8_t*)"Could not allocate buffer for average image.\n", NULL);
            //         //         break;
            //         //     }
            //         //     for (size_t i = 0; i < num_bytes; ++i) avg_buffer[i] = (uint8_t)(sum_buffer[i] / frame_limit);

            //         //     char header[32];
            //         //     snprintf(header, sizeof(header), "P6\n%d %d\n255\n", codec_ctx->width, codec_ctx->height);

            //         //     if (debug_flag)
            //         //     {
            //         //         // Save to PPM file (for debugging)
            //         //         char file_name[1024];
            //         //         snprintf(file_name, sizeof(file_name), "debug/%s/average_frame.ppm", normalized_dir_name);
            //         //         FILE* f = fopen(file_name, "wb");
            //         //         if (f)
            //         //         {
            //         //             fwrite(header, 1, strlen(header), f);
            //         //             fwrite(avg_buffer, 1, num_bytes, f);
            //         //             fclose(f);
            //         //             if (debug_flag)
            //         //                 printf("Saved %s\n", file_name);
            //         //         }
            //         //     }

            //         //     // Write average frame to output fd
            //         //     send_data_to_fd(data_fd, (const uint8_t*)header, NULL);
            //         //     send_data_to_fd(data_fd, avg_buffer, &num_bytes);
            //         //     if (debug_flag)
            //         //         printf("Average frame written to data_fd %d\n", data_fd);

            //         //     free(avg_buffer);
            //         // }
            //         frame_counter++;
            //     }
            // }
        }
        av_packet_unref(packet);
    }

end:
    // av_packet_free(&packet);
    // av_frame_free(&frame);
    // av_frame_free(&image);

    return NULL;  // Placeholder for the actual image data
}