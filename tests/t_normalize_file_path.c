/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | normalizer_test.c
    ::  ::          ::  ::    Created  | 2025-06-09
          ::::  ::::          Modified | 2025-06-09

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "utilities.h"

int normalize_file_path_test()
{
#define RESULT(test, expected) \
    strcmp(test, expected) ? ANSI_RED "KO" ANSI_RESET : ANSI_GREEN "OK" ANSI_RESET

    char* input;
    char* expected_output;
    int failed = 0;

    // Test case 1: Normalizing a valid file path
    input = "/path/to/file.txt";
    expected_output = "/path/to/file.txt";
    char* test1 = normalize_file_path(input);
    if (test1)
    {
        if (strcmp(test1, expected_output))
            failed++;
        printf("[%s] (f) normalize_file_path: input=\"%s\" | expected=\"%s\" | got=\"%s\"\n",
               RESULT(test1, expected_output), input, expected_output, test1);
        free(test1);
    }
    else
    {
        failed++;
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) normalize_file_path: input=\"%s\" | expected=\"%s\" | got=NULL\n",
               input, expected_output);
    }

    // Test case 2: Normalizing a path with invalid characters
    input = "/path/to/fi!le@#.txt";
    expected_output = "/path/to/fi_le__.txt";
    char* test2 = normalize_file_path(input);
    if (test2)
    {
        if (strcmp(test2, expected_output))
            failed++;
        printf("[%s] (f) normalize_file_path: input=\"%s\" | expected=\"%s\" | got=\"%s\"\n",
               RESULT(test2, expected_output), input, expected_output, test2);
        free(test2);
    }
    else
    {
        failed++;
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) normalize_file_path: input=\"%s\" | expected=\"%s\" | got=NULL\n",
               input, expected_output);
    }

    // Test case 3: Normalizing an empty string
    char* test3 = normalize_file_path("");
    if (!test3)
        printf(
            "[" ANSI_GREEN "OK" ANSI_RESET
            "] (f) normalize_file_path: input empty string         returned NULL as expected.\n");
    else
    {
        failed++;
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) normalize_file_path: input empty string did not         return NULL, got %s\n",
            test3);
        free(test3);
    }

    return failed;
}