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

    if (process->av_packet)
        av_packet_free(&process->av_packet);
    if (process->video_frame)
        av_frame_free(&process->video_frame);
    if (process->image_frame)
        av_frame_free(&process->image_frame);
    if (process->buffer)
        av_free(process->buffer);
    if (process->sum_buffer)
        free(process->sum_buffer);

    free(process);
}

void free_raw_image(raw_image_t* raw_image)
{
    if (!raw_image)
        return;

    if (raw_image->data)
        free(raw_image->data);
    free(raw_image);
}

void save_debug_ppm_frame(process_t* process, const options_t* options)
{
    if (!process || !options || !options->debug_dir || !process->image_frame->data[0] ||
        !process->image_size)
        return;

    char debug_file_name[256];
    snprintf(debug_file_name, sizeof(debug_file_name), "%s/debug_image_%llu.ppm",
             options->debug_dir, process->received_frames);

    save_ppm(debug_file_name, process->image_frame->data[0], process->image_size,
             process->image_frame->width, process->image_frame->height);

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Saved debug image to %s\n", debug_file_name);
}

short read_frame(stream_t* stream, process_t* process, const options_t* options)
{
    if (!stream || !process || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) read_frame | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    if (!process->av_packet)
    {
        process->av_packet = av_packet_alloc();
        if (!process->av_packet)
        {
            write_msg_to_fd(STDERR_FILENO,
                            "(f) read_frame | " ERROR_FAILED_TO_ALLOCATE_PACKET "\n");
            return RTN_ERROR;
        }
    }

    int ret = 0;

    if ((process->stream_read_status = av_read_frame(stream->format_context, process->av_packet)) <
        0)
        return RTN_ERROR;

    if (process->av_packet->stream_index == stream->video_stream_index)
    {
        ret = avcodec_send_packet(stream->codec_context, process->av_packet);
        if (ret < 0)
        {
            write_msg_to_fd(STDERR_FILENO,
                            "(f) get_streamshot | " ERROR_FAILED_TO_SEND_PACKET "\n");
            return RTN_ERROR;
        }

        while (ret >= 0)
        {
            ret = avcodec_receive_frame(stream->codec_context, process->video_frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            else if (ret < 0)
            {
                write_msg_to_fd(STDERR_FILENO,
                                "(f) get_streamshot | " ERROR_FAILED_TO_RECEIVE_FRAME "\n");
                break;
            }

            if (!process->got_first_i_frame && process->video_frame->pict_type == AV_PICTURE_TYPE_I)
            {
                process->got_first_i_frame = 1;
                if (options->debug)
                    printf(ANSI_BLUE "Debug:" ANSI_RESET " First I-frame received.\n");
            }
            else if (!process->got_first_i_frame)
                continue;

            // Convert frame to RGB
            sws_scale(stream->sws_context, (const uint8_t* const*)process->video_frame->data,
                      process->video_frame->linesize, 0, stream->codec_context->height,
                      process->image_frame->data, process->image_frame->linesize);

            // Sum pixel values
            for (size_t i = 0; i < process->image_size; ++i)
                process->sum_buffer[i] += (unsigned long long)process->image_frame->data[0][i];

            process->received_frames++;

            if (options->debug && options->debug_step &&
                (!process->received_frames || process->received_frames % options->debug_step == 0 ||
                 process->received_frames == stream->number_of_frames_to_read))
                save_debug_ppm_frame(process, options);

            av_frame_unref(process->video_frame);

            if (options->debug)
                printf(ANSI_BLUE "Debug:" ANSI_RESET " Processed frame %06llu/%06u [%06d bytes]\n",
                       process->received_frames, stream->number_of_frames_to_read,
                       process->av_packet->size);
        }
    }

    av_packet_unref(process->av_packet);
    return RTN_SUCCESS;
}

raw_image_t* _init_raw_image(const process_t* process, const stream_t* stream,
                             const options_t* options)
{
    raw_image_t* raw_image = (raw_image_t*)malloc(sizeof(raw_image_t));
    if (!raw_image)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) get_streamshot | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        goto error;
    }

    raw_image->data = (uint8_t*)malloc(process->image_size);
    if (!raw_image->data)
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) get_streamshot | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        goto error;
    }

    if (options->debug)
        printf("Computing average video_frame...\n");

    raw_image->size = process->image_size;
    raw_image->width = stream->codec_context->width;
    raw_image->height = stream->codec_context->height;

    for (size_t i = 0; stream->number_of_frames_to_read && i < process->image_size; ++i)
        raw_image->data[i] = (uint8_t)(process->sum_buffer[i] / stream->number_of_frames_to_read);

    return raw_image;

error:
    if (raw_image)
    {
        if (raw_image->data)
            free(raw_image->data);

        free(raw_image);
    }

    return NULL;
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

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Starting to read %u frames from RTSP stream...\n",
               stream->number_of_frames_to_read);

    if (_calculate_limits(stream, options))
        return NULL;

    while (process->received_frames < stream->number_of_frames_to_read &&
           time_now_in_microseconds() < stream->stop_reading_at &&
           !read_frame(stream, process, options));

    // Check if connection was interrupted
    if (process->received_frames < stream->number_of_frames_to_read)
    {
        char error_msg[256];
        if (process->stream_read_status < 0)
        {
            snprintf(error_msg, sizeof(error_msg),
                     "Camera connection lost or error occurred (av_read_frame "
                     "returned %d; processed %llu out of %u frames).\n",
                     process->stream_read_status, process->received_frames,
                     stream->number_of_frames_to_read);
        }
        else
        {
            snprintf(error_msg, sizeof(error_msg),
                     "Stream ended before enough frames were processed (%llu out of "
                     "%u frames).\n",
                     process->received_frames, stream->number_of_frames_to_read);
        }
        write_msg_to_fd(STDERR_FILENO, error_msg);
    }

    raw_image_t* raw_image = _init_raw_image(process, stream, options);

    free_process(process);
    free_stream(stream);

    return raw_image;
}
