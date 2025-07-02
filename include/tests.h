/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | tests.h
    ::  ::          ::  ::    Created  | 2025-06-25
          ::::  ::::          Modified | 2025-06-29

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
int test_image_format_to_string(void);
int test_string_to_image_format(void);
int test_convert_image(void);
int test_jpg_image(void);
int test_png_image(void);
int test_ppm_image(void);
int test_parse_args(void);
int test_validate_options(void);

#endif  // TESTS_H
