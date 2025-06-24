/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | tests.c
    ::  ::          ::  ::    Created  | 2025-06-09
          ::::  ::::          Modified | 2025-06-24

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdio.h>

#include "utilities.h"

int test_normalize_file_path();
int test_trim_flag_value();
int test_write_data_to_fd();
int test_write_msg_to_fd();

int main(void)
{
    int failed = 0;
    failed += test_normalize_file_path();
    failed += test_trim_flag_value();
    failed += test_write_data_to_fd();
    failed += test_write_msg_to_fd();

    printf("\n");
    if (failed)
        printf(ANSI_RED "%d tests failed.\n" ANSI_RESET, failed);
    else
        printf(ANSI_GREEN "All tests passed successfully.\n" ANSI_RESET);

    return 0;
}