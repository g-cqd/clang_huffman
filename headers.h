#include "config.h"

#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * CLI Color formatters
 */
/** Red Formatter */
#define H_R "\x1B[31m"
/** Green Formatter */
#define H_G "\x1B[32m"
/** Yellow Formatter */
#define H_Y "\x1B[33m"
/** Blue Formatter */
#define H_B "\x1B[34m"
/** Magenta Formatter */
#define H_M "\x1B[35m"
/** Cyan Formatter */
#define H_C "\x1B[36m"
/** White Formatter */
#define H_W "\x1B[37m"
/** Reset Formatter */
#define H_0 "\x1B[0m"

/**
 * byte_t type is encoded on a single byte and is unsigned.
 * byte_t type is an alias for unsigned char type.
 */
typedef unsigned char byte_t;

/**
 * d_byte_t type is encoded on 2 bytes and is unsigned.
 * d_byte_t type is an alias for unsigned short type.
 */
typedef unsigned short d_byte_t;

/**
 * Add 0 || 1 to the end of a byte array
 */
byte_t* add(byte_t, byte_t*, size_t);

/**
 * Copy a byte array of length size in an other byte array
 */
byte_t* cpy(byte_t*, byte_t*, size_t);

/**
 * Return a pointer to a size-allocated & reset byte-typed array
 */
byte_t* bcalloc(size_t);

/**
 * Return a pointer to a size-allocated byte-typed array
 */
byte_t* bmalloc(size_t);

/**
 * Encoder Node Type Definition
 */
typedef struct s_encoder_node {
    struct s_encoder_node* left;
    struct s_encoder_node* right;
    size_t weight;
    char ch;
} Node;

/**
 * Decoder Node Type Definition
 */
typedef struct s_decoder_node {
    struct s_decoder_node* left;
    struct s_decoder_node* right;
    byte_t *bits;
    double weight;
    byte_t length;
    char ch;
} DecoderNode;

/**
 * Tree Type Definition
 */
typedef struct s_tree {
    Node* nodes;
    byte_t length;
} Tree;

/**
 * Decoder Tree Type Definition
 */
typedef struct s_decoder_tree {
    DecoderNode* nodes;
    byte_t length;
} DecoderTree;

/**
 * 
 */
typedef struct s_decoder_value {
    byte_t ch;
    byte_t bits_read;
    byte_t int_char_read;
} DecoderValue;

/**
 * Dict Item Type Definition
 */
typedef struct s_dict_item {
    byte_t* bits;
    size_t weight;
    byte_t length;
} DictItem;

/**
 * Dict Type Definition
 */
typedef DictItem* Dict;

/**
 * Reader Return Value Type Definition
 */
typedef struct s_dict_info {
    Dict dict;
    byte_t* order;
    byte_t length;
} DictInfo;

/**
 * Sort Enum Type Definition
 */
typedef enum e_sort
{
    HFFMN_SORT_UNDEFINED,
    HFFMN_HEAPSORT,
    HFFMN_MERGESORT
} SortAlgorithm;

/**
 * Concurrency Enum Type Definition
 */
typedef enum e_task
{
    HFFMN_TASK_UNDEFINED,
    HFFMN_DECODE,
    HFFMN_ENCODE,
    HFFMN_AUTO_DETECT // Not yet
} Task;

/**
 * Concurrency Enum Type Definition
 */
typedef enum e_concurrency
{
    HFFMN_CONCURRENCY_UNDEFINED,
    HFFMN_SEQUENTIAL,
    HFFMN_PARALLEL
} Concurrency;

/**
 * Reader Thread Argument Type Definition
 */
typedef struct s_thread_args {
    char* file_path;
    size_t* array;
    pthread_t* id;
    size_t start;
    size_t length;
} ThreadArguments;

/**
 * Check if a string leads to a directory
 */
int is_dir(char*);

/**
 * Write Encoding Header informations into destination file
 */
void hffmn_write_dict(FILE*, size_t, DictInfo);

/**
 * Void Function for POSIX Thread to read a file with fread
 */
void* hffmn_thread_reader(void*);

/**
 * Count 8-bits character Occurrences in a file
 */
size_t* hffmn_sequential_reader(char*);

/**
 * POSIX Thread Manager to parallelizing file reading
 */
size_t* hffmn_parallel_reader(char*, byte_t);

/**
 * Read Dict Header
 */
size_t hffmn_read_dict(FILE *, DecoderTree *);

/**
 * Run Huffman Decompression
 */
size_t hffmn_decode(char *);

/**
 * Run Huffman Compression
 */
void hffmn_encode(char*, Concurrency, SortAlgorithm, byte_t);

/**
 * Write an compressed / encoded file
 */
size_t hffmn_write(char*, DictInfo);

/**
 * Print a checkpoint
 */
void reached(char*,unsigned);

/**
 * Print a byte array
 */
void printByte(byte_t*, size_t);

/**
 * Print Node Informations
 */
void printNode(Node*, char*);

/**
 * Print Node Informations
 */
void printDecoderNode(DecoderNode*, char*);

/**
 * Print Node List with count for each char
 */
void printNodeList(Node*, size_t);

/**
 * Print Node List with count for each char
 */
void printDecoderNodeList(DecoderNode*, size_t);

/**
 * Print a char array of specified length
 */
void printString(char*, size_t);

/**
 * Print a long long array of specified length
 */
void printLong(unsigned long*, size_t);

/**
 * Print Decoder-built Arborescence from Root
 */
void printFromDecoderRoot(DecoderNode *);

/**
 * Parse a number from a string
 */
long getNumber(char*);

/**
 * Check if a string is a CLI flag
 */
int isOption(char*);

/**
 * Check if a string matches one of two passed parameters
 */
int isAny(char*, char*, char*);

/**
 * Reverse Byte Order
 */
byte_t reverse_bits(byte_t);

/**
 * Swap two nodes
 */
void hffmn_swap(Node*, Node*);

/**
 * Build a heap from an array part
 */
void hffmn_heap(Node*, byte_t, byte_t);

/**
 * Descendant Heap Sort Algorithm
 */
void hffmn_heapSort(Node*, byte_t);

/**
 * Merge two subarrays into one
 */
void hffmn_merge(Node*, byte_t, byte_t, byte_t);

/**
 * Descendant Merge Sort Algorithm
 */
void hffmn_mergeSort(Node*, byte_t, byte_t);

/**
 * Shift Nodes of a NodeList until length
 */
int shift(Node*, Node*, byte_t, byte_t);

/**
 * Get NodeList Index from which begin to shift element
 */
unsigned getShiftIndex(size_t, Node*, byte_t);

/**
 * Shift Decoder Nodes of a NodeList until length
 */
int shiftDecoder(DecoderNode*, DecoderNode*, byte_t, byte_t);

/**
 * Get Decoder NodeList Index from which begin to shift element
 */
unsigned getShiftDecoderIndex(size_t, DecoderNode*, byte_t);

/**
 * Dictionary Creation
 */
void hffmn_mapper(Node*, byte_t*, byte_t, Dict);

/**
 * Dictionary Creation
 */
void hffmn_decoder_mapper(DecoderNode *, byte_t *, byte_t);

/**
 * Create a binary Node Tree from a Node List
 */
Node* hffmn_vegetalizer(Node*, size_t);

/**
 * Create a binary Decoder Node Tree from a Node List
 */
DecoderNode* hffmn_vegetalize_decoder(DecoderNode*, size_t);

/**
 * Return char mapping
 */
DictInfo hffmn_reduce(SortAlgorithm, unsigned long*);

/**
 * Decode an encoded character
 */
DecoderValue hffmn_decoder(byte_t *, size_t, size_t, DecoderNode *, byte_t);