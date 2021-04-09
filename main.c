#include "headers.h"

int main(int argc, char **argv)
{
    if (argc > 0)
    {
        char *path = NULL;
        Task task = HFFMN_TASK_UNDEFINED;
        Concurrency concurrency = HFFMN_CONCURRENCY_UNDEFINED;
        SortAlgorithm sort = HFFMN_SORT_UNDEFINED;
        byte_t max_thread_number = 0;

        for (int i = 1; i < argc; i++)
        {
            if (isOption(argv[i]))
            {
                if (isAny(argv[i], "-c", "--concurrency"))
                {
                    if ((i < argc - 1))
                    {
                        if (isAny(argv[i + 1], "p", "parallel"))
                        {
                            concurrency = HFFMN_PARALLEL;
                        }
                        else if (isAny(argv[i + 1], "s", "sequential"))
                        {
                            concurrency = HFFMN_SEQUENTIAL;
                        }
                        else
                        {
                            fprintf(stderr, H_R "<%s> is not a valid value for \"%s\" option." H_0 "\nFallback to parallel processing.\n", argv[i + 1], argv[i]);
                        }
                    }
                    else
                    {
                        fprintf(stdout, "\n" H_B "You did not provide any value to \"%s\" option." H_0 "\nFallback to parallel processing.\n", argv[i]);
                    }
                }
                else if (isAny(argv[i], "-s", "--sort"))
                {
                    if ((i < argc - 1))
                    {
                        if (isAny(argv[i + 1], "m", "merge"))
                        {
                            sort = HFFMN_MERGESORT;
                        }
                        else if (isAny(argv[i + 1], "h", "heap"))
                        {
                            sort = HFFMN_HEAPSORT;
                        }
                        else
                        {
                            fprintf(stderr, "\n" H_R "<%s> is not a valid value for \"%s\" option." H_0 "\nFallback to merge sort.\n", argv[i + 1], argv[i]);
                        }
                    }
                    else
                    {
                        fprintf(stdout, "\n" H_R "You did not provide any value to \"%s\" option." H_0 "\nFallback to merge sort.\n", argv[i]);
                    }
                }
                else if (isAny(argv[i], "-t", "--thread"))
                {
                    if ((i < argc - 1))
                    {
                        if ((strncmp(argv[i + 1], "auto", 4) == 0))
                        {
                            max_thread_number = MAX_THREAD;
                        }
                        else if (((max_thread_number = getNumber(argv[i + 1])) < 1) || (max_thread_number > LIMIT_MAX_THREAD))
                        {
                            max_thread_number = MAX_THREAD;
                            fprintf(stderr, H_R "<%s> is not a valid value for \"%s\" option." H_0 "\nMax number of threads set to 16.\n", argv[i + 1], argv[i]);
                        }
                    }
                    else
                    {
                        fprintf(stdout, H_B "You did not provide any value to \"%s\" option." H_0 "\nMax number of threads set to 16.\n", argv[i]);
                    }
                }
                else if (isAny(argv[i], "-d", "--decode"))
                {
                    task = HFFMN_DECODE;
                }
                else if (isAny(argv[i], "-e", "--encode"))
                {
                    task = HFFMN_ENCODE;
                }
                else
                {
                    fprintf(stderr, H_R "%s: This flag is not recognized. It will be ignored." H_0 "\n", argv[i]);
                }
            }
            else
            {
                if (((i == 1) && !isOption(argv[1])))
                {
                    if (access(argv[i], F_OK) != -1 && is_dir(argv[i]) != 1)
                    {
                        path = argv[i];
                    }
                    else
                    {
                        fprintf(stderr, H_R "File \"%s\" is not accessible or is a directory." H_0 "\n", argv[i]);
                        exit(EXIT_FAILURE);
                    }
                }
                else if ((i == (argc - 1) && ((isOption(argv[argc - 2]) == 0) || strncmp(argv[argc - 2], "--source", 8) == 0)) && is_dir(argv[i]) != 1)
                {
                    if (access(argv[i], F_OK) != -1 && is_dir(argv[i]) != 1)
                    {
                        path = argv[i];
                    }
                    else
                    {
                        fprintf(stderr, H_R "File \"%s\" is not accessible or is a directory." H_0 "\n", argv[i]);
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }

        if (path == NULL)
        {
            fprintf(stderr, H_R "You did not provide any file to (de)compress." H_0 "\n");
            exit(EXIT_FAILURE);
        }

        if (task == HFFMN_TASK_UNDEFINED)
        {
            // task = HFFMN_AUTO_DETECT; // Not yet
            task = HFFMN_ENCODE;
        }
        printf("\nWork ..................... : " H_G "%s" H_0, task == HFFMN_ENCODE ? "Encode" : "Decode");

        if (task == HFFMN_ENCODE)
        {

            if (concurrency == HFFMN_CONCURRENCY_UNDEFINED)
            {
                concurrency = HFFMN_PARALLEL;
            }
            printf("\nReading Strategy ......... : " H_G "%s" H_0, concurrency == HFFMN_PARALLEL ? "Parallel" : "Sequential");

            if (concurrency == HFFMN_PARALLEL)
            {
                if (max_thread_number < 1)
                {
                    max_thread_number = MAX_THREAD;
                }
                if (max_thread_number > LIMIT_MAX_THREAD)
                {
                    max_thread_number = LIMIT_MAX_THREAD;
                }
                printf("\nMax Thread Number ........ : " H_G "%d" H_0, max_thread_number);
            }

            if (sort == HFFMN_SORT_UNDEFINED)
            {
                sort = HFFMN_MERGESORT;
            }
            printf("\nSort Method .............. : " H_G "%s" H_0, sort == HFFMN_HEAPSORT ? "Heap Sort" : "Merge Sort");

            printf("\n");

            hffmn_encode(path, concurrency, sort, max_thread_number);
        }
        else
        {
            hffmn_decode(path);
        }
    }
    else
    {
        fprintf(stderr, "\n" H_R "You did not provide any arguments. Are you this dumb?" H_0 "\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}
