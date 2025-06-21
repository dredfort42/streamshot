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
    if (!strcmp(str, "jpg"))
        return IMAGE_FORMAT_JPG;
    else if (!strcmp(str, "jpeg"))
        return IMAGE_FORMAT_JPEG;
    else if (!strcmp(str, "png"))
        return IMAGE_FORMAT_PNG;
    else if (!strcmp(str, "ppm"))
        return IMAGE_FORMAT_PPM;
    else
        return IMAGE_FORMAT_UNKNOWN;
}
