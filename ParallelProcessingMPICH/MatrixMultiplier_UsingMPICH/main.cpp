// main.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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

	/* ������� ��� ���α׷��� �����ϰ� ����ȴ�.
	* rank �� �� ���μ����� �����ϱ� ���� ���ȴ�.
	* �ַ� rank 0 �� Ư���� �뵵�� ���ȴ�.
	*/

	/* �� �κк��� ���μ����� rank �� ���� ������ �ڵ尡 �޶�����. */
	if (rank == HOST) {	// rank �� 0 ���� ���μ����� ������ �ڵ�
		printf_s("### Process %d from %s ###\n", rank, processor_name);
		fflush(stdout);
		for (Rank i = 1; i < size; i++) {
			MPI_Recv(buff, MAX_DATA, MPI_CHAR, i, multiply, MPI_COMM_WORLD, &status);
			printf_s("%s\n", buff);
			fflush(stdout);
		}
	}
	else {				// rank �� 0 �� �ƴ� ���μ����� ������ �ڵ�
		sprintf_s(buff, MAX_DATA, "### Process %d from %s ###", rank, processor_name);
		fflush(stdout);
		MPI_Send(buff, MAX_DATA, MPI_CHAR, HOST, multiply, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
