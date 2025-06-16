/*******************************************************************

        ::          ::        +--------+-----------------------+
          ::      ::          | Author | Dmitry Novikov        |
        ::::::::::::::        | Email  | dredfort.42@gmail.com |
      ::::  ::::::  ::::      +--------+-----------------------+
    ::::::::::::::::::::::
    ::  ::::::::::::::  ::    File     | trim_flag_value_test.c
    ::  ::          ::  ::    Created  | 2025-06-09
          ::::  ::::          Modified | 2025-06-09

    GitHub:   https://github.com/dredfort42
    LinkedIn: https://linkedin.com/in/novikov-da

*******************************************************************/

#include "utilities.h"

int trim_flag_value_test()
{
#define RESULT(test, expected) strcmp(test, expected) ? ANSI_RED "KO" ANSI_RESET : ANSI_GREEN "OK" ANSI_RESET

    char* input;
    char* expected_output;
    int failed = 0;

    // Test 1: Basic trimming of spaces and quotes
    input = "  \"'example value'\"  ";
    expected_output = "example value";
    char* test1 = trim_flag_value(input);
    if (test1)
    {
        if (strcmp(test1, expected_output))
            failed++;
        printf("[%s] (f) trim_flag_value: input=\"%s\" | expected=\"%s\" | got=\"%s\"\n", RESULT(test1, expected_output), input, expected_output,
               test1 ? test1 : "NULL");
        free(test1);
    }
    else
    {
        failed++;
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" | expected=\"%s\" | got=NULL\n", input, expected_output);
    }

    // Test 2: No trimming needed
    input = "value";
    expected_output = "value";
    char* test2 = trim_flag_value(input);
    if (test2)
    {
        if (strcmp(test2, expected_output))
            failed++;
        printf("[%s] (f) trim_flag_value: input=\"%s\" | expected=\"%s\" | got=\"%s\"\n", RESULT(test2, expected_output), input, expected_output,
               test2 ? test2 : "NULL");
        free(test2);
    }
    else
    {
        failed++;
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" | expected=\"%s\" | got=NULL\n", input, expected_output);
    }

    // Test 3: Only spaces
    input = "     ";
    char* test3 = trim_flag_value(input);
    if (test3)
    {
        failed++;
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" | expected=NULL | got=\"%s\"\n", input, test3);
        free(test3);
    }
    else
    {
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" 
        returned NULL as expected\n", input);
    }

    // Test 4: Only quotes
    input = "\"\"\"";
    char* test4 = trim_flag_value(input);
    if (test4)
    {
        failed++;
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" | expected=NULL | got=\"%s\"\n", input, test4);
        free(test4);
    }
    else
    {
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" 
        returned NULL as expected\n", input);
    }

    // Test 5: Leading and trailing backticks
    input = "`value`";
    expected_output = "value";
    char* test5 = trim_flag_value(input);
    if (test5)
    {
        if (strcmp(test5, expected_output))
            failed++;
        printf("[%s] (f) trim_flag_value: input=\"%s\" | expected=\"%s\" | got=\"%s\"\n", RESULT(test5, expected_output), input, expected_output,
               test5 ? test5 : "NULL");
        free(test5);
    }
    else
    {
        failed++;
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" | expected=\"%s\" | got=NULL\n", input, expected_output);
    }

    // Test 6: Mixed whitespace and quotes
    input = "  '`value with spaces`'  ";
    expected_output = "value with spaces";
    char* test6 = trim_flag_value(input);
    if (test6)
    {
        if (strcmp(test6, expected_output))
            failed++;
        printf("[%s] (f) trim_flag_value: input=\"%s\" | expected=\"%s\" | got=\"%s\"\n", RESULT(test6, expected_output), input, expected_output,
               test6 ? test6 : "NULL");
        free(test6);
    }
    else
    {
        failed++;
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" | expected=\"%s\" | got=NULL\n", input, expected_output);
    }

    // Test 7: Empty string
    input = "";
    char* test7 = trim_flag_value(input);
    if (test7)
    {
        failed++;
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" | expected=NULL | got=\"%s\"\n", input, test7);
        free(test7);
    }
    else
    {
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" 
        returned NULL as expected\n", input);
    }

    // Test 8: NULL input
    input = NULL;
    char* test8 = trim_flag_value(input);
    if (test8)
    {
        failed++;
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) trim_flag_value: input=NULL | expected=NULL | got=\"%s\"\n", test8);
        free(test8);
    }
    else
    {
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: input=NULL 
        returned NULL as expected\n");
    }

    // Test 9: Only one character to trim
    input = " ";
    char* test9 = trim_flag_value(input);
    if (test9)
    {
        failed++;
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" | expected=NULL | got=\"%s\"\n", input, test9);
        free(test9);
    }
    else
    {
        printf("[" ANSI_GREEN "OK" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" 
        returned NULL as expected\n", input);
    }

    // Test 10: No trailing/leading, but quotes inside
    input = "val\"ue";
    expected_output = "val\"ue";
    char* test10 = trim_flag_value(input);
    if (test10)
    {
        if (strcmp(test10, expected_output))
            failed++;
        printf("[%s] (f) trim_flag_value: input=\"%s\" | expected=\"%s\" | got=\"%s\"\n", RESULT(test10, expected_output), input, expected_output,
               test10 ? test10 : "NULL");
        free(test10);
    }
    else
    {
        failed++;
        printf("[" ANSI_RED "KO" ANSI_RESET "] (f) trim_flag_value: input=\"%s\" | expected=\"%s\" | got=NULL\n", input, expected_output);
    }

    return failed;
}