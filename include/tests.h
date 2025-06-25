/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | tests.h
    ::  ::          ::  ::    Created  | 2025-06-25
          ::::  ::::          Modified | 2025-06-25

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef TESTS_H
#define TESTS_H

int test_normalize_file_path(void);
int test_trim_flag_value(void);
int test_write_data_to_fd(void);
int test_write_msg_to_fd(void);
int test_write_data_to_file(void);
int test_time_now_in_microseconds(void);
int t_image_format_to_string(void);
int t_string_to_image_format(void);

#endif  // TESTS_H
