#include "headers.h"

int isOption(char* array)
{
    return (array[0] == '-' && array[2] == '\0') || (array[0] == '-' && array[1] == '-');
}

long getNumber(char* array)
{
    char* end_pointer;
    long parsed_value = 0;
    errno             = 0;
    parsed_value      = strtol(array, &end_pointer, 10);
    if(errno || *end_pointer != '\0' || array == end_pointer || parsed_value < INT64_MIN || parsed_value > INT64_MAX)
    {
        return 0;
    }
    else
    {
        return parsed_value;
    }
}

int isAny(char* array, char* short_flag, char* long_flag)
{
    return (strcmp(array, short_flag) == 0) || (strcmp(array, long_flag) == 0);
}

void reached(char* info,unsigned line) {
    if ( DEBUG_STATE == 1 ) {
        printf("\nReached (" H_C "%s" H_0 ") ---- " H_G "%u" H_0, info, line);
    }
}

void printByte(byte_t* array, size_t length)
{
    printf("\n (" H_R "%zu" H_0 ") [ ", length);
    for(size_t i = 0; i < length; i++)
    {
        if(i > 0)
        {
            printf(", ");
        }
        printf("%d", array[i]);
    }
    printf(" ]");
    printf("\n\n");
}

void printString(char* array, size_t length)
{
    printf("\n (" H_R "%zu" H_0 ") [ ", length);
    for(size_t i = 0; i < length; i++)
    {
        if(i > 0)
        {
            printf(", ");
        }
        printf("%c", array[i]);
    }
    printf(" ]");
    printf("\n\n");
}

void printLong(unsigned long* array, size_t length)
{
    printf("\n (" H_R "%zu" H_0 ") [ ", length);
    for(unsigned int i = 0; i < length; i++)
    {
        if(i > 0)
        {
            printf(", ");
        }
        printf("%lu", array[i]);
    }
    printf(" ]");
    printf("\n\n");
}

void printNode(Node* node, char* type)
{
    printf("\nNODE " H_C "%s" H_0, type);
    printf("\nch ..... : " H_C "<%c>" H_0, node->ch == '\0' ? '-' : node->ch);
    printf("\nweight . : " H_C "%zu" H_0, node->weight);
    printf("\nleft ... : " H_C "%zu" H_0, node->left != NULL ? node->left->weight : 0);
    printf("\nright .. : " H_C "%zu" H_0, node->right != NULL ? node->right->weight : 0);
    printf("\n");
    printf("\n");
}

void printDecoderNode(DecoderNode* node, char* type)
{
    printf("\nNODE " H_C "%s" H_0, type);
    printf("\nch ..... : " H_C "<%c>" H_0, node->ch == '\0' ? '-' : node->ch);
    printf("\nweight . : " H_C "%.4f" H_0, node->weight);
    printf("\nleft ... : " H_C "%.4f" H_0, node->left != NULL ? node->left->weight : 0);
    printf("\nright .. : " H_C "%.4f" H_0, node->right != NULL ? node->right->weight : 0);
    printf("\nbits ... : ");
    printByte(node->bits, node->length);
    printf("\n");
    printf("\n");
}

void printNodeList(Node* array, size_t length)
{
    printf("\n (" H_R "%zu" H_0 ") [ ", length);
    for(size_t i = 0; i < length; i++)
    {
        if(i > 0)
            printf(", ");
        printf("<" H_Y "%c" H_0 ":" H_C "%zu" H_0 ">", array[i].ch, array[i].weight);
    }
    printf(" ]");
    printf("\n\n");
}

void printDecoderNodeList(DecoderNode* array, size_t length)
{
    printf("\n (" H_R "%zu" H_0 ") [ ", length);
    for(size_t i = 0; i < length; i++)
    {
        if(i > 0)
            printf(", ");
        printf("<" H_Y "%c" H_0 ":" H_C "%.4f" H_0 ">", array[i].ch, array[i].weight);
    }
    printf(" ]");
    printf("\n\n");
}

void printFromDecoderRoot(DecoderNode* node)
{

    if(node->ch == '\0')
    {
        if(node->left->weight > 0)
        {
            printFromDecoderRoot(node->left);
        }
        if(node->right->weight > 0)
        {
            printFromDecoderRoot(node->right);
        }
    }
    else
    {
        printDecoderNode(node,"");
    }
}