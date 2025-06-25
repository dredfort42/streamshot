/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | t_image_format_to_string.c
    ::  ::          ::  ::    Created  | 2025-06-25
          ::::  ::::          Modified | 2025-06-25

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdio.h>
#include <string.h>

#include "options.h"
#include "utilities.h"

int t_image_format_to_string(void)
{
    int failed = 0;

    struct
    {
        image_format_t format;
        const char* expected;
    } tests[] = {{IMAGE_FORMAT_JPG, "jpg"},
                 {IMAGE_FORMAT_JPEG, "jpeg"},
                 {IMAGE_FORMAT_PNG, "png"},
                 {IMAGE_FORMAT_PPM, "ppm"},
                 {IMAGE_FORMAT_UNKNOWN, "unknown format"},
                 {(image_format_t)999, "unknown format"}};

    size_t num_tests = sizeof(tests) / sizeof(tests[0]);
    for (size_t i = 0; i < num_tests; ++i)
    {
        const char* result = image_format_to_string(tests[i].format);
        if (strcmp(result, tests[i].expected) != 0)
        {
            printf("[" ANSI_RED "KO" ANSI_RESET
                   "] (f) image_format_to_string: format=%d, expected='%s', got='%s'\n",
                   (int)tests[i].format, tests[i].expected, result);
            failed = 1;
        }
        else
            printf("[" ANSI_GREEN "OK" ANSI_RESET
                   "] (f) image_format_to_string: format=%d, expected='%s', got='%s'\n",
                   (int)tests[i].format, tests[i].expected, result);
    }

    return failed;
}