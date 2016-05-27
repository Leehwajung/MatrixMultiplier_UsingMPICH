// main.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "MatrixMulGeneric.cpp"

#define WIDTH	10		// 행렬 가로 및 세로 너비

using namespace std;
using namespace MatrixSpace;


void main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	Matrix<float> A(WIDTH, WIDTH);
	Matrix<float> B(WIDTH, WIDTH);
	Matrix<float> C(WIDTH, WIDTH);

	// 임의 값으로 행렬 초기화
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			A[i][j] = (float)i;
			B[i][j] = (float)j;
			C[i][j] = 0;
		}
	}

	Rank rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == HOST) {
		matrixMultiplyOnHost(C, A, B);
	}
	else {
		<float>matrixMultiplyOnClient(rank);
	}

	

	//// 결과 행렬 출력
	//cout << "st--------------"<<endl;
	//for (int i = 0; i < WIDTH; i++) {
	//	for (int j = 0; j < WIDTH; j++) {
	//		cout << C[i][j] << " ";
	//	}
	//	cout << endl;
	//}
	//cout << "ed--------------" << endl;

	MPI_Finalize();
}
