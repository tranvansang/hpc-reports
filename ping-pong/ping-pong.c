#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mpi.h>

#define MAX_SIZE   (1 << 25)
#define NUM_ROUNDS 1000

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
	int nproc, my_rank, partner_rank;
	char buf[MAX_SIZE];
	double     t_start, t_stop;
	MPI_Status status;

	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	if (nproc != 2) {
		if (my_rank == 0)
			printf("There are %d processes while this benchmark requires exactly 2\n", nproc);
		MPI_Finalize();
		return 0;
	}
	partner_rank = 1 - my_rank;

	if (my_rank == 0)
		printf("MPI ping-pong test, %d rounds, max size %d bytes\n", NUM_ROUNDS, MAX_SIZE);

	int len, round;
	MPI_Barrier(MPI_COMM_WORLD);
	for(len = 1; len <= MAX_SIZE; len <<= 1){
		MPI_Barrier(MPI_COMM_WORLD);
		t_start = MPI_Wtime();
		for (round = 0; round < NUM_ROUNDS*2; round++) {
			if (round %2 == my_rank)
				MPI_Recv(buf, len, MPI_BYTE, partner_rank, 123, MPI_COMM_WORLD, &status);
			else
				MPI_Send(buf, len, MPI_BYTE, partner_rank, 123, MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		t_stop = MPI_Wtime();

		if (my_rank == 0)
			printf("%8d bytes \t %12.8f us\n", len, (t_stop-t_start)/NUM_ROUNDS*1.0e6);
	}

	MPI_Finalize();

	return 0;
}

/*result
MPI ping-pong test, 1000 rounds, max size 33554432 bytes
       1 bytes 	   2.11215019 us
       2 bytes 	   2.08210945 us
       4 bytes 	   2.08210945 us
       8 bytes 	   2.08497047 us
      16 bytes 	   2.08997726 us
      32 bytes 	   2.04396248 us
      64 bytes 	   2.20012665 us
     128 bytes 	   2.48098373 us
     256 bytes 	   2.42114067 us
     512 bytes 	   2.51603127 us
    1024 bytes 	   2.92015076 us
    2048 bytes 	   3.49497795 us
    4096 bytes 	   4.30393219 us
    8192 bytes 	   6.05392456 us
   16384 bytes 	   9.76681709 us
   32768 bytes 	  17.61603355 us
   65536 bytes 	  48.08115959 us
  131072 bytes 	  90.03281593 us
  262144 bytes 	 171.07820511 us
  524288 bytes 	 332.34286308 us
 1048576 bytes 	 654.08205986 us
 2097152 bytes 	 1297.05309868 us
 4194304 bytes 	 2589.25604820 us
 8388608 bytes 	 5189.98908997 us
16777216 bytes 	 10421.90098763 us
33554432 bytes 	 20220.81708908 us
*/
