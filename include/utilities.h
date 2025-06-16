/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | utilities.h
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-16

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>

#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"
#define ANSI_RESET "\033[0m"

ssize_t write_data_to_file(const char* file_path, const void* buf, size_t buf_size);
ssize_t write_data_to_fd(int fd, const void* buf, size_t buf_size);
ssize_t write_msg_to_fd(int fd, const char* msg);
char* normalize_file_path(const char* file_path);
char* trim_flag_value(const char* str);
void print_version();

long long time_now_in_microseconds();

#endif  // UTILITIES_H