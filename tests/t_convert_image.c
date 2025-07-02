/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | t_convert_image.c
    ::  ::          ::  ::    Created  | 2025-06-28
          ::::  ::::          Modified | 2025-06-28

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "errors.h"
#include "process.h"
#include "utilities.h"

int test_null_arguments(void)
{
    image_t* res = get_converted_image(NULL, NULL);
    if (res != NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) convert_image: null arguments failed | expected NULL result\n");
        free_image(res);
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) convert_image: null arguments passed\n");
    return 0;
}

int test_invalid_dimensions(void)
{
    options_t opts = {.output_format = IMAGE_FORMAT_JPG, .image_quality = 90};
    image_t img = {.width = 0, .height = 100, .data = (void*)1, .size = 1};
    image_t* res = get_converted_image(&opts, &img);
    if (res != NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) convert_image: invalid dimensions failed | expected NULL result\n");
        free_image(res);
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) convert_image: invalid dimensions passed\n");
    return 0;
}

int test_no_data(void)
{
    options_t opts = {.output_format = IMAGE_FORMAT_JPG, .image_quality = 90};
    image_t img = {.width = 100, .height = 100, .data = NULL, .size = 1};
    image_t* res = get_converted_image(&opts, &img);
    if (res != NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) convert_image: no data failed | expected NULL result\n");
        free_image(res);
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) convert_image: no data passed\n");
    return 0;
}

int test_invalid_format(void)
{
    options_t opts = {.output_format = IMAGE_FORMAT_UNKNOWN, .image_quality = 90};
    size_t dummy_size = 100 * 100 * 3;
    uint8_t* dummy = malloc(dummy_size);
    if (!dummy)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) convert_image: memory allocation failed\n");
        return 1;
    }

    memset(dummy, 255, dummy_size);
    image_t img = {.width = 100, .height = 100, .data = dummy, .size = dummy_size};
    image_t* res = get_converted_image(&opts, &img);
    if (res != NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) convert_image: invalid format failed | expected NULL result\n");
        free_image(res);
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) convert_image: invalid format passed\n");
    return 0;
}

int test_type_conversion(enum image_format_e format)
{
    options_t opts = {.output_format = format, .image_quality = 90};
    size_t dummy_size = 100 * 100 * 3;
    uint8_t* dummy = malloc(dummy_size);
    if (!dummy)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) convert_image: memory allocation failed\n");
        return 1;
    }

    memset(dummy, 255, dummy_size);
    image_t img = {.width = 100, .height = 100, .data = dummy, .size = dummy_size};
    image_t* res = get_converted_image(&opts, &img);
    if (res == NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) convert_image: %s conversion failed | expected non-NULL result\n",
               image_format_to_string(format));
        free(dummy);
        return 1;
    }

    int ok = (res->width == 100 && res->height == 100);
    free_image(res);
    free(dummy);

    if (!ok)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) convert_image: %s conversion failed | expected 100x100 image\n",
               image_format_to_string(format));
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) convert_image: %s conversion passed\n",
           image_format_to_string(format));
    return 0;
}

int test_jpg_conversion(void) { return test_type_conversion(IMAGE_FORMAT_JPG); }
int test_jpeg_conversion(void) { return test_type_conversion(IMAGE_FORMAT_JPEG); }
int test_png_conversion(void) { return test_type_conversion(IMAGE_FORMAT_PNG); }
int test_ppm_conversion(void) { return test_type_conversion(IMAGE_FORMAT_PPM); }

int test_convert_image(void)
{
    int fails = 0;
    fails += test_null_arguments();
    fails += test_invalid_dimensions();
    fails += test_no_data();
    fails += test_invalid_format();
    fails += test_jpg_conversion();
    fails += test_jpeg_conversion();
    fails += test_png_conversion();
    fails += test_ppm_conversion();

    return fails;
}