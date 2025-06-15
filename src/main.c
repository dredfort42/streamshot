/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | main.c
    ::  ::          ::  ::    Created  | 2025-06-04
          ::::  ::::          Modified | 2025-06-09

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "options.h"
#include "stream.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) main | " ERROR_INVALID_ARGUMENTS);
        print_help(argv[0]);
        return MAIN_ERROR_CODE;
    }

    short error_code = 0;
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

    get_streamshot(options);

end:
    if (options)
        free_options(options);

    return error_code;
}