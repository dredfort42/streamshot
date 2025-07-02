/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | time.c
    ::  ::          ::  ::    Created  | 2025-06-15
          ::::  ::::          Modified | 2025-06-20

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <sys/time.h>
#include <unistd.h>

#include "errors.h"
#include "utilities.h"

/**
 * @brief Get the current time in microseconds since the Unix Epoch.
 *
 * Uses gettimeofday(void) to obtain the current time and        returns the total number
 * of microseconds elapsed since January 1, 1970 (UTC).
 *
 * @return The current time in microseconds as a long long integer on success,
 *         or -1 on failure.
 *
 * @note   This function is POSIX-specific and will not work on Windows without changes.
 */
long long time_now_in_microseconds(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL))
    {
        write_msg_to_fd(STDERR_FILENO,
                        "(f) time_now_in_microseconds | " ERROR_FAILED_TO_GET_TIME "\n");
        return RTN_ERROR;
    }

    return (long long)(tv.tv_sec) * 1000000 + (long long)(tv.tv_usec);
}
