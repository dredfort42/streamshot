#include "utilities.h"

int normalize_file_path_test();
int trim_flag_value_test();

int main()
{
    int failed = 0;
    failed += normalize_file_path_test();
    failed += trim_flag_value_test();

    printf("\n");
    if (failed)
        printf(ANSI_RED "%d tests failed.\n" ANSI_RESET, failed);
    else
        printf(ANSI_GREEN "All tests passed successfully.\n" ANSI_RESET);

    return 0;
}