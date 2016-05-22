// main.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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

	/* 여기까지 모든 프로그램이 동일하게 수행된다.
	 * rank 는 각 프로세스를 구별하기 위해 사용된다.
	 * 주로 rank 0 은 특별한 용도로 사용된다.
	 */

	cout << "### Process " << rank << " from " << processor_name << " ###" << endl;
	fflush(stdout);

	Matrix<float> A(WIDTH, WIDTH);
	Matrix<float> B(WIDTH, WIDTH);
	Matrix<float> C(WIDTH, WIDTH);

	// 임의 값 초기화
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

	/* 이 부분부터 프로세스의 rank 에 따라 수행할 코드가 달라진다. */
	if (rank == HOST) {	// rank 가 0 번인 프로세스가 수행할 코드
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
	else {				// rank 가 0 이 아닌 프로세스가 수행할 코드
		for (int p = rank * divSize; p < (rank + 1) * divSize; p++) {
			MPI_Send(&C.getData()[p], sizeof(float), MPI_FLOAT, HOST, p, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
}
