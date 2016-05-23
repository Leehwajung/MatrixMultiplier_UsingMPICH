// main.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include "Matrix.cpp"

#define HOST	(Rank)0		// Host Process (Rank 0)
#define WIDTH	4096		// ��� ���� �� ���� �ʺ�

typedef int	Rank;			// Process Number

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

	/* ������� ��� ���α׷��� �����ϰ� ����ȴ�.
	 * rank �� �� ���μ����� �����ϱ� ���� ���ȴ�.
	 * �ַ� rank 0 �� Ư���� �뵵�� ���ȴ�.
	 */

	cout << "### Process " << rank << " from " << processor_name << " ###" << endl;
	fflush(stdout);

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

	// ���� ���� ���� ���� �ʱ�ȭ
	int divSize = (int)(A.getCapacity() / size + 0.5);		// ���� ��� ũ��
	const unsigned int pos = rank * divSize;				// ��� ���� ��ġ
	const unsigned int posY = pos / WIDTH;					// ��� ���� ���� ��ġ
	const unsigned int nextRankPos = (rank + 1) * divSize;	// ���� ��� ���� ��ġ
	const unsigned int nextRankPosY = nextRankPos / WIDTH;	// ���� ��� ���� ���� ��ġ
	divSize = nextRankPos > C.getCapacity() ? C.getCapacity() - pos : divSize;	// ��� ũ��

	// �ð� ���� ����
	double startwtime = 0.0, endwtime;
	if (rank == HOST) {
		startwtime = MPI_Wtime();
	}

	// ��� ����
	for (unsigned int y = posY, x = pos - posY * WIDTH, p = pos; 
		y < nextRankPosY; y++) {									// ���� ���� ����
		for (x, p; x < C.getWidth() && p < nextRankPos; x++, p++) {	// ���ι��� ����
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
	// Host�� ����� ����
	if (rank == HOST) {	// rank �� 0 ���� ���μ����� ������ �ڵ�
		for (Rank i = 1; i < size; i++) {
			int currPos = rank * divSize;	// �޽����� ������ ��Ļ� ���� ��ġ
			int blockSize = (rank < size - 1) ? divSize : C.getCapacity() - currPos;		// �޽����� ���� ������ ��� ũ��
			MPI_Recv(&C.getData()[currPos], sizeof(float) * blockSize, MPI_FLOAT, i, submul, MPI_COMM_WORLD, &status);
		}

		// ��� ��� ���
		//cout << endl;
		//for (int i = 0; i < WIDTH; i++) {
		//	for (int j = 0; j < WIDTH; j++) {
		//		cout << C[i][j] << " ";
		//	}
		//	cout << endl;
		//}

		// �ð� ���� ����
		endwtime = MPI_Wtime();
		cout << "wall clock time: " << endwtime - startwtime << endl;
	}
	else {				// rank �� 0 �� �ƴ� ���μ����� ������ �ڵ�
		MPI_Send(&C.getData()[pos], sizeof(float) * divSize, MPI_FLOAT, HOST, submul, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
