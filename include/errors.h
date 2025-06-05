/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | errors.h
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-05

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef ERRORS_H
#define ERRORS_H

#define ERROR_FAILED_TO_ALLOCATE_MEMORY "Error: Failed to allocate memory.\n"

#define ERROR_INVALID_RTSP_URL "Error: Invalid RTSP URL provided.\n"
#define ERROR_INVALID_ARGUMENTS "Error: Invalid arguments provided.\n"
#define ERROR_FAILED_TO_PARSE_CONFIG "Error: Failed to parse configuration file. Using default values.\n"
#define ERROR_FAILED_TO_WRITE "Error: Failed to write to file descriptor.\n"
#define ERROR_NO_DATA_TO_WRITE "Error: No data to write to file descriptor.\n"
#define ERROR_INVALID_OUTPUT_FORMAT "Error: Invalid output format specified.\n"
#define ERROR_INVALID_OUTPUT_FILE_PATH "Error: Invalid output file path provided.\n"

#define RTN_ERROR -1
#define RTN_SUCCESS 0
#define RTN_COMPLETE 1

#define MAIN_ERROR_CODE 1
#define MAIN_SUCCESS_CODE 0

#endif  // ERRORS_H