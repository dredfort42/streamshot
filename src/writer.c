/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | writer.c
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-06

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "writer.h"

ssize_t write_data_to_fd(int fd, const void* buf, size_t buf_size)
{
    if (fd < 0 || buf == NULL || buf_size == 0)
    {
        write(STDERR_FILENO, "(f) write_data_to_fd | " ERROR_INVALID_ARGUMENTS, strlen("(f) write_data_to_fd | " ERROR_INVALID_ARGUMENTS));
        return RTN_ERROR;
    }

    ssize_t total_written = 0;
    const uint8_t* ptr = (const uint8_t*)buf;
    while (buf_size > 0)
    {
        ssize_t written = write(fd, ptr, buf_size);
        if (written < 0)
        {
            write(STDERR_FILENO, "(f) write_data_to_fd | " ERROR_FAILED_TO_WRITE_FD, strlen("(f) write_data_to_fd | " ERROR_FAILED_TO_WRITE_FD));
            return RTN_ERROR;
        }
        total_written += written;
        ptr += written;
        buf_size -= written;
    }

    return total_written;
}

ssize_t write_msg_to_fd(int fd, const char* msg)
{
    if (fd < 0 || msg == NULL)
    {
        write(STDERR_FILENO, "(f) write_msg_to_fd | " ERROR_INVALID_ARGUMENTS, strlen("(f) write_msg_to_fd | " ERROR_INVALID_ARGUMENTS));
        return RTN_ERROR;
    }

    size_t msg_len = strlen(msg);
    ssize_t total_written = write_data_to_fd(fd, msg, msg_len);
    if (total_written < 0)
        return RTN_ERROR;

    return total_written;
}

char* normalize_file_path(const char* file_path)
{
    if (file_path == NULL || strlen(file_path) < 3)
        return NULL;

    char* normalized_path = strdup(file_path);
    if (normalized_path == NULL)
        return NULL;

    for (size_t i = 0; i < strlen(file_path); i++)
    {
        if (!((file_path[i] >= 'A' && file_path[i] <= 'Z') || (file_path[i] >= 'a' && file_path[i] <= 'z') || (file_path[i] >= '0' && file_path[i] <= '9') ||
              file_path[i] == '_' || file_path[i] == '-' || file_path[i] == '.' || file_path[i] == '/'))
            normalized_path[i] = '_';
        else
            normalized_path[i] = file_path[i];
    }

    return normalized_path;
}

ssize_t write_data_to_file(const char* file_path, const void* buf, size_t buf_size)
{
    if (file_path == NULL || buf == NULL || buf_size == 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) write_data_to_file | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    char* normalized_path = normalize_file_path(file_path);
    if (normalized_path == NULL)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) write_data_to_file | " ERROR_INVALID_ARGUMENTS);
        return RTN_ERROR;
    }

    FILE* file = fopen(normalized_path, "wb");
    if (!file)
    {
        free(normalized_path);
        write_msg_to_fd(STDERR_FILENO, "(f) write_data_to_file | " ERROR_FAILED_TO_OPEN_FILE);
        return RTN_ERROR;
    }

    size_t total_written = 0;
    const uint8_t* ptr = (const uint8_t*)buf;
    while (buf_size > 0)
    {
        size_t written = fwrite(ptr, 1, buf_size, file);
        if (written < 1)
        {
            fclose(file);
            free(normalized_path);
            write_msg_to_fd(STDERR_FILENO, "(f) write_data_to_file | " ERROR_FAILED_TO_WRITE_FILE);
            return RTN_ERROR;
        }
        total_written += written;
        ptr += written;
        buf_size -= written;
    }

    fclose(file);
    free(normalized_path);

    return total_written;
}