/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | reader.c
    ::  ::          ::  ::    Created  | 2025-06-06
          ::::  ::::          Modified | 2025-06-06

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "reader.h"

uint8_t* get_streamshot(const options_t* options)
{
    if (!options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) get_streamshot | " ERROR_INVALID_ARGUMENTS);
        return NULL;
    }

    stream_t* stream = _init_stream();
    if (!stream)
        goto end;

    if (_set_stream_options(stream, options) != RTN_SUCCESS)
        goto end;

    if (_open_stream(stream, options) != RTN_SUCCESS)
        goto end;

    if (_find_decoder(stream, options) != RTN_SUCCESS)
        goto end;

    if (_init_codec_context(stream, options) != RTN_SUCCESS)
        goto end;

    if (_calculate_number_of_frames_to_read(stream, options) != RTN_SUCCESS)
        goto end;

    if (_init_sws_context(stream, options) != RTN_SUCCESS)
        goto end;

    _process_streamshot(stream, options);

end:
    _free_stream(stream);

    return NULL;  // Return NULL for now as this is a placeholder
}