/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | read_frame.c
    ::  ::          ::  ::    Created  | 2025-06-19
          ::::  ::::          Modified | 2025-06-28

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "process.h"
#include "stream.h"
#include "utilities.h"

/**
 * @brief Saves the current image frame to a debug file in PPM format.
 *
 * This function checks if all required pointers and data are valid, then constructs
 * a file name using the debug directory and the number of received frames. It saves
 * the image frame data as a PPM file for debugging purposes.
 *
 * @param process Pointer to the process structure containing the image frame and related data.
 * @param options Pointer to the options structure containing debug settings and directory path.
 */
static void _save_debug_frame(process_t* process, const options_t* options)
{
    if (!process || !options || !options->debug_dir || !process->image_frame ||
        !process->image_frame->data[0] || !process->image_size)
        return;

    char debug_file_name[256];
    snprintf(debug_file_name, sizeof(debug_file_name), "%s/debug_image_%010llu.ppm",
             options->debug_dir, process->received_frames);

    if (!save_ppm(debug_file_name, process->image_frame->data[0], process->image_size,
                  process->image_frame->width, process->image_frame->height))
    {
        if (options->debug)
            printf(ANSI_BLUE "Debug:" ANSI_RESET " Saved debug image to %s\n", debug_file_name);
    }
    else
        write_msg_to_fd(STDERR_FILENO,
                        "(f) _save_debug_frame | " ERROR_FAILED_TO_SAVE_DEBUG_FILE "\n");
}

/**
 * @brief Reads and processes a single frame from the input stream.
 *
 * This function reads a frame from the given multimedia stream, decodes it,
 * processes the video frame, and updates the process state accordingly.
 *
 * @param stream   Pointer to the stream_t structure containing stream context.
 * @param process  Pointer to the process_t structure holding processing state and buffers.
 * @param options  Pointer to the options_t structure specifying processing options and debug flags.
 *
 * @return 0 on success, -1 on failure.
 */
short _read_frame(stream_t* stream, process_t* process, const options_t* options)
{
    if (!stream || !process || !options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _read_frame | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    if (!process->av_packet || !process->video_frame || !process->image_frame || !process->buffer ||
        !process->sum_buffer)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _read_frame | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    if ((process->stream_read_status = av_read_frame(stream->format_context, process->av_packet)) <
        0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _read_frame | " ERROR_FAILED_TO_READ_FRAME "\n");
        return RTN_ERROR;
    }

    if (process->av_packet->stream_index == stream->video_stream_index)
    {
        int ret = avcodec_send_packet(stream->codec_context, process->av_packet);
        if (ret < 0)
        {
            write_msg_to_fd(STDERR_FILENO, "(f) _read_frame | " ERROR_FAILED_TO_SEND_PACKET "\n");
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
                                "(f) _read_frame | " ERROR_FAILED_TO_RECEIVE_FRAME "\n");
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

            sws_scale(stream->sws_context, (const uint8_t* const*)process->video_frame->data,
                      process->video_frame->linesize, 0, stream->codec_context->height,
                      process->image_frame->data, process->image_frame->linesize);

            for (size_t i = 0; process->image_frame->data[0] && i < process->image_size; ++i)
                process->sum_buffer[i] += (unsigned long long)process->image_frame->data[0][i];

            process->received_frames++;

            if (options->debug)
            {
                printf(ANSI_BLUE "Debug:" ANSI_RESET " Processed frame %06llu/%06u [%06d bytes]\n",
                       process->received_frames, stream->number_of_frames_to_read,
                       process->av_packet->size);

                if (options->debug_step &&
                    (process->received_frames == 1 ||
                     process->received_frames % options->debug_step == 0 ||
                     process->received_frames == stream->number_of_frames_to_read))
                    _save_debug_frame(process, options);
            }

            av_frame_unref(process->video_frame);
        }
    }

    av_packet_unref(process->av_packet);
    return RTN_SUCCESS;
}
