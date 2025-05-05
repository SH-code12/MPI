#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
# include <stdbool.h>

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    int x,y,rank, size, n, elements_per_process, elements_received, start_index_received;


    MPI_Status status;

    double start_time = MPI_Wtime();

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //=========================================
    if (rank == 0) { // Master Process
        //=========================================
        printf("Hello from master process.\n");
        printf("Number of slave processes is %d\n", size - 1);
        printf("Please enter Lower bound...\n");
        scanf("%d", &x);
        printf("Please enter Upper bound...\n");
        scanf("%d", &y);
        //=================================================
        int range= (y-x);
        elements_per_process = range / (size - 1);
        int start_index;
        if (size > 1) {
            int i;
            for (i = 0; i < size - 2; i++) {
                start_index = i * elements_per_process;
                MPI_Send(&elements_per_process, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
                MPI_Send(&start_index, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            }

            // last process takes remaining
            start_index = i * elements_per_process;
            int remaining_eles = range - start_index;
            MPI_Send(&remaining_eles, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(&start_index, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
        }

        int count = 0;

        for (int i = 1; i < size; i++) {
            int temp;
            MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            count +=temp;
        }

        double end_time = MPI_Wtime();

        printf("Master process announce the prime numbers within given input range is %d.\n", count);
        printf("Program 2 using [MPI_Send - MPI_Recv] Execution Time: %f seconds\n", end_time - start_time);
        printf("Thanks for using our program\n");
    }

    //=========================================
    else { // Slave Processes
        MPI_Recv(&elements_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&start_index_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        int subarray_count=0;
        for (int i = start_index_received+1; i <= (elements_received + start_index_received); i++) {
            if(isPrime(i)){subarray_count++;}
        }
        //printf("Hello from slave#%d prime count in my partition is %d.\n", rank, subarray_count);
        MPI_Send(&subarray_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    //=========================================
    MPI_Finalize();
    return 0;
}
