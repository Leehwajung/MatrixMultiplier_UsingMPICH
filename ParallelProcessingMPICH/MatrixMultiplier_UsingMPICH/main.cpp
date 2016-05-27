// main.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include "MatrixMulGeneric.cpp"

#define WIDTH	10		// ��� ���� �� ���� �ʺ�

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

	

	//// ��� ��� ���
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
