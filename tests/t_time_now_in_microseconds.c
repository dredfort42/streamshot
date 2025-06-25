/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | t_time_now_in_microseconds.c
    ::  ::          ::  ::    Created  | 2025-06-25
          ::::  ::::          Modified | 2025-06-25

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdio.h>
#include <unistd.h>

#include "utilities.h"

int test_time_now_in_microseconds(void)
{
    int failed = 0;

    // Test 1: time_now_in_microseconds should return a positive value
    long long t1 = time_now_in_microseconds();
    if (t1 <= 0)
    {
        printf("" ANSI_RED "KO" ANSI_RESET
               "] (f) time_now_in_microseconds: returned non-positive value (%lld)\n",
               t1);
        failed++;
    }
    else
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) time_now_in_microseconds: returned positive value (%lld)\n",
               t1);

    // Test 2: time should increase after a short sleep
    usleep(1000);  // sleep for 1 millisecond
    long long t2 = time_now_in_microseconds();
    if (t2 <= t1)
    {
        printf("" ANSI_RED "KO" ANSI_RESET
               "] (f) time_now_in_microseconds: time did not increase after sleep (%lld <= %lld)\n",
               t2, t1);
        failed++;
    }
    else
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) time_now_in_microseconds: time increased after sleep (%lld > %lld)\n",
               t2, t1);

    // Test 3: time difference should be at least 1000 microseconds
    if (t2 - t1 < 1000)
    {
        printf("" ANSI_RED "KO" ANSI_RESET
               "] (f) time_now_in_microseconds: time difference less than 1000us (%lld)\n",
               t2 - t1);
        failed++;
    }
    else
        printf("[" ANSI_GREEN "OK" ANSI_RESET
               "] (f) time_now_in_microseconds: time difference is at least 1000us (%lld)\n",
               t2 - t1);

    return failed;
}