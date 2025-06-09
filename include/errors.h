/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | errors.h
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-08

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#ifndef ERRORS_H
#define ERRORS_H

#include <errno.h>

#define ERROR_FAILED_TO_ALLOCATE_MEMORY "Error: Failed to allocate memory.\n"
#define ERROR_FAILED_TO_CREATE_DEBUG_DIR "Error: Failed to create debug directory.\n"
#define ERROR_FAILED_TO_OPEN_FILE "Error: Failed to open file.\n"
#define ERROR_FAILED_TO_WRITE_FILE "Error: Failed to write to file.\n"
#define ERROR_FAILED_TO_WRITE_FD "Error: Failed to write to file descriptor.\n"
#define ERROR_NO_DATA_TO_WRITE "Error: No data to write to file descriptor.\n"
#define ERROR_NOT_NULL_TERMINATED "Error: The provided message is not null-terminated.\n"

#define ERROR_INVALID_ARGUMENTS "Error: Invalid arguments provided.\n"
#define ERROR_INVALID_RTSP_URL "Error: Invalid RTSP URL provided.\n"
#define ERROR_INVALID_OUTPUT_FD "Error: Invalid output file descriptor specified.\n"
#define ERROR_INVALID_TIMEOUT "Error: Invalid timeout value.\n"
#define ERROR_INVALID_EXPOSURE "Error: Invalid exposure value.\n"
#define ERROR_INVALID_OUTPUT_FORMAT "Error: Invalid output format specified.\n"
#define ERROR_INVALID_SCALE_FACTOR "Error: Invalid scale factor specified.\n"
#define ERROR_INVALID_RESIZE_HEIGHT "Error: Invalid resize height specified.\n"
#define ERROR_INVALID_RESIZE_WIDTH "Error: Invalid resize width specified.\n"
#define ERROR_INVALID_IMAGE_QUALITY "Error: Invalid image quality specified.\n"
#define ERROR_INVALID_DEBUG_STEP "Error: Invalid debug step specified.\n"
#define ERROR_INVALID_DEBUG_DIR "Error: Invalid debug directory specified.\n"

#define ERROR_FAILED_TO_SET_STREAM_OPTIONS "Error: Failed to set stream options.\n"

#define RTN_ERROR -1
#define RTN_SUCCESS 0

#define MAIN_ERROR_CODE 1
#define MAIN_SUCCESS_CODE 0

#endif  // ERRORS_H