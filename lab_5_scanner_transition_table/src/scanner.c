#include "scanner.h"

#define BUFFER_SIZE 256


void freeToken(TOKEN **token)
{
    if (*token == NULL)
        return;

    free((*token)->strval);

    free(*token);

    *token = NULL;
}

void freeTable(TRANS_TABLE **table)
{
    for (int index = 0; index < (*table)->n_token_types; index++)
    {
        free((*table)->tokens[index]);
    }
    free((*table)->tokens);

    for (int index = 0; index < (*table)->n_states; index++)
    {
        free((*table)->states[index]);
        free((*table)->table[index]);
    }
    free((*table)->states);
    free((*table)->table);

    for (int index = 0; index < (*table)->n_character_classes; index++)
    {
        free((*table)->character_classes[index]);
    }
    free((*table)->character_classes);

    free((*table));
    *table = NULL;
}


void printToken(TOKEN *token, TRANS_TABLE *table)
{
    printf("<%s %s>\n", table->tokens[token->type], token->strval);
}

TRANS_TABLE *readTable()
{
    TRANS_TABLE *table = malloc(sizeof(TRANS_TABLE));

    char buffer[BUFFER_SIZE];

    // read metadata on first line (n_token_types, n_states, n_character_classes)
    char delim;
    scanf("%c\n%d %d %d\n",
            &delim,
            &table->n_token_types,
            &table->n_states,
            &table->n_character_classes
    );
    table->accept_flag = table->n_states;
    table->skip_flag = table->n_states + 1;
    table->error_flag = table->n_states + 2;

    // allocate upper level for table
    table->tokens = calloc(sizeof(char*), (size_t)table->n_token_types);
    table->states = calloc(sizeof(char*), (size_t)table->n_states);
    table->character_classes = calloc(sizeof(char*), (size_t)table->n_character_classes);
    table->table = calloc(sizeof(int*), (size_t)table->n_states);

    // read token names
    char currentChar;
    for(int tokenIndex = 0; tokenIndex < table->n_token_types; tokenIndex++)
    {
        int bufferIndex = 0;
        currentChar = (char)getchar();
        while(currentChar != delim && currentChar != '\n')
        {
            buffer[bufferIndex++] = currentChar;
            currentChar = (char)getchar();
        }
        buffer[bufferIndex++] = '\0';
        table->tokens[tokenIndex] = calloc(sizeof(char), (size_t)bufferIndex);
        strcpy(table->tokens[tokenIndex], buffer);
    }

    // read state names
    for (int stateIndex = 0; stateIndex < table->n_states; stateIndex++)
    {
        int bufferIndex = 0;
        currentChar = (char)getchar();
        while (currentChar != delim && currentChar != '\n')
        {
            buffer[bufferIndex++] = currentChar;
            currentChar = (char)getchar();
        }
        buffer[bufferIndex++] = '\0';
        table->states[stateIndex] = calloc(sizeof(char), (size_t)bufferIndex);
        strcpy(table->states[stateIndex], buffer);
    }

    // read character classes
    for (int classIndex = 0; classIndex < table->n_character_classes; classIndex++)
    {
        int bufferIndex = 0;
        currentChar = (char)getchar();
        while (currentChar != delim && currentChar != '\n')
        {
            if (currentChar == '\\')
            {
                currentChar = (char)getchar();
                switch (currentChar)
                {
                    case 'n':
                        buffer[bufferIndex++] = '\n';
                        break;
                    case 't':
                        buffer[bufferIndex++] = '\t';
                        break;
                    case 'r':
                        buffer[bufferIndex++] = '\r';
                        break;
                    default:
                        printf("\nUnsupported escape sequence '\\%c', exiting.\n", currentChar);
                        exit(1);
                }
            }
            else
            {
                buffer[bufferIndex++] = currentChar;
            }
            currentChar = (char)getchar();
        }
        buffer[bufferIndex++] = '\0';
        table->character_classes[classIndex] = calloc(sizeof(char), (size_t)bufferIndex);
        strcpy(table->character_classes[classIndex], buffer);
    }

    // translate table to ints
    for (int stateIndex = 0; stateIndex < table->n_states; stateIndex++)
    {
        table->table[stateIndex] = calloc(sizeof(int), (size_t)table->n_character_classes+2);
        for (int classIndex = 0; classIndex < table->n_character_classes+1; classIndex++)
        {
            int bufferIndex = 0;
            currentChar = (char) getchar();
            while (currentChar != delim && currentChar != '\n')
            {
                buffer[bufferIndex++] = currentChar;
                currentChar = (char) getchar();
            }
            buffer[bufferIndex++] = '\0';

            if (strcmp(buffer, "a") == 0)
            {
                table->table[stateIndex][classIndex] = table->accept_flag;
            }
            else if (strcmp(buffer, "s") == 0)
            {
                table->table[stateIndex][classIndex] = table->skip_flag;
            }
            else if (strcmp(buffer, "e") == 0)
            {
                table->table[stateIndex][classIndex] = table->error_flag;
            }
            else
            {
                bool found = false;
                for (int outState = 0; outState < table->n_states; outState++)
                {
                    if (strcmp(buffer, table->states[outState]) == 0)
                    {
                        table->table[stateIndex][classIndex] = outState;
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    printf("\nUndefined state transition \"%s\", exiting...", buffer);
                    exit(1);
                }
            }
        }
        int bufferIndex = 0;
        currentChar = (char) getchar();
        while (currentChar != delim && currentChar != '\n')
        {
            buffer[bufferIndex++] = currentChar;
            currentChar = (char) getchar();
        }
        buffer[bufferIndex++] = '\0';
        bool found = false;
        for (int outToken = 0; outToken < table->n_token_types; outToken++)
        {
            if (strcmp(buffer, table->tokens[outToken]) == 0)
            {
                table->table[stateIndex][table->n_character_classes+1] = outToken;
                found = true;
                break;
            }
        }
        if (!found)
        {
            printf("\nUndefined output token \"%s\", exiting...", buffer);
            exit(1);
        }
    }
    return table;
}

//
// find the index of the character class in transitionTable containing char c
//

int findClassIndex(TRANS_TABLE *table, char c)
{
    for (int index = 0; index < table->n_character_classes; index++)
    {
        if (strchr(table->character_classes[index], c) != NULL)
        {
            return index;
        }
    }

    return table->n_character_classes; // not found means not in a class means invalid
}

//
// reads characters from stdin to create the next token
// uses the transition table to drive the state machine
//

TOKEN *scanner(TRANS_TABLE *table)
{
    TOKEN *token = NULL;

    char buffer[BUFFER_SIZE];
    int bufferIndex = 0;

    char currentChar;
    int state = 0;
    int classIndex = 0;

    while (token == NULL)
    {
        currentChar = (char)getchar();
        if (currentChar == EOF)
        {
            return NULL;
        }
        classIndex = findClassIndex(table, currentChar);

        int tempState = table->table[state][classIndex];

        if (tempState == table->accept_flag)
        {
            ungetc(currentChar, stdin);
            buffer[bufferIndex++] = '\0';
            token = malloc(sizeof(TOKEN));
            token->type = table->table[state][table->n_character_classes+1];
            token->strval = calloc(sizeof(char), (size_t)bufferIndex);
            strcpy(token->strval, buffer);
        }
        else if (tempState == table->error_flag)
        {
            buffer[bufferIndex++] = currentChar;
            buffer[bufferIndex++] = '\0';
            token = malloc(sizeof(token));
            token->type = 0;
            token->strval = calloc(sizeof(char), (size_t)bufferIndex);
            strcpy(token->strval, buffer);
            return token;
        }
        else if (tempState == table->skip_flag)
        {

        }
        else
        {
            buffer[bufferIndex++] = currentChar;
            state = tempState;
        }

    }

    return token;
}



