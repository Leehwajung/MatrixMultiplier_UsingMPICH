// main.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "Matrix.cpp"

#define WIDTH	10	// 행렬 가로 및 세로 너비

using namespace std;
using namespace MatrixSpace;


void host_main(int argc, char **argv, const Rank rank = HOST)
{
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;

	MPI_Get_processor_name(processor_name, &namelen);

	Matrix<float> A(WIDTH, WIDTH);
	Matrix<float> B(WIDTH, WIDTH);
	Matrix<float> C(WIDTH, WIDTH);
	
	srand((unsigned int)time(NULL));

	// 행렬 초기화
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			A[i][j] = (float)rand();	// 앞행렬 임의값 초기화
			B[i][j] = (float)rand();	// 뒷행렬 임의값 초기화
			C[i][j] = 0;				// 결과 행렬의 초기 값은 0
		}
	}

	// 행렬 곱셈
	cout << "Computing result using MPICH ..." << endl;
	
	cout << "Processing on Process " << rank << " from " << processor_name << endl;
	double wallClockTime = matrixMultiplyUsingMPICH_Host(C, A, B);
	cout << "Completed on process " << rank << " from " << processor_name << endl;

	cout << "Wall clock time: " << wallClockTime << endl;

	// 결과 행렬 출력
	cout << "Result: " << endl;
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			cout << C[i][j] << " ";
		}
		cout << endl;
	}
}

void satellite_main(int argc, char **argv, const Rank rank)
{
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;

	MPI_Get_processor_name(processor_name, &namelen);

	cout << "Computing on Process " << rank << " from " << processor_name << endl;
	matrixMultiplyUsingMPICH_Satellite();
	cout << "Completed on process " << rank << " from " << processor_name << endl;
}

void main(int argc, char **argv)
{
	Rank rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* 여기까지 모든 프로그램이 동일하게 수행된다.
	 * rank 는 각 프로세스를 구별하기 위해 사용된다.
	 * 주로 rank 0 은 특별한 용도로 사용된다.
	 */

	/* 이 부분부터 프로세스의 rank 에 따라 수행할 코드가 달라진다. */
	if (rank == HOST) {	// rank 가 0 번인 프로세스가 수행할 코드
		host_main(argc, argv);
	}
	else {				// rank 가 0 이 아닌 프로세스가 수행할 코드
		satellite_main(argc, argv, rank);
	}

	MPI_Finalize();
}
