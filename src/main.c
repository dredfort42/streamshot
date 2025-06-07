/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | main.c
    ::  ::          ::  ::    Created  | 2025-06-04
          ::::  ::::          Modified | 2025-06-05

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "options.h"
#include "reader.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        print_help(argv[0]);
        return MAIN_ERROR_CODE;
    }

    short error_code = 0;
    options_t* options = get_options(argc, argv);
    if (!options)
        goto end;

    if (options->help || options->version)
    {
        error_code = MAIN_SUCCESS_CODE;
        goto end;
    }

    // error_code = parse_args(argc, argv, &options);
    // if (error_code)
    //     goto end;

    // get_streamshot(&options);

end:
    if (options)
        free_options(options);

    return error_code < 0 ? MAIN_ERROR_CODE : MAIN_SUCCESS_CODE;
}