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
#define DEBUG 0

#define ll long long

pthread_t threads[MAX_THREADS];
int threads_ids[MAX_THREADS];
pthread_barrier_t thread_barrier;

ll* partialResults[MAX_THREADS]; // Resultado do array para cada thread
int* localPos[MAX_THREADS]; // Índice inicial para cada thread

int nElements, nPartition, nThreads;

#if DEBUG
ll input[] = {8, 4, 13, 7, 11, 100, 44, 3, 7, 7, 100, 110, 46, 44};
ll partitionArr[] = {0, 12, 70, 90, LLONG_MAX};
#else
ll* input;
ll* partitionArr;
#endif

void printLLArray(ll* arr, int arrSize) {
    printf("[");
    for (int i = 0; i < arrSize-1; i++) {
        printf("%lld, ", arr[i]);
    }
    printf("%lld]\n", arr[arrSize-1]);
}

void printIntArray(int* arr, int arrSize) {
    printf("[");
    for (int i = 0; i < arrSize-1; i++) {
        printf("%d, ", arr[i]);
    }
    printf("%d]\n", arr[arrSize-1]);
}

int compare(const void* a, const void* b) {
   return (*(ll*)a - *(ll*)b);
}

long long geraAleatorioLL() {
    int a = rand(); // Returns a pseudo-random integer between 0 and RAND_MAX.
    int b = rand();
    // long long v = (long long)a * 100 + b;
    long long v = ((long long)a * 100 + b) % 200;
    return v;
}

void calculateIndexes(int threadIndex, int* first, int* last) {
    int baseChunkSize = nPartition / nThreads;
    int remainder = nPartition % nThreads;

    if (threadIndex < remainder) {
        *first = threadIndex * (baseChunkSize + 1) + 1;
        *last = *first + baseChunkSize;
    } else {
        *first = threadIndex * baseChunkSize + remainder + 1;
        *last = *first + baseChunkSize - 1;
    }
}

void* thread_worker(void* ptr) {
    int index = *((int*) ptr);
    int first, last;
    calculateIndexes(index, &first, &last);
    // printf("Thread %d: first = %d, last = %d\n", index, first, last);

    localPos[index] = malloc(sizeof(int) * (last-first+2));

    while (1) {
        pthread_barrier_wait(&thread_barrier);

        int k = 0;
        for (int i = first; i <= last; i++) {
            for (int j = 0; j < nElements; j++) {
                if (input[j] >= partitionArr[i-1] && input[j] < partitionArr[i]) {
                    partialResults[index][k] = input[j];
                    k++;
                }
            }
            localPos[index][i - first] = k;
        }
        localPos[index][last-first+1] = -1;
        partialResults[index][k] = -1;
        
        #if DEBUG
        printf("Thread %d: ", index);
        printLLArray(partialResults[index], k);
        printf("Thread %d: localPos: ", index);
        printIntArray(localPos[index], last-first+1);
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
    for (int i = 0; i < nThreads; i++) {
        int j = 0;
        while (partialResults[i][j] != -1) {
            output[k] = partialResults[i][j];
            j++;
            k++;
        }
    }
    
    pos[0] = 0;
    int previousPos = 0;
    k = 1;
    for (int i = 0; i < nThreads; i++) {
        int j = 0;
        while (localPos[i][j] != -1) {
            // printf("localPos[%d][%d] = %d\n", i, j, localPos[i][j]);
            pos[k] = localPos[i][j] + previousPos;
            j++;
            k++;
        }
        previousPos = pos[k-1];
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    nElements = 16000000;

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
        input[i] = geraAleatorioLL();
    }
    #endif

    #if DEBUG
    nPartition = 4;
    #else
    partitionArr = malloc(sizeof(ll) * (nPartition+1));
    partitionArr[0] = 0;
    for (int i = 1; i < nPartition; i++) {
        partitionArr[i] = geraAleatorioLL();
    }
    partitionArr[nPartition] = LLONG_MAX;
    qsort(partitionArr, nPartition, sizeof(ll), compare);
    #endif

    ll* output = malloc(sizeof(ll) * nElements);
    int* pos = malloc(sizeof(int) * nPartition);

    for (int i = 0; i < nThreads; i++) {
        partialResults[i] = malloc(sizeof(ll) * nElements);
    }

    #if DEBUG
    printf("Input array: ");
    printLLArray(input, nElements);
    printf("Partition array: ");
    printLLArray(partitionArr, nPartition+1);
    #endif

    chronometer_t parallelPartitionTime;
    chrono_reset(&parallelPartitionTime);
    chrono_start(&parallelPartitionTime);

    // Execução
    multi_partition(input, nElements, partitionArr, nPartition, output, pos);

    #if DEBUG
    printf("Output array: ");
    printLLArray(output, nElements);
    printf("Pos array: ");
    printIntArray(pos, nPartition);
    #endif

    chrono_stop(&parallelPartitionTime);
    chrono_reportTime(&parallelPartitionTime, "parallelPartitionTime");

    double total_time_in_nanoseconds = (double) chrono_gettotal(&parallelPartitionTime);
    printf("Total time: %lf s\n", total_time_in_nanoseconds / (1000 * 1000 * 1000));
                                  
    double ops = nElements/total_time_in_nanoseconds*(1000*1000*1000);
    printf("Throughput: %lf OP/s\n", ops);

    verifica_particoes(input, nElements, partitionArr, nPartition, output, pos);

    #if !DEBUG
    free(input);
    free(partitionArr);
    #endif
    free(output);
    free(pos);

    for (int i = 0; i < nThreads; i++) {
        free(partialResults[i]);
        free(localPos[i]);
    }

    return 0;
}