/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | main.c
    ::  ::          ::  ::    Created  | 2025-06-04
          ::::  ::::          Modified | 2025-06-20

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "process.h"
#include "stream.h"
#include "utilities.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) main | " ERROR_INVALID_ARGUMENTS);
        print_help(argv[0]);
        return MAIN_ERROR_CODE;
    }

    short error_code = 0;
    image_t* raw_image = NULL;
    image_t* image = NULL;
    options_t* options = get_options(argc, argv);
    if (!options)
    {
        error_code = MAIN_ERROR_CODE;
        goto end;
    }

    if (options->help)
    {
        print_help(argv[0]);
        error_code = MAIN_SUCCESS_CODE;
        goto end;
    }
    else if (options->version)
    {
        print_version();
        error_code = MAIN_SUCCESS_CODE;
        goto end;
    }
    else if (!options->debug && !options->output_file_path && options->output_file_fd < 0)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) main | " ERROR_NO_OUTPUT_SPECIFIED "\n");
        print_help(argv[0]);
        error_code = MAIN_ERROR_CODE;
        goto end;
    }

    raw_image = get_raw_image(options);
    if (!raw_image)
    {
        error_code = MAIN_ERROR_CODE;
        goto end;
    }

    if (options->output_file_fd < 0 && !options->output_file_path)
        goto end;

    image = get_converted_image(options, raw_image);
    if (!image)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) main | " ERROR_FAILED_TO_CONVERT_IMAGE "\n");
        error_code = MAIN_ERROR_CODE;
        goto end;
    }

    if (options->output_file_path)
    {
        if (write_data_to_file(options->output_file_path, image->data, image->size) < 0)
            error_code = MAIN_ERROR_CODE;
        else if (options->debug)
            printf(ANSI_BLUE "Debug:" ANSI_RESET " Saved converted image to: %s\n",
                   options->output_file_path);
    }

    if (options->output_file_fd != -1)
    {
        if (write_data_to_fd(options->output_file_fd, image->data, image->size) < 0)
            error_code = MAIN_ERROR_CODE;
        else if (options->debug)
            printf(ANSI_BLUE "Debug:" ANSI_RESET " Saved converted image to file descriptor: %d\n",
                   options->output_file_fd);
    }

end:
    if (options)
        free_options(options);
    if (raw_image)
        free_image(raw_image);
    if (image)
        free_image(image);

    return error_code;
}
