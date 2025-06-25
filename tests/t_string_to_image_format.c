/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | t_string_to_image_format.c
    ::  ::          ::  ::    Created  | 2025-06-25
          ::::  ::::          Modified | 2025-06-25

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdio.h>
#include <string.h>

#include "options.h"
#include "utilities.h"

int t_string_to_image_format(void)
{
    int failed = 0;

    struct
    {
        const char* format;
        image_format_t expected;
    } tests[] = {{"jpg", IMAGE_FORMAT_JPG},
                 {"jpeg", IMAGE_FORMAT_JPEG},
                 {"png", IMAGE_FORMAT_PNG},
                 {"ppm", IMAGE_FORMAT_PPM},
                 {"unknown format", IMAGE_FORMAT_UNKNOWN},
                 {"", IMAGE_FORMAT_UNKNOWN},
                 {"invalid", IMAGE_FORMAT_UNKNOWN},
                 {"JPG", IMAGE_FORMAT_JPG},
                 {"JPEG", IMAGE_FORMAT_JPEG},
                 {"PNG", IMAGE_FORMAT_PNG},
                 {"PPM", IMAGE_FORMAT_PPM}};

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
    {
        if (string_to_image_format(tests[i].format) != tests[i].expected)
        {
            printf("[" ANSI_RED "KO" ANSI_RESET
                   "] (f) string_to_image_format: format='%s' failed\n",
                   tests[i].format);
            failed++;
        }
        else
            printf("[" ANSI_GREEN "OK" ANSI_RESET
                   "] (f) string_to_image_format: format='%s' passed\n",
                   tests[i].format);
    }

    return failed;
}