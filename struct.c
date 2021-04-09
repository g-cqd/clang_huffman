#include "headers.h"

byte_t reverse_bits(byte_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void hffmn_swap(Node* a, Node* b)
{
    Node tmp = *a;
    *a       = *b;
    *b       = tmp;
}

void hffmn_heap(Node* list, byte_t end, byte_t mid)
{
    byte_t min   = mid;
    byte_t left  = 2 * mid + 1;
    byte_t right = 2 * mid + 2;

    if(left < end && list[left].weight < list[min].weight)
    {
        min = left;
    }

    if(right < end && list[right].weight < list[min].weight)
    {
        min = right;
    }

    if(min != mid)
    {
        hffmn_swap(&list[mid], &list[min]);
        hffmn_heap(list, end, min);
    }
}

void hffmn_heapSort(Node* list, byte_t length)
{
    const byte_t end = length;
    for(int i = end / 2 - 1; i >= 0; i--)
    {
        hffmn_heap(list, end, i);
    }
    for(int i = end - 1; i >= 0; i--)
    {
        hffmn_swap(&(list)[0], &(list)[i]);
        hffmn_heap(list, i, 0);
    }
}

void hffmn_merge(Node* list, byte_t start, byte_t middle, byte_t end)
{
    register byte_t i, j, k;
    byte_t length1 = middle - start + 1;
    byte_t length2 = end - middle;

    Node* leftPart;
    Node* rightPart;

    if((leftPart = (Node*)malloc(length1 * sizeof(Node))) == NULL)
    {
        fprintf(stderr, H_R "malloc did not work as expected." H_0 "\n");
        exit(EXIT_FAILURE);
    }
    if((rightPart = (Node*)malloc(length2 * sizeof(Node))) == NULL)
    {
        fprintf(stderr, H_R "malloc did not work as expected." H_0 "\n");
        exit(EXIT_FAILURE);
    }

    /**
     * Copy Array Parts
     */
    for(i = 0; i < length1; i++)
    {
        leftPart[i] = list[start + i];
    }
    for(j = 0; j < length2; j++)
    {
        rightPart[j] = list[middle + 1 + j];
    }

    /**
     * Merge subarrays into arr[]
     */
    i = 0;
    j = 0;
    k = start;
    while(i < length1 && j < length2)
    {
        if(leftPart[i].weight > rightPart[j].weight)
        {
            list[k] = leftPart[i];
            i++;
        }
        else
        {
            list[k] = rightPart[j];
            j++;
        }
        k++;
    }

    /**
     * Copy the remaining elements of leftPart, if there are any
     */
    while(i < length1)
    {
        list[k] = leftPart[i];
        i++;
        k++;
    }

    /**
     * Copy the remaining elements of rightPart, if there are any
     */
    while(j < length2)
    {
        list[k] = rightPart[j];
        j++;
        k++;
    }
}

void hffmn_mergeSort(Node* list, byte_t start, byte_t end)
{
    if(start < end)
    {
        byte_t middle = start + (end - start) / 2;
        hffmn_mergeSort(list, start, middle);
        hffmn_mergeSort(list, middle + 1, end);
        hffmn_merge(list, start, middle, end);
    }
}

int shift(Node* element, Node* list, byte_t index, byte_t length)
{
    byte_t l = length - 1;
    byte_t x = index;
    for(unsigned i = l; i >= x; i--)
    {
        if(i == x)
        {
            // When i does equal index, insert element
            list[i] = *element;
            return 1;
        }
        else
        {
            // While i does not equal index shift from a position to position + 1
            list[i] = list[i - 1];
        }
    }
    return -1;
}

unsigned getShiftIndex(size_t weight, Node* list, byte_t length)
{
    for(byte_t i = length; i > 0; i--)
    {
        if(list[i - 1].weight >= weight)
        {
            return i;
        }
    }
    return 0;
}

int shiftDecoder(DecoderNode* element, DecoderNode* list, byte_t index, byte_t length)
{
    byte_t l = length - 1;
    byte_t x = index;
    for(unsigned i = l; i >= x; i--)
    {
        if(i == x)
        {
            // When i does equal index, insert element
            list[i] = *element;
            return 1;
        }
        else
        {
            // While i does not equal index shift from a position to position + 1
            list[i] = list[i - 1];
        }
    }
    return -1;
}

unsigned getShiftDecoderIndex(size_t weight, DecoderNode* list, byte_t length)
{
    for(byte_t i = length; i > 0; i--)
    {
        if(list[i - 1].weight >= weight)
        {
            return i;
        }
    }
    return 0;
}

void hffmn_mapper(Node* node, byte_t* prec, byte_t prec_len, Dict map)
{
    byte_t* arrayLeft  = bcalloc(prec_len + 1);
    byte_t* arrayRight = bcalloc(prec_len + 1);

    if(prec_len > 0)
    {
        cpy(arrayLeft, prec, prec_len);
        cpy(arrayRight, prec, prec_len);
    }

    if(node->ch == '\0')
    {
        if(node->left->weight > 0)
        {
            add(0, arrayLeft, prec_len);
            hffmn_mapper(node->left, arrayLeft, prec_len + 1, map);
        }
        if(node->right->weight > 0)
        {
            add(1, arrayRight, prec_len);
            hffmn_mapper(node->right, arrayRight, prec_len + 1, map);
        }
    }
    else
    {
        map[(byte_t)node->ch].bits = bcalloc(prec_len);
        cpy(map[(byte_t)node->ch].bits, prec, prec_len);
        map[(byte_t)node->ch].weight = node->weight;
        map[(byte_t)node->ch].length = (byte_t)(prec_len);
    }
}

void hffmn_decoder_mapper(DecoderNode* node, byte_t* prec, byte_t prec_len)
{
    byte_t* arrayLeft  = bcalloc(prec_len + 1);
    byte_t* arrayRight = bcalloc(prec_len + 1);

    if(prec_len > 0)
    {
        cpy(arrayLeft, prec, prec_len);
        cpy(arrayRight, prec, prec_len);
    }

    if(node->ch == '\0')
    {
        if(node->left->weight > 0)
        {
            add(0, arrayLeft, prec_len);
            hffmn_decoder_mapper(node->left, arrayLeft, prec_len + 1);
        }
        if(node->right->weight > 0)
        {
            add(1, arrayRight, prec_len);
            hffmn_decoder_mapper(node->right, arrayRight, prec_len + 1);
        }
    }
    else
    {
        node->bits = bcalloc(prec_len);
        cpy(node->bits, prec, prec_len);
        node->weight = node->weight;
        node->length = (byte_t)(prec_len);
    }
}

Node* hffmn_vegetalizer(Node* list, size_t length)
{
    for(d_byte_t i = length - 1; i >= 1; i--)
    {
        Node* node;
        if((node = (Node*)calloc(1,sizeof(Node))) == NULL)
        {
            fprintf(stderr, H_R "calloc did not work as expected." H_0 "\n");
            exit(EXIT_FAILURE);
        }
        node->weight = list[i].weight + list[i - 1].weight;
        if((node->right = (Node*)calloc(1,sizeof(Node))) == NULL)
        {
            fprintf(stderr, H_R "calloc did not work as expected." H_0 "\n");
            exit(EXIT_FAILURE);
        }
        *(node->right) = list[i];
        if((node->left = (Node*)calloc(1,sizeof(Node))) == NULL)
        {
            fprintf(stderr, H_R "calloc did not work as expected." H_0 "\n");
            exit(EXIT_FAILURE);
        }
        *(node->left) = list[i - 1];
        shift(node, list, getShiftIndex((size_t)(node->weight), list, i), i);
    }
    return list;
}

DecoderNode* hffmn_vegetalize_decoder(DecoderNode* list, size_t length)
{
    for(d_byte_t i = length - 1; i >= 1; i--)
    {
        DecoderNode* node;
        if((node = (DecoderNode*)calloc(1,sizeof(DecoderNode))) == NULL)
        {
            fprintf(stderr, H_R "calloc did not work as expected." H_0 "\n");
            exit(EXIT_FAILURE);
        }
        node->ch = '\0';
        node->weight = list[i].weight + list[i - 1].weight;
        if((node->right = (DecoderNode*)calloc(1,sizeof(DecoderNode))) == NULL)
        {
            fprintf(stderr, H_R "calloc did not work as expected." H_0 "\n");
            exit(EXIT_FAILURE);
        }
        *(node->right) = list[i];
        if((node->left = (DecoderNode*)calloc(1,sizeof(DecoderNode))) == NULL)
        {
            fprintf(stderr, H_R "calloc did not work as expected." H_0 "\n");
            exit(EXIT_FAILURE);
        }
        *(node->left) = list[i - 1];
        shiftDecoder(node, list, getShiftDecoderIndex((size_t)(node->weight), list, i), i);
    }
    return list;
}

DictInfo hffmn_reduce(SortAlgorithm sort, size_t* arr)
{
    Node* root;
    byte_t* order;
    Dict map;
    Tree tr;
    DictInfo info;

    if((tr.nodes = (Node*)calloc(256, sizeof(Node))) == NULL)
    {
        fprintf(stderr, "\n" H_R "Something went wrong with calloc. -- hffmn_reduce" H_0);
        exit(EXIT_FAILURE);
    }

    tr.length = 0;

    if((root = (Node*)calloc(1, sizeof(Node))) == NULL)
    {
        fprintf(stderr, "\n" H_R "Something went wrong with calloc. -- hffmn_reduce" H_0);
        exit(EXIT_FAILURE);
    }

    if((map = (Dict)calloc(256, sizeof(DictItem))) == NULL)
    {
        fprintf(stderr, "\n" H_R "Something went wrong with calloc. -- hffmn_reduce" H_0);
        exit(EXIT_FAILURE);
    }


    /** Initialize Dict */
    for(byte_t b = 0; b < 255; b++)
    {
        map[b].weight = 0;
        map[b].length = 0;
        if((map[b].bits = (byte_t*)calloc(128, sizeof(byte_t))) == NULL)
        {
            fprintf(stderr, "\n" H_R "Something went wrong with calloc. -- hffmn_reduce" H_0);
            exit(EXIT_FAILURE);
        }
    }

    for(d_byte_t u = 0; u < 255; u++)
    {
        if(arr[u] > 0)
        {
            tr.nodes[tr.length].ch     = u;
            tr.nodes[tr.length].weight = arr[u];
            if((tr.nodes[tr.length].left = (Node*)calloc(1, sizeof(Node))) == NULL)
            {
                fprintf(stderr, "\n" H_R "Something went wrong with calloc. -- hffmn_reduce" H_0);
                exit(EXIT_FAILURE);
            }
            if((tr.nodes[tr.length].right = (Node*)calloc(1, sizeof(Node))) == NULL)
            {
                fprintf(stderr, "\n" H_R "Something went wrong with calloc. -- hffmn_reduce" H_0);
                exit(EXIT_FAILURE);
            }
            tr.length += 1;
        }
    }

    /**
     * Choose between two Sort Algorithms
     *
     * Change Sorting Algorithm, changes Huffman Encoding
     * Also, mergeSort is parallelizable, whether heapSort is not
     */
    switch(sort)
    {
        case HFFMN_HEAPSORT: {
            hffmn_heapSort(tr.nodes, tr.length);
            break;
        }
        case HFFMN_MERGESORT: {
            hffmn_mergeSort(tr.nodes, 0, tr.length);
            break;
        }
        default: {
            exit(EXIT_FAILURE);
        }
    }

    order = bcalloc(tr.length);

    for(byte_t c = 0; c < tr.length; c++)
    {
        order[c] = tr.nodes[c].ch;
    }

    /** Huffman Tree Building */
    root = hffmn_vegetalizer(tr.nodes, tr.length);

    /** Create the map */
    hffmn_mapper(root, (byte_t*)"", 0, map);

    info.dict    = map;
    info.order  = order;
    info.length = tr.length;

    return info;
}

DecoderValue hffmn_decoder(byte_t *buffer, size_t length, size_t shift, DecoderNode* Root,byte_t padding) {
    DecoderNode *current = Root;
    DecoderValue value;
    size_t bit_rest = 8;
    byte_t found = 0;
    size_t length_bit = length * 8 - (shift + padding);
    value.ch = 0;
    value.bits_read = 0;
    value.int_char_read = 0;

    byte_t read_value = buffer[0];

    printf("\nread char: %c", read_value);

    read_value = reverse_bits(read_value);

    if ( shift > 0 ) {
        read_value >>= shift;
        bit_rest -= shift;
        value.bits_read += shift;
    }

    printf("\n" H_G "______________" H_0);

    for (size_t i = 0; i < length && found == 0;) {
        reached("read value : ",read_value);
        if ((read_value & 1) == 1)
        {
            current = current->right;
            value.bits_read++;
        }
        else if ( (read_value & 1) == 0) {
            current = current->left;
            value.bits_read++;
        }

        bit_rest--;
        if (bit_rest == 0) {
            i++;
            bit_rest = 8;
            read_value = buffer[i];
            read_value = reverse_bits(read_value);
            value.int_char_read++;
        }
        else {
            read_value >>= 1;
        }
        if ( current->ch != '\0') {
            found = 1;
            value.ch = current->ch;
        }
    }
    return value;
}
