/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | image_format.c
    ::  ::          ::  ::    Created  | 2025-06-16
          ::::  ::::          Modified | 2025-06-20

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <string.h>

#include "options.h"

/* Helper function to get string representation of image_format_t */
const char* image_format_to_string(image_format_t format)
{
    switch (format)
    {
        case IMAGE_FORMAT_JPG:
            return "jpg";
        case IMAGE_FORMAT_JPEG:
            return "jpeg";
        case IMAGE_FORMAT_PNG:
            return "png";
        case IMAGE_FORMAT_PPM:
            return "ppm";
        default:
            return "unknown format";
    }
}

/* Helper function to convert string to image_format_t */
image_format_t string_to_image_format(const char* str)
{
    if (!str)
        return IMAGE_FORMAT_UNKNOWN;

    char lower_str[16];
    size_t i;
    for (i = 0; i < sizeof(lower_str) - 1 && str[i]; ++i)
        lower_str[i] = (char)tolower((unsigned char)str[i]);
    lower_str[i] = '\0';

    if (strcmp(lower_str, "jpg") == 0)
        return IMAGE_FORMAT_JPG;
    else if (strcmp(lower_str, "jpeg") == 0)
        return IMAGE_FORMAT_JPEG;
    else if (strcmp(lower_str, "png") == 0)
        return IMAGE_FORMAT_PNG;
    else if (strcmp(lower_str, "ppm") == 0)
        return IMAGE_FORMAT_PPM;
    else
        return IMAGE_FORMAT_UNKNOWN;
}
