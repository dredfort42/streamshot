/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | writer.c
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-05

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "writer.h"

ssize_t write_to_fd(int fd, const void* buf, size_t* count)
{
    if (fd < 0 || buf == NULL)
    {
        write(STDERR_FILENO, ERROR_INVALID_ARGUMENTS, strlen(ERROR_INVALID_ARGUMENTS));
        return RTN_ERROR;
    }

    size_t bytes_to_write = 0;
    if (count && *count > 0)
    {
        bytes_to_write = *count;
    }
    else if (!count)
    {
        bytes_to_write = strlen((const char*)buf);
    }

    if (!bytes_to_write)
    {
        write(STDERR_FILENO, ERROR_NO_DATA_TO_WRITE, strlen(ERROR_NO_DATA_TO_WRITE));
        return RTN_ERROR;
    }

    ssize_t total_written = 0;
    const char* ptr = (const char*)buf;
    while (bytes_to_write > 0)
    {
        ssize_t written = write(fd, ptr, bytes_to_write);
        if (written < 0)
        {
            write(STDERR_FILENO, ERROR_FAILED_TO_WRITE, strlen(ERROR_FAILED_TO_WRITE));
            return RTN_ERROR;
        }
        total_written += written;
        ptr += written;
        bytes_to_write -= written;
    }

    return total_written;
}