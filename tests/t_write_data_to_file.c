/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | t_write_data_to_file.c
    ::  ::          ::  ::    Created  | 2025-06-25
          ::::  ::::          Modified | 2025-06-25

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "errors.h"
#include "utilities.h"

// Helper: read file content
static int read_file(const char* path, void* buf, size_t buf_size)
{
    FILE* f = fopen(path, "rb");
    if (!f)
        return -1;

    size_t n = fread(buf, 1, buf_size, f);
    fclose(f);
    return (int)n;
}

int test_write_data_to_file_success(void)
{
    const char* path = "test_output.bin";
    const char data[] = "Hello, world!";
    unlink(path);

    ssize_t written = write_data_to_file(path, data, sizeof(data));
    if (written != (ssize_t)sizeof(data))
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) write_data_to_file: success test failed (written=%zd, expected=%zu)\n",
               written, sizeof(data));
        return 1;
    }

    char buf[64] = {0};
    int read_bytes = read_file(path, buf, sizeof(buf));
    if (read_bytes != (int)sizeof(data) || memcmp(buf, data, sizeof(data)) != 0)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) write_data_to_file: file content mismatch\n");
        unlink(path);
        return 1;
    }

    unlink(path);
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) write_data_to_file: success test passed\n");
    return 0;
}

int test_write_data_to_file_null_path(void)
{
    const char data[] = "abc";
    ssize_t written = write_data_to_file(NULL, data, sizeof(data));
    if (written != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) write_data_to_file: null path test failed\n");
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) write_data_to_file: null path test passed\n");
    return 0;
}

int test_write_data_to_file_null_buf(void)
{
    ssize_t written = write_data_to_file("file.bin", NULL, 10);
    if (written != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) write_data_to_file: null buf test failed\n");
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) write_data_to_file: null buf test passed\n");
    return 0;
}

int test_write_data_to_file_zero_size(void)
{
    const char data[] = "abc";
    ssize_t written = write_data_to_file("file.bin", data, 0);
    if (written != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) write_data_to_file: zero size test failed\n");
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) write_data_to_file: zero size test passed\n");
    return 0;
}

int test_write_data_to_file_open_fail(void)
{
    ssize_t written = write_data_to_file("/nonexistent_dir/file.bin", "abc", 3);
    if (written != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) write_data_to_file: open fail test failed\n");
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) write_data_to_file: open fail test passed\n");
    return 0;
}

int test_write_data_to_file(void)
{
    int failed = 0;
    failed += test_write_data_to_file_success();
    failed += test_write_data_to_file_null_path();
    failed += test_write_data_to_file_null_buf();
    failed += test_write_data_to_file_zero_size();
    failed += test_write_data_to_file_open_fail();
    return failed;
}