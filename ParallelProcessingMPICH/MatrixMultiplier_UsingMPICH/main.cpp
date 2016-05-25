// main.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "MatrixMul.cpp"

#define WIDTH	4096		// 행렬 가로 및 세로 너비

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
		}
	}

	matrixMultiplyUsingMPICH(C, A, B);

	// 결과 행렬 출력
	cout << endl;
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			cout << C[i][j] << " ";
		}
		cout << endl;
	}

	MPI_Finalize();
}
