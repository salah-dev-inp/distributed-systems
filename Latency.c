/**************************************************************************
* 
* This program performs some calculations to measure latency of communication 
* between two processes
* 
***************************************************************************/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_ROUND_TRIP 30
#define SIZE_MSG 10000
#define BLOCKING
#define TAG 0

#define MPI_CHECK(stmt)                                           \
do {                                                              \
    int mpi_errno = (stmt);                                       \
    if (MPI_SUCCESS != mpi_errno) {                               \
            fprintf(stderr, "[%s:%d] MPI call failed with %d \n", \
                __FILE__, __LINE__,mpi_errno);                    \
            exit(EXIT_FAILURE);                                   \
    }                                                             \
    assert(MPI_SUCCESS == mpi_errno);                             \
} while(0)


int main(int argc, char* argv[])
{
    int my_rank, num_procs;
    char recv_buffer[10000],send_buffer[10000];
    FILE *stream1 = fopen("data1.txt" , "w");
    FILE *stream2 = fopen("data2.txt" , "w");
    MPI_Request req_send, req_recv;
    MPI_Status status;
    // time counters
    double t_start=0.0, t_end=0.0;

    // initialize MPI 
    MPI_CHECK(MPI_Init(&argc,&argv));
    MPI_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &num_procs));
    MPI_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &my_rank));	
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

    for(int i=0; i<NUM_ROUND_TRIP; i++)
    {

#ifdef BLOCKING
    t_start = MPI_Wtime();
    MPI_CHECK(MPI_Isend(send_buffer, SIZE_MSG, MPI_BYTE, (my_rank+1)%2, TAG, MPI_COMM_WORLD,&req_send));
    MPI_CHECK(MPI_Irecv(recv_buffer, SIZE_MSG, MPI_BYTE, (my_rank+1)%2, TAG, MPI_COMM_WORLD,&req_recv));
    MPI_CHECK(MPI_Wait(&req_send,&status));
    MPI_CHECK(MPI_Wait(&req_recv,&status));
    t_end = MPI_Wtime();
    if(!my_rank) fprintf(stream1,"%d %f \n",i,(t_end-t_start)/2);

#else
    t_start = MPI_Wtime();
    if(!my_rank)
    {
        MPI_CHECK(MPI_Send(send_buffer, SIZE_MSG, MPI_BYTE, 1, TAG, MPI_COMM_WORLD));
        MPI_CHECK(MPI_Recv(recv_buffer, SIZE_MSG, MPI_BYTE, 1, TAG, MPI_COMM_WORLD, &status));
    }else 
    {
        MPI_CHECK(MPI_Recv(recv_buffer, SIZE_MSG, MPI_BYTE, 0, TAG, MPI_COMM_WORLD, &status));
        MPI_CHECK(MPI_Send(send_buffer, SIZE_MSG, MPI_BYTE, 0, TAG, MPI_COMM_WORLD));
    }
    t_end = MPI_Wtime();
    if(!my_rank) fprintf(stream2,"%d %f\n",i,(t_end-t_start)/2);

#endif
    }
    fclose(stream1);
    fclose(stream2);
    MPI_CHECK(MPI_Finalize());
    return 0;
}