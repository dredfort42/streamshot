/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | trim_flag_value_test.c
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

int test_trim_flag_value_basic(void)
{
    char* input = "  \"'example value'\"  ";
    const char* expected = "example value";
    char* result = trim_flag_value(input);
    if (!result || strcmp(result, expected) != 0)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) trim_flag_value: basic trim failed | input=\"%s\" | expected=\"%s\" | "
               "got=\"%s\"\n",
               input, expected, result ? result : "NULL");
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: basic trim passed\n");
    free(result);
    return 0;
}

int test_trim_flag_value_no_trim(void)
{
    char* input = "value";
    const char* expected = "value";
    char* result = trim_flag_value(input);
    if (!result || strcmp(result, expected) != 0)
    {
        printf(
            "[" ANSI_RED "KO" ANSI_RESET
            "] (f) trim_flag_value: no trim failed | input=\"%s\" | expected=\"%s\" | got=\"%s\"\n",
            input, expected, result ? result : "NULL");
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: no trim passed\n");
    free(result);
    return 0;
}

int test_trim_flag_value_only_spaces(void)
{
    char* input = "     ";
    char* result = trim_flag_value(input);
    if (result)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) trim_flag_value: only spaces failed | expected=NULL | got=\"%s\"\n",
               result);
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: only spaces passed\n");
    return 0;
}

int test_trim_flag_value_only_quotes(void)
{
    char* input = "\"\"\"";
    char* result = trim_flag_value(input);
    if (result)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) trim_flag_value: only quotes failed | expected=NULL | got=\"%s\"\n",
               result);
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: only quotes passed\n");
    return 0;
}

int test_trim_flag_value_backticks(void)
{
    char* input = "`value`";
    const char* expected = "value";
    char* result = trim_flag_value(input);
    if (!result || strcmp(result, expected) != 0)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) trim_flag_value: backticks failed | input=\"%s\" | expected=\"%s\" | "
               "got=\"%s\"\n",
               input, expected, result ? result : "NULL");
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: backticks passed\n");
    free(result);
    return 0;
}

int test_trim_flag_value_mixed(void)
{
    char* input = "  '`value with spaces`'  ";
    const char* expected = "value with spaces";
    char* result = trim_flag_value(input);
    if (!result || strcmp(result, expected) != 0)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) trim_flag_value: mixed whitespace/quotes failed | input=\"%s\" | "
               "expected=\"%s\" | got=\"%s\"\n",
               input, expected, result ? result : "NULL");
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET
           "] (f) trim_flag_value: mixed whitespace/quotes passed\n");
    free(result);
    return 0;
}

int test_trim_flag_value_empty(void)
{
    char* input = "";
    char* result = trim_flag_value(input);
    if (result)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) trim_flag_value: empty string failed | expected=NULL | got=\"%s\"\n",
               result);
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: empty string passed\n");
    return 0;
}

int test_trim_flag_value_null(void)
{
    char* result = trim_flag_value(NULL);
    if (result)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) trim_flag_value: NULL input failed | expected=NULL | got=\"%s\"\n",
               result);
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: NULL input passed\n");
    return 0;
}

int test_trim_flag_value_one_char(void)
{
    char* input = " ";
    char* result = trim_flag_value(input);
    if (result)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) trim_flag_value: one char failed | expected=NULL | got=\"%s\"\n",
               result);
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: one char passed\n");
    return 0;
}

int test_trim_flag_value_inner_quotes(void)
{
    char* input = "val\"ue";
    const char* expected = "val\"ue";
    char* result = trim_flag_value(input);
    if (!result || strcmp(result, expected) != 0)
    {
        printf("[" ANSI_RED "KO" ANSI_RESET
               "] (f) trim_flag_value: inner quotes failed | input=\"%s\" | expected=\"%s\" | "
               "got=\"%s\"\n",
               input, expected, result ? result : "NULL");
        free(result);
        return 1;
    }
    printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: inner quotes passed\n");
    free(result);
    return 0;
}

int test_trim_flag_value(void)
{
    int failed = 0;
    failed += test_trim_flag_value_basic();
    failed += test_trim_flag_value_no_trim();
    failed += test_trim_flag_value_only_spaces();
    failed += test_trim_flag_value_only_quotes();
    failed += test_trim_flag_value_backticks();
    failed += test_trim_flag_value_mixed();
    failed += test_trim_flag_value_empty();
    failed += test_trim_flag_value_null();
    failed += test_trim_flag_value_one_char();
    failed += test_trim_flag_value_inner_quotes();
    return failed;
}