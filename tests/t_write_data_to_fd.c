/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | t_write_data_to_fd.c
    ::  ::          ::  ::    Created  | 2025-06-24
          ::::  ::::          Modified | 2025-06-24

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "errors.h"
#include "utilities.h"

int test_write_data_to_fd_valid()
{
    char tmpfile[] = "/tmp/test_write_data_to_fd_XXXXXX";
    int fd = mkstemp(tmpfile);
    if (fd < 0)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) write_data_to_fd: mkstemp failed\n");
        return 1;
    }

    const char* msg = "Hello, world!";
    ssize_t written = write_data_to_fd(fd, msg, strlen(msg));
    if (written != (ssize_t)strlen(msg))
    {
        close(fd);
        unlink(tmpfile);
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) write_data_to_fd: write failed\n");
        return 1;
    }

    lseek(fd, 0, SEEK_SET);
    char buf[32] = {0};
    ssize_t r = read(fd, buf, sizeof(buf));
    if (r != (ssize_t)strlen(msg) || memcmp(buf, msg, strlen(msg)) != 0)
    {
        close(fd);
        unlink(tmpfile);
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) write_data_to_fd: read failed\n");
        return 1;
    }

    close(fd);
    unlink(tmpfile);
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) write_data_to_fd: valid write test passed\n");
    return 0;
}

int test_write_data_to_fd_invalid_fd()
{
    const char* msg = "test";
    ssize_t written = write_data_to_fd(-1, msg, strlen(msg));
    if (written != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) write_data_to_fd: invalid fd test failed\n");
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) write_data_to_fd: invalid fd test passed\n");
    return 0;
}

int test_write_data_to_fd_null_buf()
{
    ssize_t written = write_data_to_fd(STDOUT_FILENO, NULL, 10);
    if (written != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) write_data_to_fd: null buf test failed\n");
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) write_data_to_fd: null buf test passed\n");
    return 0;
}

int test_write_data_to_fd_zero_size()
{
    const char* msg = "test";
    ssize_t written = write_data_to_fd(STDOUT_FILENO, msg, 0);
    if (written != RTN_ERROR)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) write_data_to_fd: zero size test failed\n");
        return 1;
    }

    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) write_data_to_fd: zero size test passed\n");
    return 0;
}

int test_write_data_to_fd()
{
    return test_write_data_to_fd_valid() + test_write_data_to_fd_invalid_fd() +
           test_write_data_to_fd_null_buf() + test_write_data_to_fd_zero_size();
}