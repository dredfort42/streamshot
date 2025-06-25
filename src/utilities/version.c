/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | printer.c
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-07

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdio.h>

/**
 * @brief Prints version and author information for the application.
 */
void print_version(void)
{
    printf("RTSP Stream Image Capture Tool\n");
    printf("Version: %s\n", "1.0.1");
    printf("Author: Dmitry Novikov <dredfort.42@gmail.com>\n");
    printf("GitHub: https://github.com/dredfort42\n");
    printf("For usage details, run with --help.\n");
}
