/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | normalise.c
    ::  ::          ::  ::    Created  | 2025-06-05
          ::::  ::::          Modified | 2025-06-19

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include <stdlib.h>
#include <string.h>

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
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
              c == '_' || c == '-' || c == '.' || c == '/'))
            normalized_path[i] = '_';
    }

    return normalized_path;
}

/**
 * @brief Trims leading and trailing whitespace and quote-like characters from a string.
 *
 * This function creates a duplicate of the input string and removes any leading or trailing
 * spaces, double quotes ('"'), single quotes ('\''), or backticks ('`'). The returned string
 * is dynamically allocated and must be freed by the caller.
 *
 * @param str The input null-terminated string to be trimmed. May be NULL.
 *
 * @return A newly allocated, trimmed string on success, or NULL if the input is NULL,
 *         empty, or memory allocation fails. The caller is responsible for freeing the result.
 *
 * @note This function does not modify the original string.
 *       If the input string contains only characters to be trimmed, an empty string is returned.
 */
char* trim_flag_value(const char* str)
{
    if (!str || *str == '\0')
        return NULL;

    const char* start = str;
    while (*start && (*start == ' ' || *start == '"' || *start == '\'' || *start == '`')) start++;

    if (*start == '\0')
        return NULL;

    const char* end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '"' || *end == '\'' || *end == '`')) end--;

    size_t trimmed_len = end - start + 1;

    char* result = (char*)malloc(trimmed_len + 1);
    if (!result)
        return NULL;

    size_t i = 0;
    while (i < trimmed_len)
    {
        result[i] = start[i];
        i++;
    }

    result[trimmed_len] = '\0';
    return result;
}