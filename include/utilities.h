/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | utilities.h
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-07

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef WRITER_H
#define WRITER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "errors.h"

ssize_t write_data_to_file(const char* file_path, const void* buf, size_t buf_size);
ssize_t write_data_to_fd(int fd, const void* buf, size_t buf_size);
ssize_t write_msg_to_fd(int fd, const char* msg);
char* normalize_file_path(const char* file_path);
void print_version();

#endif  // WRITER_H