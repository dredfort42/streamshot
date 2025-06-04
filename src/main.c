/*********************************************************************

        ##          ##        +--------+-----------------------+
          ##      ##          | Author | Dmitry Novikov        |
        ##############        | Email  | dredfort.42@gmail.com |
      ####  ######  ####      +--------+-----------------------+
    ######################
    ##  ##############  ##    File     | main.c
    ##  ##          ##  ##    Created  | 2025-06-04
          ####  ####          Modified | 2025-06-04

*********************************************************************/

#include <stdio.h>

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        // Print each argument passed to the program
        printf("Argument %d: %s\n", i, argv[i]);
    }
    return 0;
}