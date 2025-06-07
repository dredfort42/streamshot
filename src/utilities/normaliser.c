/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | normaliser.c
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-07

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "utilities.h"

/**
 * @brief Normalize a file path by replacing invalid characters with underscores.
 *
 * This function allocates and returns a new string where all characters in the input
 * path that are not alphanumeric, '_', '-', '.', or '/' are replaced with '_'.
 *
 * @param file_path  Input file path string.
 *
 * @return Newly allocated normalized path, or NULL on error.
 *
 * @note The caller is responsible for freeing the returned string.
 */
char* normalize_file_path(const char* file_path)
{
    if (file_path == NULL || strlen(file_path) < 3)
        return NULL;

    char* normalized_path = strdup(file_path);
    if (normalized_path == NULL)
        return NULL;

    size_t len = strlen(file_path);
    for (size_t i = 0; i < len; i++)
    {
        char c = file_path[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.' || c == '/'))
            normalized_path[i] = '_';
    }

    return normalized_path;
}
