// Eduardo Stefanel Paludo - GRR20210581
// Natael Pontarolo Gomes - GRR20211786

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <limits.h>

#include "chrono.h"
#include "verifica_particoes.h"

#define MAX_THREADS 8
#define PARTITION_SIZE 10000
#define DEBUG 0

#define ll long long
#define MAX_TOTAL_ELEMENTS (500*250*250)

pthread_t threads[MAX_THREADS];
int threads_ids[MAX_THREADS];
pthread_barrier_t thread_barrier;
ll inputG[MAX_TOTAL_ELEMENTS];
ll partitionArrG[MAX_TOTAL_ELEMENTS];

// ll* partialResults[MAX_THREADS]; // Resultado do array para cada thread
// int* localPos[MAX_THREADS]; // Índice inicial para cada thread

int localPos[MAX_THREADS][PARTITION_SIZE];
ll* partialResults[PARTITION_SIZE][MAX_THREADS]; // Resultado do array para cada partição

int nElements, nPartition, nThreads;

#if DEBUG
ll input[] = {8, 4, 13, 7, 11, 100, 44, 3, 7, 7, 100, 110, 46, 44};
ll partitionArr[] = {12, 70, 90, LLONG_MAX};
#else
ll* input;
ll* partitionArr;
#endif

void print_ll_array(ll* arr, int arrSize) {
    printf("[");
    for (int i = 0; i < arrSize-1; i++) {
        printf("%lld, ", arr[i]);
    }
    printf("%lld]\n", arr[arrSize-1]);
}

void print_int_array(int* arr, int arrSize) {
    printf("[");
    for (int i = 0; i < arrSize-1; i++) {
        printf("%d, ", arr[i]);
    }
    printf("%d]\n", arr[arrSize-1]);
}

int compare(const void* a, const void* b) {
   return (*(ll*)a - *(ll*)b);
}

long long gera_aleatorio_ll() {
    int a = rand(); // Returns a pseudo-random integer between 0 and RAND_MAX.
    int b = rand();
    // long long v = (long long)a * 100 + b;
    long long v = ((long long)a * 100 + b) % 200;
    return v;
}

void calculate_indexes(int threadIndex, int* first, int* last) {
    int baseChunkSize = nElements / nThreads;
    int remainder = nElements % nThreads;

    if (threadIndex < remainder) {
        *first = threadIndex * (baseChunkSize + 1);
        *last = *first + baseChunkSize;
    } else {
        *first = threadIndex * baseChunkSize + remainder;
        *last = *first + baseChunkSize - 1;
    }
}

int upper_bound(ll arr[], int n, int target) {
    int lo = 0, hi = n - 1;
    int res = n;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;

        if (arr[mid] > target) {
            res = mid;
            hi = mid - 1;
        }
        else {
            lo = mid + 1;
        }
    }
    return res;
}

void* thread_worker(void* ptr) {
    int index = *((int*) ptr);
    int first, last;
    calculate_indexes(index, &first, &last);

    for (int i = 0; i < PARTITION_SIZE; i++) {
        localPos[index][i] = 0;
    }

    while (1) {
        pthread_barrier_wait(&thread_barrier);

        for (int i = first; i <= last; i++) {
            // Retorna o índice da partição que o elemento pertence
            int partitionIdx = upper_bound(partitionArr, nPartition, input[i]);
            partialResults[partitionIdx][index][localPos[index][partitionIdx]] = input[i];
            localPos[index][partitionIdx]++;
        }
        
        #if DEBUG
        // printf("Thread %d: ", index);
        // printLLArray(partialResults[index], k);
        // printf("Thread %d: localPos: ", index);
        // printIntArray(localPos[index], last-first+1);
        #endif

        pthread_barrier_wait(&thread_barrier);

        if (index == 0) {
            return NULL;
        }
    }
    return NULL;
}

void multi_partition(ll* input, int n, ll* P, int np, ll* output, int* pos) {
    pthread_barrier_init(&thread_barrier, NULL, nThreads);

    threads_ids[0] = 0;
    for (int i = 1; i < nThreads; i++) {
        threads_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_worker, &threads_ids[i]);
    }
    thread_worker(&threads_ids[0]);

    int k = 0;
    for (int i = 0; i <  np; i++) {
        for (int j = 0; j < nThreads; j++) {
            for (int l = 0; l < localPos[j][i]; l++) {
                output[k] = partialResults[i][j][l];
                k++;
            }
        }
    }
    
    pos[0] = 0;
    for (int i = 0; i < np-1; i++) {
        pos[i+1] = pos[i];
        for (int j = 0; j < nThreads; j++) {
            pos[i+1] += localPos[j][i];
        }
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    nElements = 8000000;
    int NTIMES = 10;

    if (argc != 3) {
         printf("Usage: %s <nPartition> <nThreads>\n", argv[0]); 
         return 0;
    } else {
        nThreads = atoi(argv[2]);
        if (nThreads == 0) {
            printf("Usage: %s <nPartition> <nThreads>\n", argv[0]);
            printf("<nThreads> can't be 0\n");
            return 0;
        }     
        if (nThreads > MAX_THREADS) {  
            printf("Usage: %s <nPartition> <nThreads>\n", argv[0]);
            printf( "<nThreads> must be less than %d\n", MAX_THREADS);
            return 0;
        }
        nPartition = atoi(argv[1]);
    }
    printf("Will use %d threads to run partition, with %d total long long elements on input array and %d elements on partition array\n", nThreads, nElements, nPartition);

    // Inicializações
    #if DEBUG
    nElements = 14;
    #else
    input = malloc(sizeof(ll) * nElements);
    for (int i = 0; i < nElements; i++) {
        input[i] = gera_aleatorio_ll();
    }
    #endif

    for (int i = 0; i < MAX_TOTAL_ELEMENTS ; i++) {
        inputG[i] = input[i%nElements];
    }

    free(input);

    #if DEBUG
    nPartition = 4;
    #else
    partitionArr = malloc(sizeof(ll) * (nPartition));
    for (int i = 0; i < nPartition; i++) {
        partitionArr[i] = gera_aleatorio_ll();
    }
    qsort(partitionArr, nPartition-1, sizeof(ll), compare);
    partitionArr[nPartition-1] = LLONG_MAX;
    #endif


    for (int i = 0; i < MAX_TOTAL_ELEMENTS; i++) {
        partitionArrG[i] = partitionArr[i%nPartition];
    }

    free(partitionArr);

    ll* output = malloc(sizeof(ll) * nElements);
    int* pos = malloc(sizeof(int) * nPartition);

    for (int i = 0; i < nPartition; i++) {
        for (int j = 0; j < nThreads; j++) {
            partialResults[i][j] = malloc(sizeof(ll) * nElements/nThreads);
        }
    }

    #if DEBUG
    printf("Input array: ");
    printLLArray(input, nElements);
    printf("Partition array: ");
    printLLArray(partitionArr, nPartition);
    #endif

    chronometer_t parallelPartitionTime;
    chrono_reset(&parallelPartitionTime);
    chrono_start(&parallelPartitionTime);

    // Execução
    int start_position_input = 0;
    int start_position_partition = 0;
    input = &inputG[start_position_input];
    partitionArr = &partitionArrG[start_position_partition];
    for (int i = 0; i < NTIMES; i++) {
        multi_partition(input, nElements, partitionArr, nPartition, output, pos);

        start_position_input += nElements;
        start_position_partition += nPartition;

        if ((start_position_input + nElements) > MAX_TOTAL_ELEMENTS) 
            start_position_input = 0;
        input = &inputG[start_position_input];

        if ((start_position_partition + nPartition )> MAX_TOTAL_ELEMENTS) 
            start_position_partition = 0;
        partitionArr = &partitionArrG[start_position_partition];
    }
    
    // multi_partition(input, nElements, partitionArr, nPartition, output, pos);

    #if DEBUG
    printf("Output array: ");
    print_ll_array(output, nElements);
    printf("Pos array: ");
    print_int_array(pos, nPartition);
    #endif

    chrono_stop(&parallelPartitionTime);
    chrono_reportTime(&parallelPartitionTime, "parallelPartitionTime");

    double total_time_in_nanoseconds = (double) chrono_gettotal(&parallelPartitionTime);
    printf("Total time: %lf s\n", total_time_in_nanoseconds / (1000 * 1000 * 1000));
                                  
    double ops = nElements/total_time_in_nanoseconds*(1000*1000*1000);
    printf("Throughput: %lf OP/s\n", ops);

    verifica_particoes(input, nElements, partitionArr, nPartition, output, pos);

    #if !DEBUG
    // free(input);
    // free(partitionArr);
    #endif
    free(output);
    free(pos);

    for (int i = 0; i < nPartition; i++) {
        for (int j = 0; j < nThreads; j++) {
            free(partialResults[i][j]);
        }
    }

    return 0;
}