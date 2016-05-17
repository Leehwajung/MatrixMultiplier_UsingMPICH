// test.c : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#define MAX_DATA 100

int main(int argc, char **argv)
{
	int rank;
	int size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;
	char buff[MAX_DATA];
	int i;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	if (rank == 0) {
		printf_s("### Process %d from %s ###\n", rank, processor_name);
		fflush(stdout);
		for (i = 1; i < size; i++) {
			MPI_Recv(buff, MAX_DATA, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
			printf_s("%s\n", buff); fflush(stdout);
		}
	}
	else {
		sprintf_s(buff, MAX_DATA, "### Process %d from %s ###", rank, processor_name);
		fflush(stdout);
		MPI_Send(buff, MAX_DATA, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}

