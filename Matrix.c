#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include "mpi.h"



int main(int argc, char *argv[]) {
    
    setvbuf(stdout,NULL,_IONBF,0);

    int my_rank; /* rank of process */

    int p; /* number of processes */

    // N --.> rows , M --> cols

    int N, M;

    int *A = NULL;

    int *B = NULL;

    int *C = NULL;

    int *SlaveA, *SlaveB, *SlaveC;



    /* Start up MPI */

    MPI_Init(&argc, &argv);

    /* Find out process rank */

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out number of processes */

    MPI_Comm_size(MPI_COMM_WORLD, &p);



    /* Master Process */

    if (my_rank == 0) {

        printf("Hello, Shahd Elnassag ^_^ \n");



        // Get matrix dimensions from the user

        printf("Enter number of rows: ");

        scanf("%d", &N);

        printf("Enter number of columns: ");

        scanf("%d", &M);



        // Allocate memory 

        int totalSize = N * M;

        A = malloc(totalSize * sizeof(int));

        B = malloc(totalSize * sizeof(int));

        C = malloc(totalSize * sizeof(int));



        // Input matrix A elements

        printf("Enter elements of the matrix A: (%d x %d):\n", N, M);

        for (int i = 0; i < totalSize; i++) {

            printf("Element [%d][%d]: ", i / N, i % M);

            scanf("%d", &A[i]);

        }



        // Input matrix B elements

        printf("Enter elements of the matrix B: (%d x %d):\n", N, M);

        for (int i = 0; i < totalSize; i++) {

            printf("Element [%d][%d]: ", i / N, i % M);

            scanf("%d", &B[i]);

        }



        

    }



    // Calculate chunk size (split rows evenly across processes)

    int rowsPerProcess = N / p; // Rows per process

    int remainderRows = N % p;  // Remainder rows to be handled

    int chunkSize = rowsPerProcess * M;



    // Allocate memory for slave chunks (only rowsPerProcess rows)

    SlaveA = malloc(chunkSize * sizeof(int));

    SlaveB = malloc(chunkSize * sizeof(int));

    SlaveC = malloc(chunkSize * sizeof(int));



    // Scatter the rows of matrix A and B to all processes

    MPI_Scatter(A, chunkSize, MPI_INT, SlaveA, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Scatter(B, chunkSize, MPI_INT, SlaveB, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);





    // Local sum of the chunks

    for (int i = 0; i < chunkSize; i++) {

        SlaveC[i] = SlaveA[i] + SlaveB[i];

    }



    // Gather the computed chunks from all processes

    MPI_Gather(SlaveC, chunkSize, MPI_INT, C, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);



    // Master process reconstructs and prints the final result

    if (my_rank == 0) {

        

       // Handle any remaining rows if rows is not perfectly divisible by p

        int startIndx = rowsPerProcess * p;

        for (int i = 0; i < remainderRows; i++) {

            for (int j = 0; j < M; j++) {

                C[(startIndx + i) * M + j] = A[(startIndx + i) * M + j] + B[(startIndx + i) * M + j];

            }

        }



        // Print the final result matrix C

        printf("Resultant matrix C :\n");

        for (int i = 0; i < N; i++) {

            for (int j = 0; j < M; j++) {

                printf("%d ", C[i * M + j]);

            }

            printf("\n");

        }



        printf("Thanks for using our program\n");



        // Free the allocated memory

        free(A);

        free(B);

        free(C);

    }



    // Free memory for slave chunks

    free(SlaveA);

    free(SlaveB);

    free(SlaveC);



    /* Shutdown MPI */

    MPI_Finalize();



    return 0;

}
