#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

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
    int rank, size, x, y, range, elements_per_process;

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

        range = y - x;
        elements_per_process = range / (size - 1);
    }

    // Broadcast the essential variables to all processes
    int bcast_data[3] = {range, elements_per_process, size};
    MPI_Bcast(bcast_data, 3, MPI_INT, 0, MPI_COMM_WORLD);
    range = bcast_data[0];
    elements_per_process = bcast_data[1];
    size = bcast_data[2]; // In case some MPI implementations modify size

    //================================================
    int start_index = (rank-1) * elements_per_process;
    int end_index;
    if(rank == size - 1){
         end_index = start_index + elements_per_process;
    }
    else{
         end_index = (start_index + elements_per_process) + ( range % (size - 1) );
    }
    int subarray_count=0;
    for(int i = start_index+1; i <= end_index; i++){
          if(isPrime(i)){
             subarray_count++;
          }
    }

    // Reduce all local counts to get the total count at root
    int total_count;
    MPI_Reduce(&subarray_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double end_time = MPI_Wtime();

        printf("Master process announces the prime numbers within given input range is %d.\n", total_count);
        printf("Program 1 using [MPI_Bcast - MPI_Reduce] Execution Time: %f seconds\n", end_time - start_time);
        printf("Thanks for using our program\n");
    }
    //=========================================
    MPI_Finalize();
    return 0;
}