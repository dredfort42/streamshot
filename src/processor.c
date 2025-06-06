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
    AVFrame* frame = av_frame_alloc();
    // AVFrame* scaled_frame = av_frame_alloc();
    int8_t read_status = 0;
    uint8_t first_i_frame_received = 0;
    long long frame_counter = 0;

    if (_set_stop_reading_at(stream, options) != RTN_SUCCESS)
        goto end;

    while ((read_status = av_read_frame(stream->format_context, packet)) >= 0 && frame_counter < stream->number_of_frames_to_read &&
           _time_now_microseconds() < stream->stop_reading_at)
    {
        if (packet->stream_index == stream->video_stream_index && stream->codec_context->codec_type == AVMEDIA_TYPE_VIDEO &&
            (avcodec_send_packet(stream->codec_context, packet) == 0))
        {
            while (avcodec_receive_frame(stream->codec_context, frame) == 0)
            {
                if (!first_i_frame_received)
                {
                    if (frame->pict_type == AV_PICTURE_TYPE_I)
                    {
                        first_i_frame_received = 1;
                        frame_counter = 0;
                        if (options->debug)
                            printf(ANSI_BLUE "Debug:" ANSI_RESET " First I-frame received.\n");
                    }
                    else
                        continue;
                }
                // Save the frame as a PPM image
                // char filename[256];
                // snprintf(filename, sizeof(filename), "frame_%lld.ppm", frame_counter);

                // FILE* ppm_file = fopen(filename, "wb");
                // if (!ppm_file)
                // {
                //     write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | Failed to open file for writing");
                //     continue;
                // }

                // // Write PPM header
                // fprintf(ppm_file, "P6\n%d %d\n255\n", frame->width, frame->height);

                // // Write pixel data (assuming AV_PIX_FMT_RGB24)
                // if (frame->format == AV_PIX_FMT_RGB24)
                // {
                //     for (int y = 0; y < frame->height; y++)
                //     {
                //         fwrite(frame->data[0] + y * frame->linesize[0], 1, frame->width * 3, ppm_file);
                //     }
                // }
                // else
                // {
                //     write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | Frame is not in RGB24 format");
                // }

                // fclose(ppm_file);
                frame_counter++;
            }
        }

        av_packet_unref(packet);  // Unreference the packet after processing
    }

end:
    av_packet_free(&packet);
    av_frame_free(&frame);

    return NULL;  // Placeholder for the actual image data
}