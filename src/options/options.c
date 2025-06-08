/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | options.c
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-08

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "options.h"

/**
 * @brief Initializes and allocates an options_t structure with default values.
 *
 * This function allocates memory for an options_t structure, sets all fields to zero,
 * and then assigns default values to each configurable option. If memory allocation fails
 * at any point, an error message is written to STDERR and NULL is returned.
 *
 * @return options_t* Pointer to the newly allocated and initialized options_t structure,
 *         or NULL if memory allocation fails.
 *
 * @note The returned structure must be freed by the caller using the appropriate
 *       deallocation function to avoid memory leaks.
 *
 * Error Handling:
 * - If allocation of the options_t structure or the debug_dir string fails,
 *   an error message is written to STDERR and NULL is returned.
 */
options_t* init_options()
{
    options_t* options = (options_t*)malloc(sizeof(options_t));
    if (!options)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) init_options | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
        return NULL;
    }

    memset(options, 0, sizeof(*options));

    options->timeout_sec = DEFAULT_TIMEOUT_SEC;
    options->exposure_sec = DEFAULT_EXPOSURE_SEC;
    options->output_file_fd = -1;

    options->output_format = DEFAULT_OUTPUT_FORMAT;
    options->scale_factor = DEFAULT_SCALE_FACTOR;
    options->image_quality = DEFAULT_IMAGE_QUALITY;
    options->debug_step = DEFAULT_DEBUG_STEP;
    options->debug_dir = strdup(DEFAULT_DEBUG_DIR);
    if (!options->debug_dir)
    {
        write_msg_to_fd(STDERR_FILENO, "(f) init_options | " ERROR_FAILED_TO_ALLOCATE_MEMORY);
        free(options);
        return NULL;
    }

    return options;
}

options_t* get_options(int argc, char* argv[])
{
    options_t* options = init_options();
    if (!options)
        return NULL;

    if (parse_args(argc, argv, options) == RTN_ERROR)
        return NULL;

    if (options->debug)
        print_options(options);

    return options;
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