#include "scanner.h"

int main(int argc, char** argv)
{
    TOKEN *token = NULL;

    freopen(argv[1], "r", stdin);

    TRANS_TABLE *table = readTable();

    freopen(argv[2], "r", stdin);

    while ((token = scanner(table)) != NULL)
    {
        printToken(token, table);
        freeToken(&token);
        fflush(stdout);
    }
    printf("\n");
    freeTable(&table);
}