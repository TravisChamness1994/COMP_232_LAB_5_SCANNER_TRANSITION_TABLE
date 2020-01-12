#ifndef __SCANNER_H
#define __SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


typedef struct token
{
    int type;
    char *strval;
} TOKEN;

typedef struct
{
    int n_token_types;
    int n_states;
    int n_character_classes;
    int accept_flag;
    int skip_flag;
    int error_flag;
    char **tokens;
    char **states;
    char **character_classes;
    int **table;
} TRANS_TABLE;

TRANS_TABLE *readTable();
int findClassIndex(TRANS_TABLE *table, char c);
TOKEN *scanner(TRANS_TABLE *table);
void printToken(TOKEN *, TRANS_TABLE *table);
void freeToken(TOKEN **);
void freeTable(TRANS_TABLE **);

#define BUF_SIZE 128
#define MAX_LINE_LENGTH 256

#endif