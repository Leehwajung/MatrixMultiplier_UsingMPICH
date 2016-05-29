// main.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include "Matrix.cpp"

#define WIDTH	10	// ��� ���� �� ���� �ʺ�

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

	// ��� �ʱ�ȭ
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			A[i][j] = (float)rand();	// ����� ���ǰ� �ʱ�ȭ
			B[i][j] = (float)rand();	// ����� ���ǰ� �ʱ�ȭ
			C[i][j] = 0;				// ��� ����� �ʱ� ���� 0
		}
	}

	// ��� ����
	cout << "Computing result using MPICH ..." << endl;
	
	cout << "Processing on Process " << rank << " from " << processor_name << endl;
	double wallClockTime = matrixMultiplyUsingMPICH_Host(C, A, B);
	cout << "Completed on process " << rank << " from " << processor_name << endl;

	cout << "Wall clock time: " << wallClockTime << endl;

	// ��� ��� ���
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

	/* ������� ��� ���α׷��� �����ϰ� ����ȴ�.
	 * rank �� �� ���μ����� �����ϱ� ���� ���ȴ�.
	 * �ַ� rank 0 �� Ư���� �뵵�� ���ȴ�.
	 */

	/* �� �κк��� ���μ����� rank �� ���� ������ �ڵ尡 �޶�����. */
	if (rank == HOST) {	// rank �� 0 ���� ���μ����� ������ �ڵ�
		host_main(argc, argv);
	}
	else {				// rank �� 0 �� �ƴ� ���μ����� ������ �ڵ�
		satellite_main(argc, argv, rank);
	}

	MPI_Finalize();
}
