#include "process.h"

#include <unistd.h>

#include "errors.h"
#include "stream.h"
#include "utilities.h"

process_t* _init_process(stream_t* stream, const options_t* options);
short _calculate_limits(stream_t* stream, const options_t* options);

void free_process(process_t* process)
{
    if (!process)

        return;

    if (process->video_frame)
        av_frame_free(&process->video_frame);
    if (process->image_frame)
        av_frame_free(&process->image_frame);
    if (process->buffer)
        av_free(process->buffer);
    if (process->pixel_sum_buffer)
        free(process->pixel_sum_buffer);

    free(process);
}

raw_image_t* get_raw_image(options_t* options)
{
    if (!options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | " ERROR_INVALID_ARGUMENTS "\n");

        return NULL;
    }

    stream_t* stream = get_stream(options);
    if (stream == NULL)

        return NULL;

    process_t* process = _init_process(stream, options);

    // -------------

    AVPacket* av_packet = av_packet_alloc();

    // return NULL;

    raw_image_t* raw_image = (raw_image_t*)malloc(sizeof(raw_image_t));
    if (!raw_image)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        free_process(process);
        return NULL;
    }

    raw_image->data = (uint8_t*)malloc(process->image_size);
    if (!raw_image->data)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        free_process(process);
        free(raw_image);
        return NULL;
    }

    raw_image->size = process->image_size;
    raw_image->width = stream->codec_context->width;
    raw_image->height = stream->codec_context->height;

    // -------------

    unsigned long long frame_counter = 0;
    short got_first_i_frame_flag = 0;

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Starting to read %u frames from RTSP stream...\n", stream->number_of_frames_to_read);

    if (_calculate_limits(stream, options))
        return NULL;

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
                while (avcodec_receive_frame(stream->codec_context, process->video_frame) == 0)
                {
                    // Wait for first I-frame (key frame)
                    if (!got_first_i_frame_flag)
                    {
                        if (process->video_frame->pict_type == AV_PICTURE_TYPE_I)
                            got_first_i_frame_flag = 1;
                        else
                            continue;
                    }

                    // Convert video_frame to RGB
                    sws_scale(stream->sws_context, (const uint8_t* const*)process->video_frame->data, process->video_frame->linesize, 0,
                              stream->codec_context->height, process->image_frame->data, process->image_frame->linesize);

                    // Sum pixel values
                    // uint8_t* data = image_frame->data[0];
                    for (size_t i = 0; i < process->image_size; ++i) process->pixel_sum_buffer[i] += process->image_frame->data[0][i];

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
                            fwrite(process->image_frame->data[0], 1, process->image_size, f);
                            fclose(f);
                            printf("Saved %s\n", file_name);
                        }
                    }

                    // If last video_frame, compute average and write result
                    if (frame_counter == stream->number_of_frames_to_read - 1)
                    {
                        if (options->debug)
                            printf("Computing average video_frame...\n");

                        for (size_t i = 0; i < process->image_size; ++i)
                            raw_image->data[i] = (uint8_t)(process->pixel_sum_buffer[i] / stream->number_of_frames_to_read);
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
    free_process(process);
    av_packet_free(&av_packet);
    sws_freeContext(stream->sws_context);
    avformat_close_input(&stream->format_context);
    av_dict_free(&stream->options);

    if (frame_counter < stream->number_of_frames_to_read)

        return NULL;

    return raw_image;
}
