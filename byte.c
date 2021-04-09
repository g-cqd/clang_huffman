#include "headers.h"

byte_t* add(byte_t byte, byte_t* array, size_t length)
{
    array[length]     = byte;
    array[length + 1] = (char)(-1);
    return array;
}

byte_t* cpy(byte_t* dest, byte_t* src, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }
    dest[size] = -1;
    return dest;
}

byte_t* bcalloc(size_t size)
{
    byte_t* pointer;
    if((pointer = (byte_t*)calloc(size + 1, sizeof(byte_t))) == NULL)
    {
        fprintf(stderr, "\n" H_R "Something went wrong with calloc. -- balloc(%zu)" H_0, size);
        exit(EXIT_FAILURE);
    }
    pointer[size] = -1;
    return pointer;
}

byte_t* bmalloc(size_t size)
{
    byte_t* pointer;
    if((pointer = (byte_t*)malloc(size + 1 * sizeof(byte_t))) == NULL)
    {
        fprintf(stderr, "\n" H_R "Something went wrong with calloc. -- balloc(%zu)" H_0, size);
        exit(EXIT_FAILURE);
    }
    for(size_t i = 0; i < size; i++)
    {
        pointer[i] = 0;
    }
    pointer[size] = -1;
    return pointer;
}
