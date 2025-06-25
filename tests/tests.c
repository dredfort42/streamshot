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

#include "tests.h"

#include <stdio.h>

#include "utilities.h"

int main()
{
    int failed = 0;
    failed += test_normalize_file_path();
    failed += test_trim_flag_value();
    failed += test_write_data_to_fd();
    failed += test_write_msg_to_fd();
    failed += test_write_data_to_file();
    failed += test_time_now_in_microseconds();
    failed += t_image_format_to_string();
    failed += t_string_to_image_format();

    printf("\n");
    if (failed)
        printf(ANSI_RED "%d tests failed.\n" ANSI_RESET, failed);
    else
        printf(ANSI_GREEN "All tests passed successfully.\n" ANSI_RESET);

    return 0;
}