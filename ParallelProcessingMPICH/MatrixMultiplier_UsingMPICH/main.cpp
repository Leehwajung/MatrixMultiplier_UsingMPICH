// main.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include "Matrix.cpp"

#define HOST		(Rank)0		// Host Process (Rank 0)
#define MAX_DATA	100

#define WIDTH	3

typedef int	Rank;				// Process Number

using namespace std;
using namespace MatrixSpace;

enum Message
{
	total,
	submul
};

void main(int argc, char **argv)
{
	int size;
	Rank rank;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;

	//char buff[MAX_DATA];

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	/* ������� ��� ���α׷��� �����ϰ� ����ȴ�.
	 * rank �� �� ���μ����� �����ϱ� ���� ���ȴ�.
	 * �ַ� rank 0 �� Ư���� �뵵�� ���ȴ�.
	 */

	cout << "### Process " << rank << " from " << processor_name << " ###" << endl;
	fflush(stdout);

	Matrix<float> A(WIDTH, WIDTH);
	Matrix<float> B(WIDTH, WIDTH);
	Matrix<float> C(WIDTH, WIDTH);

	// ���� �� �ʱ�ȭ
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			A[i][j] = (float)i;
			B[i][j] = (float)j;
		}
	}

	int divSize = A.getCapacity() / size;
	int pos = rank * divSize;
	int posY = pos / WIDTH;
	int nextRankPos = (rank + 1) * divSize;
	int nextRankPosY = nextRankPos / WIDTH;
	int x = pos - posY * WIDTH;

	for (int y = posY, p = pos; y < nextRankPosY; y++) {
		for (p, x; p < nextRankPos && x < C.getWidth(); p++, x++) {
			// Csub is used to store the element
			float Csub = 0;

			// Loop over all the sub-matrices of A and B
			for (int i = 0; i < WIDTH; i++) {
				Csub += A[y][i] * B[i][x];
			}
			C[y][x] = Csub;
		}
		x = 0;
	}

	/* �� �κк��� ���μ����� rank �� ���� ������ �ڵ尡 �޶�����. */
	if (rank == HOST) {	// rank �� 0 ���� ���μ����� ������ �ڵ�
		for (Rank i = 1; i < size; i++) {
			for (int p = rank * divSize; p < (rank + 1) * divSize; p++) {
				float value;
				MPI_Recv(&value, sizeof(float), MPI_FLOAT, i, p, MPI_COMM_WORLD, &status);
				C.getData()[p] = value;
			}
		}

		cout << endl;
		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < WIDTH; j++) {
				cout << C[i][j] << " ";
			}
			cout << endl;
		}
	}
	else {				// rank �� 0 �� �ƴ� ���μ����� ������ �ڵ�
		for (int p = rank * divSize; p < (rank + 1) * divSize; p++) {
			MPI_Send(&C.getData()[p], sizeof(float), MPI_FLOAT, HOST, p, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
}
