/**************************************************************************
 * 
 * This is a simple ping pong program to test the communication between two 
 * processes using blocking send and receive mpi functions
 * 
 ***************************************************************************/
#include <stdio.h>
#include <mpi.h>

#define MAX_ROUND_TRIP 1

int main(int argc, char*argv[])
{
  int my_rank, size;
  int send_buffer = 1;
  int recv_buffer;
  MPI_Status status; 
  // time counts
  double start,end;

  // init MPI 
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);	

  MPI_Barrier(MPI_COMM_WORLD);

  start = MPI_Wtime();

  for(int i=0; i<MAX_ROUND_TRIP;i++){
    if(my_rank==0){
      MPI_Send(&send_buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
      MPI_Recv(&recv_buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
    }else {
      MPI_Recv(&recv_buffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Send(&send_buffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
  }

  end = MPI_Wtime();
  
  printf("latency of %d : %f \n",my_rank,(end-start)/2);
  MPI_Finalize();
  return 0;
}