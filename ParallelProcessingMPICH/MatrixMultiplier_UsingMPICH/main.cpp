// main.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include "Matrix.cpp"

#define INITWIDTH	10	// ��� ���� �� ���� �ʺ� �⺻��

using namespace std;
using namespace MatrixSpace;


template<class NUM>
void printMatrix(Matrix<NUM>& matrix);


// rank �� 0 ���� ���μ����� ������ �ڵ�
void host_main(int argc, char *argv[], const Rank rank = HOST)
{
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;

	char option;
	int height = INITWIDTH;
	int correspondence = INITWIDTH;
	int width = INITWIDTH;
	bool isSquare = false;
	bool print[3] = { false, false, false };	// A, B, C
	bool debug = false;

	// ��� �μ�
	for (int i = 0; i < argc; i++) {
		if (*argv[i] == '-') {
			option = tolower(argv[i][1]);
			switch (option) {
			case 'd':	// debug
				debug = true;
				break;
			}
		}
		else {
			switch (option) {
			case 's':
				isSquare = true;
				height = correspondence = width = atoi(argv[i]);
				break;
			case 'p':
				switch (tolower(argv[i][0])) {
				case 'a':
				case '0':	// A ���
					print[0] = true;
					break;
				case 'b':
				case '1':	// B ���
					print[1] = true;
					break;
				case 'c':
				case '2':	// C ���
					print[2] = true;
					break;
				case 'w':
				case '3':	// A, B, C ��ü ���
				default:
					print[0] = print[1] = print[2] = true;
					break;
				}
				break;
			}
			if (!isSquare) {
				switch (option) {
				case 'h':	// height
					height = atoi(argv[i]);
					break;
				case 'c':	// correspondence
					correspondence = atoi(argv[i]);
					break;
				case 'w':	// width
					width = atoi(argv[i]);
					break;
				}
			}
		}
	}

	// ��� ����
	Matrix<float> A(height, correspondence);
	Matrix<float> B(correspondence, width);
	Matrix<float> C(height, width);
	
	// ��� �ʱ�ȭ
	if (isSquare && !debug) {	// ������ ��� �ʱ�ȭ
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				A[i][j] = (float)rand() / 100.0;	// ����� ���ǰ� �ʱ�ȭ
				B[i][j] = (float)rand() / 100.0;	// ����� ���ǰ� �ʱ�ȭ
			}
		}
	}
	else if (!debug) {			// �������� ��� �ʱ�ȭ
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < correspondence; j++) {
				A[i][j] = (float)rand() / 100.0;	// ����� ���ǰ� �ʱ�ȭ
			}
		}
		for (int i = 0; i < correspondence; i++) {
			for (int j = 0; j < width; j++) {
				B[i][j] = (float)rand() / 100.0;	// ����� ���ǰ� �ʱ�ȭ
			}
		}
	}
	else {						// -d �ɼ� (debug �ɼ�) �ʱ�ȭ
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < correspondence; j++) {
				A[i][j] = (float)i;	// ����� �ʱ�ȭ
			}
		}
		for (int i = 0; i < correspondence; i++) {
			for (int j = 0; j < width; j++) {
				B[i][j] = (float)j;	// ����� �ʱ�ȭ
			}
		}
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				C[i][j] = 0;		// ��� ����� �ʱ� ���� 0
			}
		}
	}

	// �Է� ��� ���
	if (print[0]) {
		cout << "Matrix A: " << endl;
		printMatrix(A);
	}
	if (print[1]) {
		cout << "Matrix B: " << endl;
		printMatrix(B);
	}

	// ��� ����
	MPI_Get_processor_name(processor_name, &namelen);
	cout << "Computing result using MPICH ..." << endl;
	
	cout << "Processing on Process " << rank << " from " << processor_name << endl;
	double wallClockTime = matrixMultiplyUsingMPICH_Host(C, A, B);
	cout << "Completed on process " << rank << " from " << processor_name << endl;

	cout << "Wall clock time: " << wallClockTime << endl;

	// ��� ��� ���
	if (print[2]) {
		cout << endl;
		cout << "Matrix C (Result): " << endl;
		printMatrix(C);
	}
}

// rank �� 0 �� �ƴ� ���μ����� ������ �ڵ�
void satellite_main(int argc, char *argv[], const Rank rank)
{
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;

	MPI_Get_processor_name(processor_name, &namelen);

	cout << "Computing on Process " << rank << " from " << processor_name << endl;
	matrixMultiplyUsingMPICH_Satellite();
	cout << "Completed on process " << rank << " from " << processor_name << endl;
}

// ���α׷� ����
void main(int argc, char *argv[])
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

// ��� ���
template<class NUM>
void printMatrix(Matrix<NUM>& matrix)
{
	for (int i = 0; i < matrix.getHeight(); i++) {
		for (int j = 0; j < matrix.getWidth(); j++) {
			cout << matrix[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}
