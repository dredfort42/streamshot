/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | write.c
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-20

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "errors.h"
#include "utilities.h"

/**
 * @brief Writes the specified buffer to a file descriptor.
 *
 * This function attempts to write exactly `buf_size` bytes from the buffer `buf`
 * to the file descriptor `fd`. It handles partial writes by repeatedly calling
 * the write system call until all data is written or an error occurs.
 *
 * @param fd        The file descriptor to which data will be written.
 * @param buf       Pointer to the buffer containing the data to write.
 * @param buf_size  The number of bytes to write from the buffer.
 *
 * @return On success, returns the total number of written bytes.
 *         On error, returns -1.
 *
 * @note This function does not close the file descriptor.
 *       If a signal interrupts the write operation, the function will retry.
 */
ssize_t write_data_to_fd(int fd, const void* buf, size_t buf_size)
{
    if (fd < 0 || buf == NULL || buf_size == 0)
    {
        write(STDERR_FILENO, "(f) write_data_to_fd | " ERROR_INVALID_ARGUMENTS "\n",
              strlen("(f) write_data_to_fd | " ERROR_INVALID_ARGUMENTS "\n"));
        return RTN_ERROR;
    }

    size_t total_written = 0;
    const uint8_t* ptr = (const uint8_t*)buf;

    while (total_written < buf_size)
    {
        ssize_t written = write(fd, ptr + total_written, buf_size - total_written);

        if (written > 0)
            total_written += written;
        else if (written == 0)
            break;  // EOF reached, no more data to write
        else if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            continue;  // Retry on non-blocking or interrupted write
        else
            goto error;  // Other errors, exit with error
    }

    return (ssize_t)total_written;

error:

    write(STDERR_FILENO, "(f) write_data_to_fd | " ERROR_FAILED_TO_WRITE_FD "\n",
          strlen("(f) write_data_to_fd | " ERROR_FAILED_TO_WRITE_FD "\n"));
    return RTN_ERROR;
}

/**
 * @brief Writes a null-terminated message to the specified file descriptor.
 *
 * This function attempts to write the entire message pointed to by `msg` to the file
 * descriptor `fd`. The message is expected to be a null-terminated string.
 *
 * @param fd   The file descriptor to which the message will be written.
 * @param msg  A pointer to the null-terminated string message to write.
 *
 * @return On success, returns the total number of written bytes.
 *         On error, returns -1.
 *
 * @note This function limits the message size to 4096 bytes for safety.
 *       It checks for null pointers and ensures the message is null-terminated.
 */
ssize_t write_msg_to_fd(int fd, const char* msg)
{
    if (fd < 0 || msg == NULL)
    {
        write(STDERR_FILENO, "(f) write_msg_to_fd | " ERROR_INVALID_ARGUMENTS "\n",
              strlen("(f) write_msg_to_fd | " ERROR_INVALID_ARGUMENTS "\n"));
        return RTN_ERROR;
    }

    if (strchr(msg, '\0') == NULL)
    {
        write(STDERR_FILENO, "(f) write_msg_to_fd | " ERROR_NOT_NULL_TERMINATED "\n",
              strlen("(f) write_msg_to_fd | " ERROR_NOT_NULL_TERMINATED "\n"));
        return RTN_ERROR;
    }

    size_t msg_len = strlen(msg);
    if (msg_len > 4096)
        msg_len = 4096;  // Limit message size for safety

    ssize_t total_written = write_data_to_fd(fd, msg, msg_len);
    if (total_written < 0)
        return RTN_ERROR;

    return total_written;
}

/**
 * @brief Write a buffer to a file, normalizing the file path.
 *
 * This function writes the entire buffer to a file at the given path (after normalization).
 * It handles partial writes, checks for errors, and ensures resources are freed.
 *
 * @param file_path  Path to the file to write (will be normalized).
 * @param buf        Pointer to the buffer to write.
 * @param buf_size   Number of bytes to write from the buffer.
 *
 * @return On success, returns the total number of written bytes.
 *         On error, returns -1.
 *
 * @note The function normalizes the file path to ensure it is valid.
 */
ssize_t write_data_to_file(const char* file_path, const void* buf, size_t buf_size)
{
    if (file_path == NULL || buf == NULL || buf_size == 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) write_data_to_file | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    char* normalized_path = normalize_file_path(file_path);
    if (normalized_path == NULL)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) write_data_to_file | " ERROR_INVALID_ARGUMENTS "\n");
        return RTN_ERROR;
    }

    size_t total_written = 0;
    FILE* file = fopen(normalized_path, "wb");
    if (!file)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) write_data_to_file | " ERROR_FAILED_TO_OPEN_FILE "\n");
        goto end;
    }

    const uint8_t* ptr = (const uint8_t*)buf;
    size_t remaining = buf_size;
    while (remaining > 0)
    {
        size_t written = fwrite(ptr, 1, remaining, file);
        if (written == 0)
        {
            if (ferror(file))
            {
                write_msg_to_fd(STDERR_FILENO,
                                "(f) write_data_to_file | " ERROR_FAILED_TO_WRITE_FILE "\n");
                goto end;
            }

            break;
        }

        total_written += written;
        ptr += written;
        remaining -= written;
    }

end:
    if (file)
        fclose(file);
    if (normalized_path)
        free(normalized_path);

    if (total_written != buf_size)
        return RTN_ERROR;

    return (ssize_t)total_written;
}
