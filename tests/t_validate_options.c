
/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | t_validate_options.c
    ::  ::          ::  ::    Created  | 2025-06-29
          ::::  ::::          Modified | 2025-06-29

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "options.h"
#include "utilities.h"

static options_t* make_valid_options(void)
{
    options_t* opts = (options_t*)malloc(sizeof(options_t));
    if (!opts)
    {
        fprintf(stderr, "Failed to allocate memory for options_t\n");
        exit(1);
    }

    memset(opts, 0, sizeof(options_t));
    opts->rtsp_url = "rtsp://localhost:8554/stream";
    opts->timeout_sec = 10;
    opts->output_file_path = "/tmp/test_output.jpg";
    opts->output_file_fd = -1;
    opts->exposure_sec = 0;
    opts->output_format = IMAGE_FORMAT_JPEG;
    opts->scale_factor = 1.0f;
    opts->resize_height = -1;
    opts->resize_width = -1;
    opts->image_quality = 90;
    opts->debug = 0;
    opts->debug_step = 1;
    opts->debug_dir = "/tmp";
    opts->help = 0;
    opts->version = 0;
    return opts;
}

int test_valid_options(void)
{
    options_t* opts = make_valid_options();
    short ret = validate_options(opts);
    free(opts);
    if (ret)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) validate_options: valid options test failed | expected return code %d, got %d\n",
            RTN_SUCCESS, ret);
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) validate_options: valid options test passed\n");
    return ret ? 1 : 0;
}

int test_null_options(void)
{
    // Test with NULL options
    short ret = validate_options(NULL);
    if (!ret)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) validate_options: null options test failed | expected return code %d, got %d\n",
            RTN_ERROR, ret);
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) validate_options: null options test passed\n");
    return 0;
}

int test_help_shortcut(void)
{
    int failed = 0;

    options_t* opts = make_valid_options();
    opts->help = 1;
    short ret = validate_options(opts);
    if (ret)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) validate_options: help shortcut test failed | expected return code %d, got %d\n",
            RTN_SUCCESS, ret);
        failed++;
    }

    opts->help = 0;
    ret = validate_options(opts);
    if (ret)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) validate_options: help shortcut test failed | expected return code %d, got %d\n",
            RTN_SUCCESS, ret);
        failed++;
    }

    opts->version = 111;
    ret = validate_options(opts);
    if (ret)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) validate_options: help shortcut test failed | expected return code %d, got %d\n",
            RTN_SUCCESS, ret);
        failed++;
    }

    opts->version = -111;
    ret = validate_options(opts);
    if (ret)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) validate_options: help shortcut test failed | expected return code %d, got %d\n",
            RTN_SUCCESS, ret);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: help shortcut test passed\n");

    return failed;
}

int test_version_shortcut(void)
{
    int failed = 0;

    options_t* opts = make_valid_options();
    opts->version = 1;
    short ret = validate_options(opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: version shortcut test failed | expected return code %d, "
               "got %d\n",
               RTN_SUCCESS, ret);
        failed++;
    }

    opts->version = 0;
    ret = validate_options(opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: version shortcut test failed | expected return code %d, "
               "got %d\n",
               RTN_SUCCESS, ret);
        failed++;
    }

    opts->help = 111;
    ret = validate_options(opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: version shortcut test failed | expected return code %d, "
               "got %d\n",
               RTN_SUCCESS, ret);
        failed++;
    }

    opts->help = -111;
    ret = validate_options(opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: version shortcut test failed | expected return code %d, "
               "got %d\n",
               RTN_SUCCESS, ret);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: version shortcut test passed\n");

    return failed;
}

int test_debug_shortcut(void)
{
    int failed = 0;
    options_t* opts = make_valid_options();
    opts->debug = 1;
    short ret = validate_options(opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: debug shortcut test failed | expected return code %d, got "
               "%d\n",
               RTN_SUCCESS, ret);
        failed++;
    }

    opts->debug = 0;
    ret = validate_options(opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: debug shortcut test failed | expected return code %d, got "
               "%d\n",
               RTN_SUCCESS, ret);
        failed++;
    }

    opts->debug = 111;
    ret = validate_options(opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: debug shortcut test failed | expected return code %d, got "
               "%d\n",
               RTN_SUCCESS, ret);
        failed++;
    }

    opts->debug = -111;
    ret = validate_options(opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: debug shortcut test failed | expected return code %d, got "
               "%d\n",
               RTN_SUCCESS, ret);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: debug shortcut test passed\n");

    return failed;
}

int test_invalid_rtsp_url(void)
{
    int failed = 0;
    options_t* opts = make_valid_options();
    opts->rtsp_url = "http://badurl";
    short res = validate_options(opts);
    if (res == RTN_SUCCESS)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid RTSP URL test failed | expected return code %d, "
               "got %d\n",
               RTN_ERROR, res);
        failed++;
    }

    opts->rtsp_url = NULL;
    res = validate_options(opts);
    if (res == RTN_SUCCESS)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid RTSP URL test failed | expected return code %d, "
               "got %d\n",
               RTN_ERROR, res);
        failed++;
    }

    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: invalid RTSP URL test passed\n");

    free(opts);
    return failed;
}

int test_invalid_timeout(void)
{
    int failed = 0;
    options_t* opts = make_valid_options();
    opts->timeout_sec = 0;
    short res = validate_options(opts);
    if (res != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid timeout test failed | expected return code %d, got "
               "%d\n",
               RTN_ERROR, res);
        failed++;
    }

    opts->timeout_sec = MAX_TIMEOUT_SEC + 1;
    res = validate_options(opts);
    if (res != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid timeout test failed | expected return code %d, got "
               "%d\n",
               RTN_ERROR, res);
        failed++;
    }

    opts->timeout_sec = 10;
    res = validate_options(opts);
    if (res != RTN_SUCCESS)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid timeout test failed | expected return code %d, got "
               "%d\n",
               RTN_SUCCESS, res);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: invalid timeout test passed\n");

    return failed;
}

int test_invalid_output_file_path(void)
{
    int failed = 0;

    options_t* opts = make_valid_options();
    opts->output_file_path = "t";
    short ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid output file path test failed | expected return "
               "code %d, got "
               "%d\n",
               RTN_ERROR, ret);
        failed++;
    }

    opts->output_file_path = NULL;
    ret = validate_options(opts);
    if (ret != RTN_SUCCESS)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid output file path test failed | expected return "
               "code %d, got "
               "%d\n",
               RTN_SUCCESS, ret);
        failed++;
    }

    opts->output_file_path = "/tmp/test_output.jpg";
    ret = validate_options(opts);
    if (ret != RTN_SUCCESS)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid output file path test failed | expected return "
               "code %d, got "
               "%d\n",
               RTN_SUCCESS, ret);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: invalid output file path test passed\n");

    return failed;
}

int test_invalid_output_file_fd(void)
{
    int failed = 0;
    options_t* opts = make_valid_options();
    opts->output_file_fd = MIN_OUTPUT_FD - 1;
    short ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid output file fd test failed | expected return code "
               "%d, got %d\n",
               RTN_ERROR, ret);
        failed++;
    }

    opts->output_file_fd = MIN_OUTPUT_FD;
    ret = validate_options(opts);
    if (ret != RTN_SUCCESS)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid output file fd test failed | expected return code "
               "%d, got %d\n",
               RTN_SUCCESS, ret);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: invalid output file fd test passed\n");

    return failed;
}

int test_invalid_exposure(void)
{
    int failed = 0;
    options_t* opts = make_valid_options();
    opts->exposure_sec = -1;
    short ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid exposure test failed | expected return code %d, "
               "got %d\n",
               RTN_ERROR, ret);
        failed++;
    }

    opts->exposure_sec = MAX_EXPOSURE_SEC + 1;
    ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid exposure test failed | expected return code %d, "
               "got %d\n",
               RTN_ERROR, ret);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: invalid exposure test passed\n");

    return failed;
}

int test_invalid_output_format(void)
{
    int failed = 0;
    options_t* opts = make_valid_options();
    opts->output_format = IMAGE_FORMAT_UNKNOWN;
    short ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid output format test failed | expected return code "
               "%d, got %d\n",
               RTN_ERROR, ret);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: invalid output format test passed\n");

    return failed;
}

int test_invalid_scale_factor(void)
{
    int failed = 0;
    options_t* opts = make_valid_options();
    opts->scale_factor = MIN_SCALE_FACTOR - 0.1f;
    short ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid scale factor test failed | expected return code "
               "%d, got %d\n",
               RTN_ERROR, ret);
        failed++;
    }

    opts->scale_factor = MAX_SCALE_FACTOR + 0.1f;
    ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid scale factor test failed | expected return code "
               "%d, got %d\n",
               RTN_ERROR, ret);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: invalid scale factor test passed\n");

    return failed;
}

int test_invalid_resize_height(void)
{
    int failed = 0;
    options_t* opts = make_valid_options();
    opts->resize_height = MIN_RESIZE_HEIGHT - 1;
    short ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid resize height test failed | expected return code "
               "%d, got %d\n",
               RTN_ERROR, ret);
        failed++;
    }

    opts->resize_height = MAX_RESIZE_HEIGHT + 1;
    ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid resize height test failed | expected return code "
               "%d, got %d\n",
               RTN_ERROR, ret);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: invalid resize height test passed\n");

    return failed;
}

int test_invalid_resize_width(void)
{
    int failed = 0;
    options_t* opts = make_valid_options();
    opts->resize_width = MIN_RESIZE_WIDTH - 1;
    short ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid resize width test failed | expected return code "
               "%d, got %d\n",
               RTN_ERROR, ret);
        failed++;
    }

    opts->resize_width = MAX_RESIZE_WIDTH + 1;
    ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid resize width test failed | expected return code "
               "%d, got %d\n",
               RTN_ERROR, ret);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: invalid resize width test passed\n");

    return failed;
}

int test_invalid_image_quality(void)
{
    int failed = 0;
    options_t* opts = make_valid_options();
    opts->image_quality = MIN_IMAGE_QUALITY - 1;
    short ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid image quality test failed | expected return code "
               "%d, got %d\n",
               RTN_ERROR, ret);
        failed++;
    }

    opts->image_quality = MAX_IMAGE_QUALITY + 1;
    ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: invalid image quality test failed | expected return code "
               "%d, got %d\n",
               RTN_ERROR, ret);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: invalid image quality test passed\n");

    return failed;
}

int test_debug_options(void)
{
    int failed = 0;
    options_t* opts = make_valid_options();
    opts->debug = 1;
    opts->debug_step = 0;
    short ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) validate_options: debug options test failed | expected return code %d, got %d\n",
            RTN_ERROR, ret);
        failed++;
    }

    opts->debug_step = 1;
    opts->debug_dir = "a";
    ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) validate_options: debug options test failed | expected return code %d, got %d\n",
            RTN_ERROR, ret);
        failed++;
    }

    opts->debug_dir = "/unlikely_debug_dir_that_should_not_exist";
    ret = validate_options(opts);
    if (ret != RTN_ERROR)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) validate_options: debug options test failed | expected return code %d, got %d\n",
            RTN_ERROR, ret);
        failed++;
    }

    opts->debug_dir = DEFAULT_DEBUG_DIR;
    ret = validate_options(opts);
    if (!(ret == RTN_SUCCESS || ret == RTN_ERROR))
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) validate_options: debug options test failed | expected return code %d or %d, "
               "got %d\n",
               RTN_SUCCESS, RTN_ERROR, ret);
        failed++;
    }

    free(opts);
    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) validate_options: debug options test passed\n");

    return failed;
}

int test_validate_options(void)
{
    int failed = 0;
    failed += test_valid_options();
    failed += test_null_options();
    failed += test_help_shortcut();
    failed += test_version_shortcut();
    failed += test_debug_shortcut();
    failed += test_invalid_rtsp_url();
    failed += test_invalid_timeout();
    failed += test_invalid_output_file_path();
    failed += test_invalid_output_file_fd();
    failed += test_invalid_exposure();
    failed += test_invalid_output_format();
    failed += test_invalid_scale_factor();
    failed += test_invalid_resize_height();
    failed += test_invalid_resize_width();
    failed += test_invalid_image_quality();
    failed += test_debug_options();
    return failed;
}