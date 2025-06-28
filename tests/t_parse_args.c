
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"
#include "utilities.h"

int test_version_flag(void)
{
    char* argv[] = {"prog", "-v"};
    options_t opts = {0};
    short ret = parse_args(2, argv, &opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: version flag test failed | expected 0 return code\n");
        return 1;
    }

    if (opts.version != 1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: version flag test failed | expected options.version to be 1\n");
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: version flag test passed\n");
    return 0;
}

int test_version_flag_equals(void)
{
    char* argv[] = {"prog", "--version"};
    options_t opts = {0};
    short ret = parse_args(2, argv, &opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: version flag with equals test failed | expected 0 return code\n");
        return 1;
    }

    if (opts.version != 1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: version flag with equals test failed | expected options.version "
               "to be 1\n");
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET
           "] (f) parse_args: version flag with equals test passed\n");
    return 0;
}

int test_help_flag(void)
{
    char* argv[] = {"prog", "-h"};
    options_t opts = {0};
    short ret = parse_args(2, argv, &opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: help flag test failed | expected 0 return code\n");
        return 1;
    }

    if (opts.help != 1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: help flag test failed | expected options.help to be "
               "1\n");
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: help flag test passed\n");
    return 0;
}
int test_help_flag_equals(void)
{
    char* argv[] = {"prog", "--help"};
    options_t opts = {0};
    short ret = parse_args(2, argv, &opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: help flag with equals test failed | expected 0 return code\n");
        return 1;
    }

    if (opts.help != 1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: help flag with equals test failed | expected options.help to be "
               "1\n");
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: help flag with equals test passed\n");
    return 0;
}

int test_input_url(void)
{
    char* argv[] = {"prog", "-i", "rtsp://example.com/stream"};
    options_t opts = {0};
    short ret = parse_args(3, argv, &opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: input url test failed | expected 0 return code\n");
        return 1;
    }

    if (!(opts.rtsp_url && strcmp(opts.rtsp_url, "rtsp://example.com/stream") == 0))
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: input url test failed | expected rtsp_url to match\n");
        free(opts.rtsp_url);
        return 1;
    }

    free(opts.rtsp_url);
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: input url test passed\n");
    return 0;
}

int test_input_url_equals(void)
{
    char* argv[] = {"prog", "--input=rtsp://example.com/stream"};
    options_t opts = {0};
    short ret = parse_args(2, argv, &opts);

    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: input url test with equals failed | expected 0 return code\n");
        return 1;
    }

    if (!(opts.rtsp_url && strcmp(opts.rtsp_url, "rtsp://example.com/stream") == 0))
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) parse_args: input url test with equals failed | expected rtsp_url to match\n");
        free(opts.rtsp_url);
        return 1;
    }

    free(opts.rtsp_url);
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: input url with equals test passed\n");
    return 0;
}

int test_output_file_and_fd(void)
{
    char* argv[] = {"prog", "-o", "out.jpg", "-O", "5"};
    options_t opts = {0};
    short ret = parse_args(5, argv, &opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output file/fd test failed | expected 0 return code\n");
        return 1;
    }

    if (!(opts.output_file_path && strcmp(opts.output_file_path, "out.jpg") == 0))
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) parse_args: output file/fd test failed | expected output_file_path to match\n");
        free(opts.output_file_path);
        return 1;
    }

    if (opts.output_file_fd != 5)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output file/fd test failed | expected output_file_fd to be 5\n");
        free(opts.output_file_path);
        return 1;
    }

    free(opts.output_file_path);
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: output file/fd test passed\n");
    return 0;
}

int test_output_file_and_fd_equals(void)
{
    char* argv[] = {"prog", "--output-file=out.jpg", "--output-fd=5"};
    options_t opts = {0};
    short ret = parse_args(3, argv, &opts);
    if (ret)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) parse_args: output file/fd with equals test failed | expected 0 return code\n");
        return 1;
    }

    if (!(opts.output_file_path && strcmp(opts.output_file_path, "out.jpg") == 0))
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output file/fd with equals test failed | expected "
               "output_file_path to match\n");
        free(opts.output_file_path);
        return 1;
    }

    if (opts.output_file_fd != 5)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output file/fd with equals test failed | expected output_file_fd "
               "to be 5\n");
        free(opts.output_file_path);
        return 1;
    }

    free(opts.output_file_path);
    printf("[" ANSI_GREEN "OK" ANSI_RESET
           "] (f) parse_args: output file/fd with equals test passed\n");
    return 0;
}

// -------
int test_timeout_and_exposure(void)
{
    char* argv[] = {"prog", "--timeout", "10", "--exposure=3"};
    options_t opts = {0};
    short ret = parse_args(5, argv, &opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: timeout/exposure test failed | expected 0 return code\n");
        return 1;
    }
    if (opts.timeout_sec != 10)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: timeout/exposure test failed | expected timeout_sec to be 10\n");
        return 1;
    }
    if (opts.exposure_sec != 3)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: timeout/exposure test failed | expected exposure_sec to be 3\n");
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: timeout/exposure test passed\n");
    return 0;
}

int test_output_format(void)
{
    char* argv[] = {"prog", "-f", "jpeg"};
    options_t opts = {0};
    short ret = parse_args(3, argv, &opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output format test failed | expected 0 return code\n");
        return 1;
    }
    if (opts.output_format != 42)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: output format test failed | expected output_format to be 42\n");
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: output format test passed\n");
    return 0;
}

int test_scale_and_resize(void)
{
    char* argv[] = {"prog", "--scale", "0.5", "--resize-height", "100", "--resize-width", "200"};
    options_t opts = {0};
    opts.resize_height = 0;
    opts.resize_width = 0;
    short ret = parse_args(7, argv, &opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: scale/resize test failed | expected 0 return code\n");
        return 1;
    }
    if (opts.scale_factor != 0.5f)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: scale/resize test failed | expected scale_factor to be 0.5\n");
        return 1;
    }
    if (opts.resize_height != -1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: scale/resize test failed | expected resize_height to be -1\n");
        return 1;
    }
    if (opts.resize_width != -1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: scale/resize test failed | expected resize_width to be -1\n");
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: scale/resize test passed\n");
    return 0;
}

int test_image_quality(void)
{
    char* argv[] = {"prog", "-q", "85"};
    options_t opts = {0};
    short ret = parse_args(3, argv, &opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: image quality test failed | expected 0 return code\n");
        return 1;
    }
    if (opts.image_quality != 85)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: image quality test failed | expected image_quality to be 85\n");
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: image quality test passed\n");
    return 0;
}

int test_debug_flags(void)
{
    char* argv[] = {"prog", "-d", "--debug-step", "2", "--debug-dir", "tmp"};
    options_t opts = {0};
    short ret = parse_args(6, argv, &opts);
    if (ret)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug flags test failed | expected 0 return code\n");
        return 1;
    }
    if (opts.debug != 1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug flags test failed | expected debug to be 1\n");
        free(opts.debug_dir);
        return 1;
    }
    if (opts.debug_step != 2)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug flags test failed | expected debug_step to be 2\n");
        free(opts.debug_dir);
        return 1;
    }
    if (!(opts.debug_dir && strcmp(opts.debug_dir, "tmp") == 0))
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: debug flags test failed | expected debug_dir to match\n");
        free(opts.debug_dir);
        return 1;
    }
    free(opts.debug_dir);
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: debug flags test passed\n");
    return 0;
}

int test_invalid_flag(void)
{
    char* argv[] = {"prog", "--unknown"};
    options_t opts = {0};
    short ret = parse_args(2, argv, &opts);
    if (ret != -1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: invalid flag test failed | expected -1 return code\n");
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: invalid flag test passed\n");
    return 0;
}

int test_missing_value(void)
{
    char* argv[] = {"prog", "-i", "-d"};
    options_t opts = {0};
    short ret = parse_args(3, argv, &opts);
    if (ret != -1)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) parse_args: missing value test failed | expected -1 return code\n");
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) parse_args: missing value test passed\n");
    return 0;
}

int test_parse_args(void)
{
    int failed = 0;
    failed += test_version_flag();
    failed += test_version_flag_equals();
    failed += test_help_flag();
    failed += test_help_flag_equals();
    failed += test_input_url();
    failed += test_input_url_equals();
    failed += test_output_file_and_fd();
    failed += test_output_file_and_fd_equals();
    // failed += test_timeout_and_exposure();
    // failed += test_output_format();
    // failed += test_scale_and_resize();
    // failed += test_image_quality();
    // failed += test_debug_flags();
    // failed += test_invalid_flag();
    // failed += test_missing_value();
    return failed;
}