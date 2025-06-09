/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | options.c
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-09

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "options.h"

/**
 * @brief Initializes and allocates an options_t structure with default values.
 *
 * This function allocates memory for an options_t structure, sets all fields to zero,
 * and then assigns default values to each configurable option. If memory allocation fails
 * at any point, NULL is returned.
 *
 * @return options_t* Pointer to the newly allocated and initialized options_t structure,
 *         or NULL if memory allocation fails.
 *
 * @note The returned structure must be freed by the caller using the appropriate
 *       deallocation function to avoid memory leaks.
 */
options_t* init_options()
{
    options_t* options = (options_t*)malloc(sizeof(options_t));
    if (!options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) init_options | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
        return NULL;
    }

    options->rtsp_url = NULL;
    options->timeout_sec = DEFAULT_TIMEOUT_SEC;
    options->output_file_path = NULL;
    options->output_file_fd = -1;
    options->exposure_sec = DEFAULT_EXPOSURE_SEC;
    options->output_format = DEFAULT_OUTPUT_FORMAT;
    options->scale_factor = DEFAULT_SCALE_FACTOR;
    options->resize_height = 0;
    options->resize_width = 0;
    options->image_quality = DEFAULT_IMAGE_QUALITY;
    options->debug = 0;
    options->debug_step = DEFAULT_DEBUG_STEP;
    options->debug_dir = strdup(DEFAULT_DEBUG_DIR);
    if (!options->debug_dir)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) init_options | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
        free(options);
        return NULL;
    }
    options->help = 0;
    options->version = 0;

    return options;
}

/**
 * @brief Parses command-line arguments and populates the options_t structure.
 *
 * This function processes the command-line arguments provided in argc and argv,
 * populating the options_t structure with the parsed values. It handles both short
 * and long option formats, as well as key=value pairs. If any errors occur during
 * parsing or validation, it returns NULL.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line argument strings.
 *
 * @return options_t* Pointer to the populated options_t structure, or NULL if an error occurs.
 */
options_t* get_options(int argc, char* argv[])
{
    options_t* options = init_options();
    if (!options)
        goto error;

    if (parse_args(argc, argv, options) == RTN_ERROR)
        goto error;

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " ARGS parsed successfully.\n");

    if (validate_options(options) == RTN_ERROR)
        goto error;

    if (options->debug)
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Options validated successfully.\n");

    if (options->debug)
    {
        printf(ANSI_BLUE "Debug:" ANSI_RESET " Printing options:\n");
        print_options(options);
    }

    return options;

error:
    free_options(options);
    return NULL;
}

/**
 * @brief Frees the memory allocated for an options_t structure.
 *
 * This function deallocates all memory associated with the provided options_t structure,
 * including its string fields. It sets all pointers in the structure to NULL after freeing
 * to avoid dangling pointers.
 *
 * @param options Pointer to the options_t structure to free.
 */
void free_options(options_t* options)
{
    if (!options)
        return;

    if (options->rtsp_url)
    {
        free(options->rtsp_url);
        options->rtsp_url = NULL;
    }

    if (options->output_file_path)
    {
        free(options->output_file_path);
        options->output_file_path = NULL;
    }

    if (options->debug_dir)
    {
        free(options->debug_dir);
        options->debug_dir = NULL;
    }

    free(options);
}