/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | t_ppm_image.c
    ::  ::          ::  ::    Created  | 2025-06-28
          ::::  ::::          Modified | 2025-06-28

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "process.h"
#include "utilities.h"

static int test_valid_rgb_data(void)
{
    size_t size = 100 * 100 * RGB_BYTES_PER_PIXEL;
    uint8_t* rgb_data = malloc(size);
    if (!rgb_data)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) test_valid_rgb_data: memory allocation failed\n");
        return 1;
    }

    for (size_t i = 0; i < size; ++i) rgb_data[i] = (uint8_t)(i % 256);

    image_t* img = get_ppm_image(rgb_data, size, 100, 100);
    if (img == NULL || img->data == NULL || img->size == 0 || img->width != 100 ||
        img->height != 100)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) get_ppm_image: failed to create image\n");
        free_image(img);
        free(rgb_data);
        return 1;
    }

    free_image(img);
    free(rgb_data);
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) get_ppm_image: valid RGB data test passed\n");
    return 0;
}

static int test_null_data(void)
{
    size_t size = 100 * 100 * RGB_BYTES_PER_PIXEL;
    image_t* img = get_ppm_image(NULL, size, 100, 100);
    if (img != NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) get_ppm_image: NULL data test failed | expected NULL result\n");
        free_image(img);
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) get_ppm_image: NULL data test passed\n");
    return 0;
}

static int test_invalid_arguments(void)
{
    int failed = 0;
    size_t size = 100 * 100 * RGB_BYTES_PER_PIXEL;
    uint8_t* rgb_data = malloc(size);
    if (!rgb_data)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) test_invalid_arguments: memory allocation failed\n");
        return 1;
    }

    for (size_t i = 0; i < size; ++i) rgb_data[i] = (uint8_t)(i % 256);

    if (get_ppm_image(rgb_data, size, 0, 100) != NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) get_ppm_image: invalid width test failed | expected NULL result\n");
        failed += 1;
    }
    else
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) get_ppm_image: invalid width test passed | expected NULL result\n");

    if (get_ppm_image(rgb_data, size, -1, 100) != NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) get_ppm_image: invalid width test failed | expected NULL result\n");
        failed += 1;
    }
    else
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) get_ppm_image: invalid width test passed | expected NULL result\n");

    if (get_ppm_image(rgb_data, size, 100, 0) != NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) get_ppm_image: invalid height test failed | expected NULL result\n");
        failed += 1;
    }
    else
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) get_ppm_image: invalid height test passed | expected NULL result\n");

    if (get_ppm_image(rgb_data, size, 100, -1) != NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) get_ppm_image: invalid height test failed | expected NULL result\n");
        failed += 1;
    }
    else
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) get_ppm_image: invalid height test passed | expected NULL result\n");

    if (get_ppm_image(rgb_data, -1, 100, 100) != NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) get_ppm_image: invalid size test failed | expected NULL result\n");
        failed += 1;
    }
    else
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) get_ppm_image: invalid size test passed | expected NULL result\n");

    if (get_ppm_image(rgb_data, 111, 100, 100) != NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) get_ppm_image: invalid size test failed | expected NULL result\n");
        failed += 1;
    }
    else
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) get_ppm_image: invalid size test passed | expected NULL result\n");

    if (get_ppm_image(rgb_data, size + 1, 100, 100) != NULL)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) get_ppm_image: invalid size test failed | expected NULL result\n");
        failed += 1;
    }
    else
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) get_ppm_image: invalid size test passed | expected NULL result\n");

    free(rgb_data);
    return failed;
}

int test_ppm_image(void)
{
    int failed = 0;
    failed += test_valid_rgb_data();
    failed += test_null_data();
    failed += test_invalid_arguments();
    return failed;
}