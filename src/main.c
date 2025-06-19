/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | main.c
    ::  ::          ::  ::    Created  | 2025-06-04
          ::::  ::::          Modified | 2025-06-19

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <unistd.h>

#include "errors.h"
#include "options.h"
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

    raw_image_t* raw_image = get_raw_image(options);
    if (!raw_image)
    {
        error_code = MAIN_ERROR_CODE;
        goto end;
    }

    // TMP
    char header[32];
    snprintf(header, sizeof(header), "P6\n%d %d\n255\n", raw_image->width, raw_image->height);

    // if (options->debug)
    // {
    // Save to PPM file (for debugging)
    char file_name[1024];
    snprintf(file_name, sizeof(file_name), "%s/average_frame.ppm", options->debug_dir);
    FILE* f = fopen(file_name, "wb");
    if (f)
    {
        fwrite(header, 1, strlen(header), f);
        fwrite(raw_image->data, 1, raw_image->size, f);
        fclose(f);
        if (options->debug)
            printf("Saved %s\n", file_name);
    }
    // }

    // // Write average video_frame to output fd
    // write_data_to_fd(data_fd, (const uint8_t*)header, strlen(header));
    // write_data_to_fd(data_fd, avg_buffer, image_size);
    // if (options->debug)
    //     printf("Average video_frame written to data_fd %d\n", data_fd);

end:
    if (options)
        free_options(options);
    return error_code;
}