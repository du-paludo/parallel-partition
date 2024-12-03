# Multi-Partition with Parallelism

This repository contains the implementation and execution of a multi-partition program with parallelism. The primary goal is to evaluate the algorithm's efficiency by analyzing metrics such as execution time and partitioned elements throughput (MEPS) as a function of the number of threads used. Tests were conducted in a cluster environment with varying partition and thread configurations, and the results were analyzed through generated graphs.

## Compilation and Execution

The program is implemented in main.c, which contains all the necessary functions to execute the multi-partition algorithm with parallelism.

### Compilation

To compile the program, simply run:

```
make
```

### Execution

Run the program using:

```
./multi-partition <nPartition> <nThreads>
```

Where:

- `<nPartition>:` Number of elements in the partition vector.

- `<nThreads>:` Number of threads used to execute the algorithm.

**Note:** The number of input vector elements is fixed at 8 million in the code.

## Algorithm

### Vector Initialization

Upon execution, input and partition vectors are dynamically initialized with random long long values using the `long long gera_aleatorio_ll()` function. The partition vector is then sorted using the qsort function.

The vectors are replicated into larger vectors of size `(500 * 250 * 250)` to minimize cache effects during execution.

### Algorithm Execution

After initialization, the program enters a loop with `NTIMES` iterations, during which the multi-partition function is executed. The input and partition vectors are adjusted at the end of each iteration to force a cache reset.

### Thread Initialization

Threads are initialized the first time `multi_partition` is called. Each thread executes the thread_worker function. After initialization, the main thread sets the initialized flag to 1 and also executes `thread_worker`.

### `thread_worker` Function Execution

Inside the `thread_worker` function, each thread calculates the input vector indices it will analyze. The three-dimensional matrix `long long* partialResults[PARTITION_SIZE][MAX_THREADS]` is initialized within each thread. For every element in its range, the thread:

1. Executes the `upper_bound` function (a binary search) to find the element's partition.

2. Stores the element in its local results for that partition.

The `localPos[MAX_THREADS][PARTITION_SIZE]` matrix tracks the current index for each partition and thread.

### Memory Limitation and Dynamic Allocation

Each partition's `partialResults` vector is initialized with a size of 16 elements. When additional space is required, the vector size doubles. The currArrSize vector tracks the allocated size for each partition and thread.

Due to memory constraints, the heap cannot accommodate all potential allocations. For example, with `nElements = 8 million` and `PARTITION_SIZE = 100,000`, the theoretical allocation would exceed heap capacity. Thus, dynamic resizing is employed.

### Synchronization and Processing

Threads perform calculations within an infinite loop, waiting at a barrier to start and finish each calculation. This thread pool implementation ensures threads only process data when work is available and execute in parallel. Once all threads finish their calculations, the main thread processes the partial results.

### Output Vector Composition

The output vector is constructed by iterating through the partial results for each partition and thread. The elements are naturally in order due to proportional thread input indices. The position vector is formed by summing the `localPos` of all threads for each partition.

## Results and Metrics

At the end of the execution, the program outputs:

- Total execution time (in seconds).

- Average execution time per iteration.

- Throughput in millions of elements partitioned per second (MEPS).

## Scripts and Testing

### Execution Script

The `roda-todos-slurm.s`h script automates multiple executions of the multi-threaded program. It is configured with:

- `NTIMES = 10`

- `MAX_EXECS = 5`

### Usage

To run the script locally:

```
./roda-todos-slurm.sh <nPartitions>
```

To run the script in a cluster environment:

```
sbatch --exclusive -N 1 roda-todos-slurm.sh <nPartitions>
```

### Test Configurations

The script was executed with `<nPartitions> = 1000` and `100,000` for both parts A and B.

Results were logged in `saida.txt` and processed using professor-provided spreadsheets to generate graphs.

### Limitations

When running with `100,000` partitions and `8` threads, execution may fail due to cluster node time limits. To address this, the script was executed separately with these configurations.

## Conclusion

The tests demonstrate the algorithm's scalability with an increasing number of threads, highlighting the importance of parallelism for processing large data volumes. Collected metrics and generated graphs provide insights into the impact of thread count on execution time and system throughput.
