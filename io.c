#include "headers.h"

int is_dir(char* path)
{
    struct stat statbuf;
    if(stat(path, &statbuf) != 0)
    {
        return 0;
    }
    return S_ISDIR(statbuf.st_mode);
}

void hffmn_write_dict(FILE* F, size_t file_size, DictInfo info)
{
    fprintf(F, "(%zu;%d", file_size, info.length);
    for(d_byte_t c = 0; c < info.length; c++)
    {
        if(info.dict[info.order[c]].weight > 0)
        {
            fprintf(F, ";%c,%.4f", info.order[c], (((double)info.dict[info.order[c]].weight / file_size) * 100));
        }
    }
    fprintf(F, ")");
}

size_t hffmn_read_dict(FILE* in_file, DecoderTree* tree) {;
    if(fseek(in_file, 0, SEEK_SET) != 0)
    {
        fprintf(stderr, "\n" H_R "Something went wront while moving read pointer through file." H_0);
        exit(EXIT_FAILURE);
    }

    size_t file_size = 0;
    unsigned short info_length = 0;

    if ( fscanf(in_file, "(%zu;%hu", &file_size, &info_length) != 2 ) {
        fprintf(stderr, "\n" H_R "Something went wrong while reading encoded file header -- hffmn_read_dict" H_0);
        exit(EXIT_FAILURE);
    }

    for (d_byte_t i = 0; i < info_length;i++) {
        if (fscanf(in_file,";%c",&(tree->nodes[i].ch)) < 1) {
            fprintf(stderr, "\n" H_R "Something went wrong while reading encoded file header -- hffmn_read_dict::scanning character" H_0);
            exit(EXIT_FAILURE);
        }
        if (fscanf(in_file, ",%lf",&(tree->nodes[i].weight)) < 1) {
            fprintf(stderr, "\n" H_R "Something went wrong while reading encoded file header -- hffmn_read_dict::scanning weight" H_0);
            exit(EXIT_FAILURE);
        }
        tree->length += 1;
    }

    return (size_t)ftell(in_file);
}

int hffmn_read_padding(FILE* in_file, size_t afterHeader) {
    if(fseek(in_file, -3, SEEK_END) != 0)
    {
        fprintf(stderr, "\n" H_R "Something went wront while moving read pointer through file." H_0);
        exit(EXIT_FAILURE);
    }
    int padding = 0;
    if (fscanf(in_file,"(%d)",&padding) < 1) {
        fprintf(stderr, "\n" H_R "Something went wrong while reading encoded file header -- hffmn_read_dict::scanning character" H_0);
        exit(EXIT_FAILURE);
    }
    if(fseek(in_file, afterHeader, SEEK_SET) != 0)
    {
        fprintf(stderr, "\n" H_R "Something went wront while moving read pointer through file." H_0);
        exit(EXIT_FAILURE);
    }
    return padding;
}

size_t hffmn_read_size(FILE* in_file, size_t afterHeader) {
    size_t size = 0;
    if(fseek(in_file, -3, SEEK_END) != 0)
    {
        fprintf(stderr, "\n" H_R "Something went wront while moving read pointer through file." H_0);
        exit(EXIT_FAILURE);
    }
    size = (size_t)ftell(in_file) - afterHeader;
    if(fseek(in_file, afterHeader, SEEK_SET) != 0)
    {
        fprintf(stderr, "\n" H_R "Something went wront while moving read pointer through file." H_0);
        exit(EXIT_FAILURE);
    }
    return size;
}

size_t hffmn_decode(char* in_filepath) {

    FILE * in_file;
    FILE * out_file;
    DecoderTree* Tr;
    DecoderNode* Ro;
    char* out_filepath;
    byte_t *in_buffer;
    // byte_t *out_buffer;
    size_t buffer_size;
    size_t read_progress;
    size_t afterHeader;
    size_t file_size;
    int padding;
    d_byte_t i;
    byte_t ext;
    byte_t found;
    byte_t last_chunk;

    if((out_filepath = (char*)calloc(256, sizeof(char))) == NULL)
    {
        fprintf(stderr, "\n" H_R "Something went wrong with calloc." H_0);
        exit(EXIT_FAILURE);
    }

    afterHeader = 0;
    file_size = 0;
    padding = 0;

    i = 0;
    ext = 0;
    found = 0;

    for (; (i < 256) && (found == 0); i++)
    {
        if ( ext == 0 && in_filepath[i] == '.' ) {
            ext++;
            i++;
        }
        if ( ext == 1 && in_filepath[i] == 'h') {
            ext++;
            i++;
        }
        else {
            ext = 0;
        }
        if ( ext == 2 && in_filepath[i] == 'f') {
            ext++;
            i++;
        }
        else {
            ext = 0;
        }
        if ( ext == 3 && in_filepath[i] == '\0') {
            ext++;
            found = 1;
        }
        else {
            ext = 0;
        }
    }

    if ( found == 0 ) {
        fprintf(stderr, "\n" H_R "Your path to encoded file is too long. Limit it to 256 characters." H_0);
        exit(EXIT_FAILURE);
    }
    else {
        for (byte_t j = 0; j < i - 4;j++) {
            out_filepath[j] = in_filepath[j];
        }
        out_filepath[i - 4] = '\0';
        strcat(out_filepath, ".uhf");
    }

    if ((in_file = fopen(in_filepath, "r")) == NULL)
    {
        exit(EXIT_FAILURE);
    }

    if((out_file = fopen(out_filepath, "w")) == NULL)
    {
        exit(EXIT_FAILURE);
    }

    if((Tr = (DecoderTree*)calloc(1, sizeof(DecoderTree))) == NULL)
    {
        fprintf(stderr, "\n" H_R "Something went wrong with calloc. -- hffmn_decode" H_0);
        exit(EXIT_FAILURE);
    }


    if((Tr->nodes = (DecoderNode*)calloc(256, sizeof(DecoderNode))) == NULL)
    {
        fprintf(stderr, "\n" H_R "Something went wrong with calloc. -- hffmn_decode" H_0);
        exit(EXIT_FAILURE);
    }

    Tr->length = 0;

    if((Ro = (DecoderNode*)calloc(1, sizeof(DecoderNode))) == NULL)
    {
        fprintf(stderr, "\n" H_R "Something went wrong with calloc. -- hffmn_decode" H_0);
        exit(EXIT_FAILURE);
    }

    afterHeader = hffmn_read_dict(in_file, Tr) + 1;

    padding = hffmn_read_padding(in_file, afterHeader);

    file_size = hffmn_read_size(in_file, afterHeader);

    printf("\nHeader size ............. : " H_C "%ld" H_0, afterHeader);
    printf("\nFile size ............... : " H_C "%ld" H_0, file_size);
    printf("\nPadding ................. : " H_C "%d" H_0, padding);

    Ro = hffmn_vegetalize_decoder(Tr->nodes, Tr->length);

    hffmn_decoder_mapper(Ro, (byte_t*)"", 0);

    buffer_size = 4096 * 128;
    read_progress = 0;
    last_chunk = 0;

    if ( file_size <= buffer_size ) {
        buffer_size = file_size;
        last_chunk = 1;
    }

    in_buffer = bcalloc(buffer_size);

    size_t shift = 0;

    while ( read_progress < file_size )
    {
        size_t buffer_read_size = 0;
        size_t rest_to_read = 0;

        rest_to_read = file_size - read_progress;

        if(last_chunk == 0)
        {
            last_chunk = rest_to_read <= buffer_size ? 1 : 0;
            if(last_chunk == 0)
            {
                rest_to_read = buffer_size;
            }
        }

        if ((buffer_read_size = fread(in_buffer, sizeof(byte_t), rest_to_read, in_file)) == rest_to_read) {
            reached("ici", buffer_read_size);

            size_t k = 0;
            for (; k < buffer_read_size;)
            {
                DecoderValue current;
                if (last_chunk > 0)
                {
                    current = hffmn_decoder(&(in_buffer[k]), buffer_read_size - k, shift, Ro, padding);
                }
                else {
                    current = hffmn_decoder(&(in_buffer[k]), buffer_read_size - k, shift, Ro, 0);
                }
                

                printf("\ndecoded: %c", current.ch);
                printf("\ndecoded bit: %d", current.bits_read);
                printf("\ndecoded char: %d", current.int_char_read);

                shift = current.bits_read - (current.int_char_read * 8);
                k += current.int_char_read;
                if (current.ch != 0) {
                    fwrite(&current.ch, 1, 1, out_file);
                }
            }
            read_progress += k;
        }
    }

    if(fclose(in_file) != 0)
    {
        perror("::");
        exit(EXIT_FAILURE);
    }
    in_file = NULL;

    if(fclose(out_file) != 0)
    {
        perror("::");
        exit(EXIT_FAILURE);
    }
    out_file = NULL;

    return 0;
}

size_t hffmn_write(char* in_filepath, DictInfo info)
{
    Dict dict;

    FILE* in_file;
    FILE* out_file;
    char* out_filepath;
    byte_t* in_buffer;
    byte_t* out_buffer;

    size_t buffer_size;
    size_t read_progress;
    size_t file_size;
    size_t backward;
    size_t i;
    size_t j;

    byte_t last_chunk;
    byte_t padding;

    dict = info.dict;

    if((out_filepath = (char*)calloc(sizeof(in_filepath) + 3, sizeof(in_filepath))) == NULL)
    {
        fprintf(stderr, "\n" H_R "Something went wrong with calloc." H_0);
        exit(EXIT_FAILURE);
    }

    strcpy(out_filepath, in_filepath);
    strcat(out_filepath, ".hf");

    if((in_file = fopen(in_filepath, "r")) == NULL)
    {
        perror("::");
        exit(EXIT_FAILURE);
    }

    if((out_file = fopen(out_filepath, "w")) == NULL)
    {
        perror("::");
        exit(EXIT_FAILURE);
    }

    if(fseek(in_file, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "\n" H_R "Something went wront while moving read pointer through file." H_0);
        exit(EXIT_FAILURE);
    }

    // Get file size
    file_size = ftell(in_file);

    if(fseek(in_file, 0, SEEK_SET) != 0)
    {
        fprintf(stderr, "\n" H_R "Something went wront while moving read pointer through file." H_0);
        exit(EXIT_FAILURE);
    }

    hffmn_write_dict(out_file, file_size, info);

    buffer_size   = 4096 * 128;
    read_progress = 0;
    last_chunk    = 0;
    padding       = 0;
    backward      = 0;

    if(file_size <= buffer_size)
    {
        buffer_size = file_size;
        last_chunk  = 1;
    }

    in_buffer = bcalloc(buffer_size);

    while(read_progress < file_size)
    {
        size_t rest_to_read     = 0;
        size_t buffer_read_size = 0;

        if(backward > 0)
        {
            fseek(in_file, read_progress, SEEK_SET);
            backward = 0;
        }

        rest_to_read = file_size - read_progress;
        if(last_chunk == 0)
        {
            last_chunk = rest_to_read <= buffer_size ? 1 : 0;
            if(last_chunk == 0)
            {
                rest_to_read = buffer_size;
            }
        }

        if((buffer_read_size = fread(in_buffer, sizeof(byte_t), rest_to_read, in_file)) == rest_to_read)
        {
            size_t out_buffer_cursor = 0;
            byte_t bit_count        = 0;

            for(i = 0; i < rest_to_read; i++)
            {
                size_t chunk_size      = 0; // char number to process
                size_t chunk_size_bits = 0; // huf char bit-sized length
                size_t chunk_size_bytes;
                size_t chunk_rest;
                byte_t hffmn_bit_cursor;

                for(j = i; j < rest_to_read; j++)
                {
                    chunk_size_bits += dict[in_buffer[j]].length;
                    chunk_size += 1;
                }

                if(last_chunk == 0)
                {
                    chunk_size_bytes = chunk_size_bits / 8;
                    chunk_rest       = chunk_size_bits & 7;

                    while(chunk_rest > 0)
                    {
                        --chunk_size;
                        --rest_to_read;
                        ++backward;
                        chunk_size_bits -= dict[in_buffer[--j]].length;
                        chunk_rest = chunk_size_bits & 7;
                    }
                }
                else
                {
                    chunk_size_bytes = chunk_size_bits / 8;
                    chunk_rest       = chunk_size_bits & 7;
                    if(chunk_rest > 0)
                    {
                        chunk_size_bytes++;
                        padding = 8 - chunk_rest;
                    }
                }

                out_buffer = bcalloc(chunk_size);

                for(j = i; j < (i + chunk_size); j++)
                {
                    for(hffmn_bit_cursor = 0; hffmn_bit_cursor < dict[in_buffer[j]].length; hffmn_bit_cursor++)
                    {
                        out_buffer[out_buffer_cursor] <<= 1;
                        if(dict[in_buffer[j]].bits[hffmn_bit_cursor] == 1)
                        {
                            out_buffer[out_buffer_cursor] |= 1;
                        }
                        bit_count++;
                        if((bit_count & 7) == 0)
                        {
                            out_buffer_cursor++;
                            bit_count = 0;
                        }
                    }
                }

                if(last_chunk == 1)
                {
                    for(; padding > 0; padding--)
                    {
                        out_buffer[out_buffer_cursor] <<= 1;
                        bit_count++;
                    }
                    padding = 8 - chunk_rest;
                }
                if(chunk_size_bytes != fwrite(out_buffer, 1, chunk_size_bytes, out_file))
                {
                    fprintf(stderr, "\n" H_R "File encoding has been stopped because something went wrong while writing encoded data." H_0);
                    exit(EXIT_FAILURE);
                }
                i += chunk_size;
            }
            read_progress += rest_to_read;
        }
        else
        {
            fprintf(stderr, "\n" H_R "Backward : %zu" H_0, backward);
            fprintf(stderr, "\n" H_R "Read state : %zu" H_0, buffer_read_size);
            fprintf(stderr, "\n" H_R "File could not be read entirely. Try again." H_0);
            exit(EXIT_FAILURE);
        }
    }

    fprintf(out_file, "(%o)", padding);

    if(fclose(in_file) != 0)
    {
        perror("::");
        exit(EXIT_FAILURE);
    }
    in_file = NULL;

    if(fclose(out_file) != 0)
    {
        perror("::");
        exit(EXIT_FAILURE);
    }
    out_file = NULL;

    return read_progress;
}

size_t* hffmn_sequential_reader(char* in_file_path)
{
    FILE* in_file;
    size_t* weights_array;
    size_t file_size;
    size_t read_progress;
    size_t i;

    in_file       = NULL;
    file_size     = 0;
    read_progress = 0;

    if((weights_array = (size_t*)calloc(256, sizeof(size_t))) == NULL)
    {
        perror("::");
        exit(EXIT_FAILURE);
    }

    /** File Opening */
    if((in_file = fopen(in_file_path, "r")) == NULL)
    {
        fprintf(stderr, "\n" H_R "Error %d: %s not open." H_0 "\n", errno, in_file_path);
        exit(EXIT_FAILURE);
    }

    if(fseek(in_file, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "\n" H_R "Something went wront while moving read pointer through file." H_0);
        exit(EXIT_FAILURE);
    }

    /** Get file size */
    file_size = ftell(in_file);

    if ( file_size == 0 ) {
        printf("\n" H_G "Nothing can be done for this file since it is empty." H_0);
        exit(EXIT_SUCCESS);
    }

    /** Place pointer to file beginning */
    if(fseek(in_file, 0, SEEK_SET) != 0)
    {
        fprintf(stderr, "\n" H_R "Something went wront while moving read pointer through file." H_0);
        exit(EXIT_FAILURE);
    }

    for(i = file_size; i > 0; i--, read_progress++)
    {
        weights_array[(byte_t)fgetc(in_file)] += 1;
    }

    /** Close file */
    if(fclose(in_file) != 0)
    {
        perror("::");
        exit(EXIT_FAILURE);
    }
    in_file = NULL;

    if(read_progress != file_size)
    {
        fprintf(stderr, "\n" H_R "File size is %zu but read byte number is %zu." H_0, file_size, read_progress);
        exit(EXIT_FAILURE);
    }

    return weights_array;
}

void* hffmn_thread_reader(void* arg)
{
    ThreadArguments* thread_arg = (ThreadArguments*)(arg);

    FILE* in_file;
    byte_t* buffer;
    size_t buffer_size;
    size_t read_progress;
    size_t i;

    if((in_file = fopen(thread_arg->file_path, "r")) == NULL)
    {
        perror("::");
        exit(EXIT_FAILURE);
    }

    if(fseek(in_file, thread_arg->start, SEEK_SET) != 0)
    {
        fprintf(stderr, "\n" H_R "Something went wront while moving read pointer through file." H_0);
        exit(EXIT_FAILURE);
    }

    buffer_size   = 4096 * 512;
    buffer        = bmalloc(buffer_size);
    read_progress = 0;

    while(read_progress < thread_arg->length)
    {
        size_t payload_rest = thread_arg->length - read_progress;
        if(thread_arg->length >= buffer_size)
        {
            payload_rest = payload_rest > buffer_size ? buffer_size : payload_rest;
        }
        if(fread(buffer, 1, payload_rest, in_file) == payload_rest)
        {
            for(i = 0; i < payload_rest; i++)
            {
                thread_arg->array[buffer[i]] += 1;
            }
            read_progress += payload_rest;
        }
        else
        {
            fprintf(stderr, "\n" H_R "File reading could not have been completed." H_0);
            exit(EXIT_FAILURE);
        }
    }

    /** Close file */
    if(fclose(in_file) != 0)
    {
        perror("::");
        exit(EXIT_FAILURE);
    }
    in_file = NULL;

    pthread_exit(NULL);
}

size_t* hffmn_parallel_reader(char* file_path, byte_t max_thread_number)
{
    /** Pointer Array to Weights Array */
    size_t** weights_arrays;
    /** File Descriptor */
    FILE* in_file;
    /** File Size */
    size_t file_size;
    /** Read bytes number */
    size_t read_progress;
    /** Initial payload to dispatch to threads */
    size_t initial_payload;
    /** Rest of payload to dispatch to threads */
    size_t payload_rest;
    /** Number of threads */
    byte_t thread_number;
    /** Number of weights arrays */
    byte_t arrays_number;
    /** Pthread Attributes */
    pthread_attr_t thread_attibutes;
    /** Incrementer */
    byte_t i;

    file_size     = 0;
    read_progress = 0;




    /** Set threads to be joinable through threads attributes */
    pthread_attr_init(&thread_attibutes);
    pthread_attr_setdetachstate(&thread_attibutes, PTHREAD_CREATE_JOINABLE);

    if((in_file = fopen(file_path, "r")) == NULL)
    {
        fprintf(stderr, "\n" H_R "Error %d: %s not open." H_0 "\n", errno, file_path);
        exit(EXIT_FAILURE);
    }

    /** Place pointer to the end of file */
    if(fseek(in_file, 0, SEEK_END) != 0)
    {
        fprintf(stderr, H_R "Guys, where is the file?" H_0);
        exit(EXIT_FAILURE);
    }

    /** Get File size */
    file_size = ftell(in_file);

    if ( file_size == 0 ) {
        printf("\n" H_G "Nothing can be done for this file since it is empty." H_0);
        exit(EXIT_SUCCESS);
    }

    /** Close file */
    if(fclose(in_file) != 0)
    {
        perror("::");
        exit(EXIT_FAILURE);
    }

    in_file = NULL;

    /** Compute number of threads depending on maximal thread number */
    thread_number = file_size / 1000;
    thread_number = thread_number == 0 ? 1 : (thread_number > max_thread_number ? max_thread_number : thread_number);
    arrays_number = thread_number + 1;

    /** Array List */

    if((weights_arrays = (size_t**)calloc(arrays_number, sizeof(size_t*))) == NULL)
    {
        perror("::");
        exit(EXIT_FAILURE);
    }

    for(i = 0; i < arrays_number; i++)
    {
        if((weights_arrays[i] = (size_t*)calloc(256, sizeof(size_t))) == NULL)
        {
            perror("::");
            exit(EXIT_FAILURE);
        }
    }




    /** Pthread IDs List */
    pthread_t thread_ids[thread_number];
    /** Thread Arg Store */
    ThreadArguments arg_store[thread_number];

    initial_payload = file_size / thread_number;
    payload_rest    = file_size - initial_payload * thread_number;

    for(i = 0; i < thread_number; i++)
    {
        arg_store[i].length   = initial_payload + (i < payload_rest);
        arg_store[i].start    = i < payload_rest ? (i * (initial_payload + 1)) : (payload_rest * (initial_payload + 1)) + ((i - payload_rest) * initial_payload);
        arg_store[i].array      = weights_arrays[i];
        arg_store[i].file_path = file_path;
        arg_store[i].id       = &thread_ids[i];
    }

    for(i = 0; i < thread_number; i++)
    {
        pthread_create(&thread_ids[i], &thread_attibutes, &hffmn_thread_reader, &arg_store[i]);
    }

    for(i = 0; i < thread_number; i++)
    {
        pthread_join(thread_ids[i], NULL);
    }

    /** Sum char count through threads result */
    for(d_byte_t l = 0; l < 256; l++)
    {
        for(i = 0; i < thread_number; i++)
        {
            weights_arrays[thread_number][l] += weights_arrays[i][l];
            read_progress += weights_arrays[i][l];
        }
    }




    if(read_progress != file_size)
    {
        fprintf(stderr, "\n" H_R "File size is %zu but read byte number is %zu." H_0, file_size, read_progress);
        exit(EXIT_FAILURE);
    }

    return weights_arrays[thread_number];
}

void hffmn_encode(char* file_path, Concurrency concurrency, SortAlgorithm sort, byte_t max_thread_number)
{
    unsigned long* weights_array;
    DictInfo info;

    switch(concurrency)
    {
        case HFFMN_SEQUENTIAL: {
            weights_array = hffmn_sequential_reader(file_path);
            break;
        }
        case HFFMN_PARALLEL: {
            weights_array = hffmn_parallel_reader(file_path, max_thread_number);
            break;
        }
        default: {
            exit(EXIT_FAILURE);
        }
    }

    info = hffmn_reduce(sort, weights_array);


    hffmn_write(file_path, info);
}
