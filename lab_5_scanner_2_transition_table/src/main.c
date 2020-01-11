#include "scanner.h"
#include <unistd.h>

int main(int argc, char** argv)
{
    TOKEN *token = NULL;


    freopen(argv[1], "r", stdin); // switch stdin to get the table from the config file

    TRANS_TABLE *table = readTable();

    freopen(argv[2], "r", stdin); // switch stdin to read from the input file

    while ((token = scanner(table)) != NULL)
    {
        printToken(token, table);
        freeToken(&token);
        fflush(stdout);
    }
    printf("\n");
}