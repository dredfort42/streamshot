/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | normalizer_test.c
    ::  ::          ::  ::    Created  | 2025-06-09
          ::::  ::::          Modified | 2025-06-24

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

int test_normalize_file_path_valid(void)
{
    char* input = "/path/to/file.txt";
    const char* expected = "/path/to/file.txt";
    char* result = normalize_file_path(input);
    if (!result || strcmp(result, expected) != 0)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) normalize_file_path: valid path failed | input=\"%s\" | expected=\"%s\" | "
               "got=\"%s\"\n",
               input, expected, result ? result : "NULL");
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) normalize_file_path: valid path passed\n");
    free(result);
    return 0;
}

int test_normalize_file_path_invalid_chars(void)
{
    char* input = "/path/to/fi!le@#.txt";
    const char* expected = "/path/to/fi_le__.txt";
    char* result = normalize_file_path(input);
    if (!result || strcmp(result, expected) != 0)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) normalize_file_path: invalid chars failed | input=\"%s\" | expected=\"%s\" | "
               "got=\"%s\"\n",
               input, expected, result ? result : "NULL");
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) normalize_file_path: invalid chars passed\n");
    free(result);
    return 0;
}

int test_normalize_file_path_empty(void)
{
    char* input = "";
    char* result = normalize_file_path(input);
    if (result)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) normalize_file_path: empty string failed | expected=NULL | got=\"%s\"\n",
               result);
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) normalize_file_path: empty string passed\n");
    return 0;
}

int test_normalize_file_path_null(void)
{
    char* result = normalize_file_path(NULL);
    if (result)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) normalize_file_path: NULL input failed | expected=NULL | got=\"%s\"\n",
               result);
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) normalize_file_path: NULL input passed\n");
    return 0;
}

int test_normalize_file_path(void)
{
    int failed = 0;
    failed += test_normalize_file_path_valid();
    failed += test_normalize_file_path_invalid_chars();
    failed += test_normalize_file_path_empty();
    failed += test_normalize_file_path_null();
    return failed;
}