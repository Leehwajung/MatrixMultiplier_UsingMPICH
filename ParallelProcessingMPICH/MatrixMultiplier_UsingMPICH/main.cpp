// main.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#define HOST		(Rank)0		// Host Process (Rank 0)
#define MAX_DATA	100

typedef int	Rank;				// Process Number

enum Message
{
	multiply
};

void main(int argc, char **argv)
{
	int size;
	Rank rank;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;

	char buff[MAX_DATA];

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	/* 여기까지 모든 프로그램이 동일하게 수행된다.
	* rank 는 각 프로세스를 구별하기 위해 사용된다.
	* 주로 rank 0 은 특별한 용도로 사용된다.
	*/

	/* 이 부분부터 프로세스의 rank 에 따라 수행할 코드가 달라진다. */
	if (rank == HOST) {	// rank 가 0 번인 프로세스가 수행할 코드
		printf_s("### Process %d from %s ###\n", rank, processor_name);
		fflush(stdout);
		for (Rank i = 1; i < size; i++) {
			MPI_Recv(buff, MAX_DATA, MPI_CHAR, i, multiply, MPI_COMM_WORLD, &status);
			printf_s("%s\n", buff);
			fflush(stdout);
		}
	}
	else {				// rank 가 0 이 아닌 프로세스가 수행할 코드
		sprintf_s(buff, MAX_DATA, "### Process %d from %s ###", rank, processor_name);
		fflush(stdout);
		MPI_Send(buff, MAX_DATA, MPI_CHAR, HOST, multiply, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
