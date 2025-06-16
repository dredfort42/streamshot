#include <unistd.h>

#include "errors.h"
#include "process.h"
#include "stream.h"
#include "utilities.h"

short _calculate_limits(stream_t* stream, const options_t* options);

void free_image_process(process_t* img_proc)
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

static process_t* _init_image_process(stream_t* stream, const options_t* options)
{
    if (!stream || !stream->codec_context || stream->video_stream_index < 0 || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_INVALID_ARGUMENTS "\n");

        return NULL;
    }

    process_t* img_proc = (process_t*)malloc(sizeof(process_t));
    if (!img_proc)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");

        return NULL;
    }

    img_proc->video_frame = av_frame_alloc();
    if (!img_proc->video_frame)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        goto error;
    }

    img_proc->image_frame = av_frame_alloc();
    if (!img_proc->image_frame)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        goto error;
    }

    img_proc->image_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, stream->codec_context->width, stream->codec_context->height, 1);
    if (img_proc->image_size < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_FAILED_TO_GET_IMAGE_SIZE "\n");
    }

    img_proc->buffer = (uint8_t*)av_malloc(img_proc->image_size * sizeof(uint8_t));
    if (!img_proc->buffer)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        goto error;
    }

    if ((ssize_t)img_proc->image_size != (ssize_t)av_image_fill_arrays(img_proc->image_frame->data, img_proc->image_frame->linesize, img_proc->buffer,
                                                                       AV_PIX_FMT_RGB24, stream->codec_context->width, stream->codec_context->height, 1))
        goto error;

    img_proc->pixel_sum_buffer = (unsigned long long*)calloc(img_proc->image_size, sizeof(unsigned long long));
    if (!img_proc->pixel_sum_buffer)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_image_process | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        goto error;
    }

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Initialized image processing structures for video stream index: %d\n", stream->video_stream_index);

    return img_proc;

error:
    free_image_process(img_proc);

    return NULL;
}

short process_stream(options_t* options)
{
    if (!options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | " ERROR_INVALID_ARGUMENTS "\n");

        return RTN_ERROR;
    }

    stream_t* stream = get_stream(options);
    if (!stream)

        return RTN_ERROR;

    process_t* img_proc = _init_image_process(stream, options);

    // -------------

    AVPacket* av_packet = av_packet_alloc();

    unsigned long long frame_counter = 0;
    short got_first_i_frame = 0;

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Starting to read %u frames from RTSP stream...\n", stream->number_of_frames_to_read);

    if (_calculate_limits(stream, options))
        return RTN_ERROR;

    // Main loop: read and process frames
    int read_status = 0;
    // while ((read_result = av_read_frame(stream->format_context, av_packet)) >= 0 && frame_counter < stream->number_of_frames_to_read)
    while ((read_status = av_read_frame(stream->format_context, av_packet)) >= 0 && frame_counter < stream->number_of_frames_to_read &&
           time_now_in_microseconds() < stream->stop_reading_at)
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
                            write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
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