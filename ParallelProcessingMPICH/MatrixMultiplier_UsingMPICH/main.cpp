// main.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "Matrix.cpp"

#define HOST		(Rank)0		// Host Process (Rank 0)
#define MAX_DATA	100

#define WIDTH	1024

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

	// 임의 값으로 행렬 초기화
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			A[i][j] = (float)i;
			B[i][j] = (float)j;
		}
	}

	int divSize = (int)(A.getCapacity() / size + 0.5);
	int pos = rank * divSize;
	int posY = pos / WIDTH;
	int nextRankPos = (rank + 1) * divSize;
	int nextRankPosY = nextRankPos / WIDTH;
	unsigned int x = pos - posY * WIDTH;

	// 시간 측정 시작
	clock_t start = clock();

	// 행렬 곱셈
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
	// Host로 결과값 전달
	if (rank == HOST) {	// rank 가 0 번인 프로세스가 수행할 코드
		for (Rank i = 1; i < size; i++) {
			int currPos = rank * divSize;	// 메시지를 저장할 행렬상 시작 위치
			int blockSize = (rank < size - 1) ? divSize : C.getCapacity() - currPos;		// 메시지를 받을 데이터 블록 크기
			MPI_Recv(&C.getData()[currPos], sizeof(float) * blockSize, MPI_FLOAT, i, submul, MPI_COMM_WORLD, &status);
		}

		// 결과 행렬 출력
		//cout << endl;
		//for (int i = 0; i < WIDTH; i++) {
		//	for (int j = 0; j < WIDTH; j++) {
		//		cout << C[i][j] << " ";
		//	}
		//	cout << endl;
		//}
	}
	else {				// rank 가 0 이 아닌 프로세스가 수행할 코드
		int blockSize = (rank < size - 1) ? divSize : C.getCapacity() - pos;		// 메시지를 보낼 데이터 블록 크기
		MPI_Send(&C.getData()[pos], sizeof(float) * blockSize, MPI_FLOAT, HOST, submul, MPI_COMM_WORLD);
	}

	// 시간 측정 완료
	clock_t finish = clock();
	cout << "Total time: " << (double)(finish - start) << endl;

	MPI_Finalize();
}
