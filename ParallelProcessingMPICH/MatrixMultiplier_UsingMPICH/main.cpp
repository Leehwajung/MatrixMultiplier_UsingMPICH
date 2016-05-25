// main.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include "MatrixMul.cpp"

#define WIDTH	4096		// ��� ���� �� ���� �ʺ�

using namespace std;
using namespace MatrixSpace;


void main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	Matrix<float> A(WIDTH, WIDTH);
	Matrix<float> B(WIDTH, WIDTH);
	Matrix<float> C(WIDTH, WIDTH);

	// ���� ������ ��� �ʱ�ȭ
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			A[i][j] = (float)i;
			B[i][j] = (float)j;
		}
	}

	matrixMultiplyUsingMPICH(C, A, B);

	// ��� ��� ���
	cout << endl;
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			cout << C[i][j] << " ";
		}
		cout << endl;
	}

	MPI_Finalize();
}
