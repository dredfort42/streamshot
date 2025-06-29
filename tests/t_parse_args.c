/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | t_parse_args.c
    ::  ::          ::  ::    Created  | 2025-06-29
          ::::  ::::          Modified | 2025-06-29

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "options.h"
#include "utilities.h"

int _test_flag(int argc, char* test_name, char* argv[], int (*check_function)(options_t*),
               short expected_return_code)
{
    options_t* opts = (options_t*)malloc(sizeof(options_t));
    if (!opts)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_options | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        return 1;
    }

    opts->rtsp_url = NULL;
    opts->timeout_sec = DEFAULT_TIMEOUT_SEC;
    opts->output_file_path = NULL;
    opts->output_file_fd = -1;
    opts->exposure_sec = DEFAULT_EXPOSURE_SEC;
    opts->output_format = DEFAULT_OUTPUT_FORMAT;
    opts->scale_factor = DEFAULT_SCALE_FACTOR;
    opts->resize_height = 0;
    opts->resize_width = 0;
    opts->image_quality = DEFAULT_IMAGE_QUALITY;
    opts->debug = 0;
    opts->debug_step = DEFAULT_DEBUG_STEP;
    opts->debug_dir = strdup(DEFAULT_DEBUG_DIR);
    if (!opts->debug_dir)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) _init_options | " ERROR_FAILED_TO_ALLOCATE_MEMORY "\n");
        free(opts);
        return 1;
    }
    opts->help = 0;
    opts->version = 0;

    short ret = parse_args(argc, argv, opts);
    if (ret != 0 && ret != expected_return_code)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: %s test failed | expected return code %d, got %d\n",
               test_name, expected_return_code, ret);
        free_options(opts);
        return 1;
    }

    int check = check_function(opts);

    free_options(opts);
    return check;
}

int check_version_flag(options_t* opts)
{
    if (!opts || opts->version != 1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: version flag test failed | expected options.version == 1, "
               "got %d\n",
               opts->version);
        return 1;
    }

    return 0;
}

int test_version_flag(void)
{
    int failed = 0;

    char* argv[] = {"prog", "-v"};
    failed += _test_flag(2, "version short flag", argv, check_version_flag, RTN_SUCCESS);

    char* argv_long[] = {"prog", "--version"};
    failed += _test_flag(2, "version long flag", argv_long, check_version_flag, RTN_SUCCESS);

    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: version flag test passed\n");

    return failed;
}

int check_help_flag(options_t* opts)
{
    if (!opts || opts->help != 1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: help flag test failed | expected options.help == 1, "
               "got %d\n",
               opts->help);
        return 1;
    }

    return 0;
}

int test_help_flag(void)
{
    int failed = 0;

    char* argv[] = {"prog", "-h"};
    failed += _test_flag(2, "help short flag", argv, check_help_flag, RTN_SUCCESS);

    char* argv_long[] = {"prog", "--help"};
    failed += _test_flag(2, "help long flag", argv_long, check_help_flag, RTN_SUCCESS);

    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: help flag test passed\n");

    return failed;
}

int check_input_url(options_t* opts)
{
    if (!opts || !opts->rtsp_url || strcmp(opts->rtsp_url, "rtsp://example.com/stream") != 0)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: input url test failed | expected rtsp_url to match\n");
        return 1;
    }

    return 0;
}

int check_empty_input_url(options_t* opts)
{
    if (!opts || opts->rtsp_url)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: empty input url test failed | expected rtsp_url to be empty\n");
        return 1;
    }

    return 0;
}

int test_input_url(void)
{
    int failed = 0;

    char* argv[] = {"prog", "-i", "rtsp://example.com/stream"};
    failed += _test_flag(3, "input url short flag", argv, check_input_url, RTN_SUCCESS);

    char* argv_long[] = {"prog", "--input", "rtsp://example.com/stream"};
    failed += _test_flag(3, "input url long flag", argv_long, check_input_url, RTN_SUCCESS);

    char* argv_equals[] = {"prog", "-i=rtsp://example.com/stream"};
    failed += _test_flag(2, "input url short flag with equals", argv_equals, check_input_url,
                         RTN_SUCCESS);

    char* argv_long_equals[] = {"prog", "--input=rtsp://example.com/stream"};
    failed += _test_flag(2, "input url long flag with equals", argv_long_equals, check_input_url,
                         RTN_SUCCESS);

    char* argv_no_input[] = {"prog", "-i"};
    failed += _test_flag(2, "no input url", argv_no_input, check_empty_input_url, RTN_ERROR);

    char* argv_empty_input[] = {"prog", "-i", ""};
    failed += _test_flag(3, "empty input url", argv_empty_input, check_empty_input_url, RTN_ERROR);

    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: input url flag test passed\n");

    return failed;
}

int check_output_file_and_fd(options_t* opts)
{
    int failed = 0;

    if (!opts || !opts->output_file_path || strcmp(opts->output_file_path, "out.jpg") != 0)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output file test failed | expected output_file_path to match\n");
        failed++;
    }

    if (!opts || opts->output_file_fd != 5)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output fd test failed | expected output_file_fd to be 5\n");
        failed++;
    }

    return failed;
}

int check_no_output_file_and_fd(options_t* opts)
{
    if (!opts && opts->output_file_path && opts->output_file_fd != -1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: no output file/fd test failed | expected no output file or fd\n");
        return 1;
    }

    return 0;
}

int test_output_file_and_fd(void)
{
    int failed = 0;

    char* argv[] = {"prog", "-o", "out.jpg", "-O", "5"};
    failed +=
        _test_flag(5, "output file/fd short flag", argv, check_output_file_and_fd, RTN_SUCCESS);

    char* argv_long[] = {"prog", "--output-file", "out.jpg", "--output-fd", "5"};
    failed +=
        _test_flag(5, "output file/fd long flag", argv_long, check_output_file_and_fd, RTN_SUCCESS);

    char* argv_equals[] = {"prog", "-o=out.jpg", "-O=5"};
    failed += _test_flag(3, "output file/fd short flag with equals", argv_equals,
                         check_output_file_and_fd, RTN_SUCCESS);

    char* argv_long_equals[] = {"prog", "--output-file=out.jpg", "--output-fd=5"};
    failed += _test_flag(3, "output file/fd long flag with equals", argv_long_equals,
                         check_output_file_and_fd, RTN_SUCCESS);

    char* argv_no_output[] = {"prog", "-o"};
    failed +=
        _test_flag(2, "no output file", argv_no_output, check_no_output_file_and_fd, RTN_ERROR);

    char* argv_no_output_fd[] = {"prog", "-O"};
    failed +=
        _test_flag(2, "no output fd", argv_no_output_fd, check_no_output_file_and_fd, RTN_ERROR);

    char* argv_empty_output[] = {"prog", "-o", ""};
    failed += _test_flag(3, "empty output file", argv_empty_output, check_no_output_file_and_fd,
                         RTN_ERROR);

    char* argv_empty_output_fd[] = {"prog", "-O", ""};
    failed += _test_flag(3, "empty output fd", argv_empty_output_fd, check_no_output_file_and_fd,
                         RTN_ERROR);

    char* argv_no_output_file_and_fd_debug[] = {"prog", "-d"};
    failed += _test_flag(2, "no output file and fd", argv_no_output_file_and_fd_debug,
                         check_no_output_file_and_fd, RTN_SUCCESS);

    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) parse_args: output file/fd flag test passed\n");

    return failed;
}

int check_timeout_and_exposure(options_t* opts)
{
    int failed = 0;

    if (opts->timeout_sec != 10)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: timeout test failed | expected timeout_sec to be 10\n");
        failed++;
    }

    if (opts->exposure_sec != 3)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: exposure test failed | expected exposure_sec to be 3\n");
        failed++;
    }

    return failed;
}

int check_no_timeout_and_exposure(options_t* opts)
{
    if (!opts || opts->timeout_sec != DEFAULT_TIMEOUT_SEC ||
        opts->exposure_sec != DEFAULT_EXPOSURE_SEC)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) parse_args: no timeout/exposure test failed | timeout_sec got %d, expected %d; "
            "exposure_sec got %d, expected %d\n",
            opts->timeout_sec, DEFAULT_TIMEOUT_SEC, opts->exposure_sec, DEFAULT_EXPOSURE_SEC);
        return 1;
    }

    return 0;
}

int check_invalid_type_timeout_and_exposure(options_t* opts)
{
    int failed = 0;
    if (!opts || opts->timeout_sec)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: timeout test failed | expected timeout_sec to be %d, got %d\n",
               0, opts->timeout_sec);
        failed++;
    }

    if (!opts || opts->exposure_sec)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: exposure test failed | expected exposure_sec to be %d, got %d\n",
               0, opts->exposure_sec);
        failed++;
    }

    return failed;
}

int test_timeout_and_exposure(void)
{
    int failed = 0;

    char* argv[] = {"prog", "-t", "10", "-e", "3"};
    failed +=
        _test_flag(5, "timeout/exposure short flag", argv, check_timeout_and_exposure, RTN_SUCCESS);

    char* argv_long[] = {"prog", "--timeout", "10", "--exposure", "3"};
    failed += _test_flag(5, "timeout/exposure long flag", argv_long, check_timeout_and_exposure,
                         RTN_SUCCESS);

    char* argv_equals[] = {"prog", "-t=10", "-e=3"};
    failed += _test_flag(3, "timeout/exposure short flag with equals", argv_equals,
                         check_timeout_and_exposure, RTN_SUCCESS);

    char* argv_long_equals[] = {"prog", "--timeout=10", "--exposure=3"};
    failed += _test_flag(3, "timeout/exposure long flag with equals", argv_long_equals,
                         check_timeout_and_exposure, RTN_SUCCESS);

    char* argv_no_timeout[] = {"prog", "-t"};
    failed +=
        _test_flag(2, "no timeout", argv_no_timeout, check_no_timeout_and_exposure, RTN_ERROR);

    char* argv_no_exposure[] = {"prog", "-e"};
    failed +=
        _test_flag(2, "no exposure", argv_no_exposure, check_no_timeout_and_exposure, RTN_ERROR);

    char* argv_empty_timeout[] = {"prog", "-t", ""};
    failed += _test_flag(3, "empty timeout", argv_empty_timeout, check_no_timeout_and_exposure,
                         RTN_ERROR);

    char* argv_empty_exposure[] = {"prog", "-e", ""};
    failed += _test_flag(3, "empty exposure", argv_empty_exposure, check_no_timeout_and_exposure,
                         RTN_ERROR);

    char* argv_invalid_timeout_and_exposure[] = {"prog", "-t", "invalid", "-e", "invalid"};
    failed += _test_flag(5, "invalid timeout/exposure", argv_invalid_timeout_and_exposure,
                         check_invalid_type_timeout_and_exposure, RTN_ERROR);

    char* argv_no_timeout_and_exposure_debug[] = {"prog", "-d"};
    failed += _test_flag(2, "no timeout and exposure", argv_no_timeout_and_exposure_debug,
                         check_no_timeout_and_exposure, RTN_SUCCESS);

    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: timeout/exposure test passed\n");

    return failed;
}

int check_output_format_jpg(options_t* opts)
{
    if (opts->output_format != IMAGE_FORMAT_JPG)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output format test failed | expected output_format to be "
               "JPG\n");
        return 1;
    }

    return 0;
}

int check_output_format_jpeg(options_t* opts)
{
    if (opts->output_format != IMAGE_FORMAT_JPEG)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output format test failed | expected output_format to be "
               "JPEG\n");
        return 1;
    }

    return 0;
}

int check_output_format_png(options_t* opts)
{
    if (opts->output_format != IMAGE_FORMAT_PNG)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output format test failed | expected output_format to be "
               "PNG\n");
        return 1;
    }

    return 0;
}

int check_output_format_ppm(options_t* opts)
{
    if (opts->output_format != IMAGE_FORMAT_PPM)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output format test failed | expected output_format to be "
               "PPM\n");
        return 1;
    }

    return 0;
}

int check_output_format_unknown(options_t* opts)
{
    if (opts->output_format != IMAGE_FORMAT_UNKNOWN)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output format test failed | expected output_format to be "
               "UNKNOWN\n");
        return 1;
    }

    return 0;
}

int test_output_format(void)
{
    int failed = 0;

    char* argv_jpg[] = {"prog", "-f", "jpg"};
    failed += _test_flag(3, "output format jpg short flag", argv_jpg, check_output_format_jpg,
                         RTN_SUCCESS);

    char* argv_jpg_case[] = {"prog", "--output-format", "JpG"};
    failed += _test_flag(3, "output format jpg case-insensitive long flag", argv_jpg_case,
                         check_output_format_jpg, RTN_SUCCESS);

    char* argv_jpg_long[] = {"prog", "--output-format", "jpg"};
    failed += _test_flag(3, "output format jpg long flag", argv_jpg_long, check_output_format_jpg,
                         RTN_SUCCESS);

    char* argv_jpg_equals[] = {"prog", "-f=jpg"};
    failed += _test_flag(2, "output format jpg short flag with equals", argv_jpg_equals,
                         check_output_format_jpg, RTN_SUCCESS);

    char* argv_jpg_long_equals[] = {"prog", "--output-format=jpg"};
    failed += _test_flag(2, "output format jpg long flag with equals", argv_jpg_long_equals,
                         check_output_format_jpg, RTN_SUCCESS);

    char* argv_jpeg[] = {"prog", "-f", "jpeg"};
    failed += _test_flag(3, "output format jpeg short flag", argv_jpeg, check_output_format_jpeg,
                         RTN_SUCCESS);

    char* argv_jpeg_case[] = {"prog", "--output-format", "JpEg"};
    failed += _test_flag(3, "output format jpeg case-insensitive long flag", argv_jpeg_case,
                         check_output_format_jpeg, RTN_SUCCESS);

    char* argv_jpeg_long[] = {"prog", "--output-format", "jpeg"};
    failed += _test_flag(3, "output format jpeg long flag", argv_jpeg_long,
                         check_output_format_jpeg, RTN_SUCCESS);

    char* argv_jpeg_equals[] = {"prog", "-f=jpeg"};
    failed += _test_flag(2, "output format jpeg short flag with equals", argv_jpeg_equals,
                         check_output_format_jpeg, RTN_SUCCESS);

    char* argv_jpeg_long_equals[] = {"prog", "--output-format=jpeg"};
    failed += _test_flag(2, "output format jpeg long flag with equals", argv_jpeg_long_equals,
                         check_output_format_jpeg, RTN_SUCCESS);

    char* argv_png[] = {"prog", "-f", "png"};
    failed += _test_flag(3, "output format png short flag", argv_png, check_output_format_png,
                         RTN_SUCCESS);

    char* argv_png_case[] = {"prog", "--output-format", "PnG"};
    failed += _test_flag(3, "output format png case-insensitive long flag", argv_png_case,
                         check_output_format_png, RTN_SUCCESS);

    char* argv_png_long[] = {"prog", "--output-format", "png"};
    failed += _test_flag(3, "output format png long flag", argv_png_long, check_output_format_png,
                         RTN_SUCCESS);

    char* argv_png_equals[] = {"prog", "-f=png"};
    failed += _test_flag(2, "output format png short flag with equals", argv_png_equals,
                         check_output_format_png, RTN_SUCCESS);

    char* argv_png_long_equals[] = {"prog", "--output-format=png"};
    failed += _test_flag(2, "output format png long flag with equals", argv_png_long_equals,
                         check_output_format_png, RTN_SUCCESS);

    char* argv_ppm[] = {"prog", "-f", "ppm"};
    failed += _test_flag(3, "output format ppm short flag", argv_ppm, check_output_format_ppm,
                         RTN_SUCCESS);

    char* argv_ppm_case[] = {"prog", "--output-format", "PpM"};
    failed += _test_flag(3, "output format ppm case-insensitive long flag", argv_ppm_case,
                         check_output_format_ppm, RTN_SUCCESS);

    char* argv_ppm_long[] = {"prog", "--output-format", "ppm"};
    failed += _test_flag(3, "output format ppm long flag", argv_ppm_long, check_output_format_ppm,
                         RTN_SUCCESS);

    char* argv_ppm_equals[] = {"prog", "-f=ppm"};
    failed += _test_flag(2, "output format ppm short flag with equals", argv_ppm_equals,
                         check_output_format_ppm, RTN_SUCCESS);

    char* argv_ppm_long_equals[] = {"prog", "--output-format=ppm"};
    failed += _test_flag(2, "output format ppm long flag with equals", argv_ppm_long_equals,
                         check_output_format_ppm, RTN_SUCCESS);

    char* argv_unknown[] = {"prog", "-f", "unknown"};
    failed += _test_flag(3, "output format unknown short flag", argv_unknown,
                         check_output_format_unknown, RTN_SUCCESS);

    char* argv_unknown_case[] = {"prog", "--output-format", "UnKnOwN"};
    failed += _test_flag(3, "output format unknown case-insensitive long flag", argv_unknown_case,
                         check_output_format_unknown, RTN_SUCCESS);

    char* argv_unknown_long[] = {"prog", "--output-format", "unknown"};
    failed += _test_flag(3, "output format unknown long flag", argv_unknown_long,
                         check_output_format_unknown, RTN_SUCCESS);

    char* argv_unknown_equals[] = {"prog", "-f=unknown"};
    failed += _test_flag(2, "output format unknown short flag with equals", argv_unknown_equals,
                         check_output_format_unknown, RTN_SUCCESS);

    char* argv_unknown_long_equals[] = {"prog", "--output-format=unknown"};
    failed += _test_flag(2, "output format unknown long flag with equals", argv_unknown_long_equals,
                         check_output_format_unknown, RTN_SUCCESS);

    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: output format test passed\n");

    return failed;
}

int check_scale_and_resize(options_t* opts)
{
    if (!opts || opts->scale_factor != 0.5f || opts->resize_height != -1 ||
        opts->resize_width != -1)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) parse_args: scale/resize test failed | expected scale_factor to be 0.5 got %f, "
            "resize_height to be -1 got %d, resize_width to be -1 got %d\n",
            opts->scale_factor, opts->resize_height, opts->resize_width);
        return 1;
    }

    return 0;
}

int test_scale_and_resize(void)
{
    int failed = 0;

    char* argv[] = {"prog", "-s", "0.5", "-h", "100", "-w", "200"};
    failed += _test_flag(7, "scale/resize short flag", argv, check_scale_and_resize, RTN_SUCCESS);

    char* argv_long[] = {"prog", "--scale",        "0.5", "--resize-height",
                         "100",  "--resize-width", "200"};
    failed +=
        _test_flag(7, "scale/resize long flag", argv_long, check_scale_and_resize, RTN_SUCCESS);

    char* argv_equals[] = {"prog", "-s=0.5", "-h=100", "-w=200"};
    failed += _test_flag(4, "scale/resize short flag with equals", argv_equals,
                         check_scale_and_resize, RTN_SUCCESS);

    char* argv_long_equals[] = {"prog", "--scale=0.5", "--resize-height=100", "--resize-width=200"};
    failed += _test_flag(4, "scale/resize long flag with equals", argv_long_equals,
                         check_scale_and_resize, RTN_SUCCESS);

    char* argv_no_resize[] = {"prog", "-s", "0.5"};
    failed +=
        _test_flag(3, "scale only short flag", argv_no_resize, check_scale_and_resize, RTN_SUCCESS);

    char* argv_invalid_resize[] = {"prog", "--scale",        "0.5", "--resize-height",
                                   "0",    "--resize-width", "-2"};
    failed += _test_flag(7, "scale with invalid resize long flag", argv_invalid_resize,
                         check_scale_and_resize, RTN_ERROR);

    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: scale/resize test passed\n");

    return failed;
}

int check_image_quality(options_t* opts)
{
    if (!opts || opts->image_quality != 42)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: image quality test failed | expected image_quality to be 42, "
               "got %d\n",
               opts->image_quality);
        return 1;
    }

    return 0;
}

int test_image_quality(void)
{
    int failed = 0;

    char* argv[] = {"prog", "-q", "42"};
    failed += _test_flag(3, "image quality short flag", argv, check_image_quality, RTN_SUCCESS);

    char* argv_long[] = {"prog", "--image-quality", "42"};
    failed += _test_flag(3, "image quality long flag", argv_long, check_image_quality, RTN_SUCCESS);

    char* argv_equals[] = {"prog", "-q=42"};
    failed += _test_flag(2, "image quality short flag with equals", argv_equals,
                         check_image_quality, RTN_SUCCESS);

    char* argv_long_equals[] = {"prog", "--image-quality=42"};
    failed += _test_flag(2, "image quality long flag with equals", argv_long_equals,
                         check_image_quality, RTN_SUCCESS);

    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: image quality test passed\n");

    return failed;
}

int check_debug_flags(options_t* opts)
{
    int failed = 0;

    if (!opts || opts->debug != 1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug flag test failed | expected debug to be 1 got %d\n",
               opts->debug);
        failed++;
    }

    if (!opts || opts->debug_step != 2)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug step test failed | expected debug_step to be 2 got %d\n",
               opts->debug_step);
        failed++;
    }

    if (!opts || !(opts->debug_dir && strcmp(opts->debug_dir, "tmp") == 0))
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug dir test failed | expected debug_dir to match tmp got %s\n",
               opts->debug_dir);
        failed++;
    }

    return failed;
}

int check_no_debug_flags(options_t* opts)
{
    int failed = 0;

    if (!opts || opts->debug)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug flag test failed | expected debug to be 0 got %d\n",
               opts->debug);
        failed++;
    }

    if (!opts || opts->debug_step != DEFAULT_DEBUG_STEP)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug step test failed | expected debug_step to be %d got %d\n",
               DEFAULT_DEBUG_STEP, opts->debug_step);
        failed++;
    }

    if (!opts || strcmp(opts->debug_dir, DEFAULT_DEBUG_DIR))
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug dir test failed | expected debug_dir to be %s, got %s\n",
               DEFAULT_DEBUG_DIR, opts->debug_dir);
        failed++;
    }

    return failed;
}

int check_debug_flag_only(options_t* opts)
{
    int failed = 0;

    if (!opts || opts->debug != 1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug flag test failed | expected debug to be 1 got %d\n",
               opts->debug);
        failed++;
    }

    if (!opts || opts->debug_step != DEFAULT_DEBUG_STEP)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug step test failed | expected debug_step to be %d got %d\n",
               DEFAULT_DEBUG_STEP, opts->debug_step);
        failed++;
    }

    if (!opts || !opts->debug_dir || strcmp(opts->debug_dir, DEFAULT_DEBUG_DIR))
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug dir test failed | expected debug_dir to be %s, got %s\n",
               DEFAULT_DEBUG_DIR, opts->debug_dir ? opts->debug_dir : "NULL");
        failed++;
    }

    return failed;
}

int test_debug_flags(void)
{
    int failed = 0;

    char* argv[] = {"prog", "-d", "--debug-step", "2", "--debug-dir", "tmp"};
    failed += _test_flag(6, "debug flags", argv, check_debug_flags, RTN_SUCCESS);

    char* argv_long[] = {"prog", "--debug", "--debug-step", "2", "--debug-dir", "tmp"};
    failed += _test_flag(6, "debug flags long", argv_long, check_debug_flags, RTN_SUCCESS);

    char* argv_equals[] = {"prog", "-d", "--debug-step=2", "--debug-dir=tmp"};
    failed += _test_flag(4, "debug flags with equals", argv_equals, check_debug_flags, RTN_SUCCESS);

    char* argv_long_equals[] = {"prog", "--debug", "--debug-step=2", "--debug-dir=tmp"};
    failed += _test_flag(4, "debug flags long with equals", argv_long_equals, check_debug_flags,
                         RTN_SUCCESS);

    char* argv_debug_only[] = {"prog", "-d"};
    failed +=
        _test_flag(2, "debug flags only", argv_debug_only, check_debug_flag_only, RTN_SUCCESS);

    char* argv_debug_only_long[] = {"prog", "--debug"};
    failed += _test_flag(2, "debug flags only long", argv_debug_only_long, check_debug_flag_only,
                         RTN_SUCCESS);

    char* argv_no_debug[] = {"prog", "-i", "rtsp://example.com/stream"};
    failed += _test_flag(3, "no debug flags", argv_no_debug, check_no_debug_flags, RTN_SUCCESS);

    if (!failed)
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: debug flags test passed\n");

    return failed;
}

int check_invalid_flag(options_t* opts)
{
    if (!opts || opts->rtsp_url != NULL || opts->timeout_sec != DEFAULT_TIMEOUT_SEC ||
        opts->output_file_path != NULL || opts->output_file_fd != -1 ||
        opts->exposure_sec != DEFAULT_EXPOSURE_SEC ||
        opts->output_format != DEFAULT_OUTPUT_FORMAT ||
        opts->scale_factor != DEFAULT_SCALE_FACTOR || opts->resize_height != 0 ||
        opts->resize_width != 0 || opts->image_quality != DEFAULT_IMAGE_QUALITY ||
        opts->debug != 0 || opts->debug_step != DEFAULT_DEBUG_STEP ||
        strcmp(opts->debug_dir, DEFAULT_DEBUG_DIR))
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: invalid flag test failed | expected no options to be set, got: \n"
               "version: %d, help: %d, rtsp_url: %s, output_file_path: %s, output_file_fd: %d, "
               "scale_factor: %f, "
               "resize_height: %d, resize_width: %d, image_quality: %d, "
               "debug: %d, debug_step: %d, debug_dir: %s\n",
               opts->version, opts->help, opts->rtsp_url ? opts->rtsp_url : "NULL",
               opts->output_file_path ? opts->output_file_path : "NULL", opts->output_file_fd,
               opts->scale_factor, opts->resize_height, opts->resize_width, opts->image_quality,
               opts->debug, opts->debug_step, opts->debug_dir ? opts->debug_dir : "NULL");
        return 1;
    }

    return 0;
}

int test_invalid_flag(void)
{
    char* argv[] = {"prog", "--unknown"};
    if (_test_flag(2, "invalid flag", argv, check_invalid_flag, RTN_ERROR))
        return 1;

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: invalid flag test passed\n");
    return 0;
}

int test_missing_value(void)
{
    char* argv[] = {"prog", "-i", "-d"};
    if (_test_flag(3, "missing value", argv, check_invalid_flag, RTN_ERROR))
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: missing value test failed | expected 0 return code\n");
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: missing value test passed\n");
    return 0;
}

int test_parse_args(void)
{
    int failed = 0;
    failed += test_version_flag();
    failed += test_help_flag();
    failed += test_input_url();
    failed += test_output_file_and_fd();
    failed += test_timeout_and_exposure();
    failed += test_output_format();
    failed += test_scale_and_resize();
    failed += test_image_quality();
    failed += test_debug_flags();
    failed += test_invalid_flag();
    failed += test_missing_value();
    return failed;
}