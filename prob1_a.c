include <mpi.h>
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

        range = (y - x) + 1;
        elements_per_process = range / (size - 1);
    }

    // Broadcast the essential variables to all processes
    int bcast_data[4] = {range, elements_per_process, size, x};
    MPI_Bcast(bcast_data, 4, MPI_INT, 0, MPI_COMM_WORLD);
    range = bcast_data[0];
    elements_per_process = bcast_data[1];
    size = bcast_data[2]; // In case some MPI implementations modify size
    x = bcast_data[3];
    //================================================
    int start_index=0 ,end_index=0;
    if(rank != 0)
    {
       start_index = x +((rank-1) * elements_per_process);
       end_index;
       if(rank == size - 1){
            end_index = start_index + (elements_per_process + ( range % (size - 1) ));}
      else{
          end_index = start_index + elements_per_process;
          }
    }

    int subarray_count=0;
    for(int i = start_index; i < end_index; i++){
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
