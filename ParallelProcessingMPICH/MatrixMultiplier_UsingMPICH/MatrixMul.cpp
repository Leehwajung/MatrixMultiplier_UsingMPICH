#include "stdafx.h"
#include "MatrixMul.h"

#include "Matrix.cpp"

namespace MatrixSpace
{
	template<class NUM>
	void matrixMultiplyUsingMPICH(Matrix<NUM>& MatrixC, const Matrix<NUM>& MatrixA, const Matrix<NUM>& MatrixB)
	{
		int size;
		Rank rank;
		char processor_name[MPI_MAX_PROCESSOR_NAME];
		int namelen;

		MPI_Status status;
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Get_processor_name(processor_name, &namelen);

		/* ������� ��� ���α׷��� �����ϰ� ����ȴ�.
		 * rank �� �� ���μ����� �����ϱ� ���� ���ȴ�.
		 * �ַ� rank 0 �� Ư���� �뵵�� ���ȴ�.
		 */

		if (rank == HOST) {
			cout << "Computing result using MPICH ..." << endl;
		}
		
		// ���� ���� ���� ���� �ʱ�ȭ
		int divSize = (int)(MatrixA.getCapacity() / size + 0.5);	// ���� ��� ũ��
		const unsigned int width = MatrixA.getWidth();				// ��� ���� �ʺ�
		const unsigned int pos = rank * divSize;					// ��� ���� ��ġ
		const unsigned int posY = pos / width;						// ��� ���� ���� ��ġ
		const unsigned int nextRankPos = (rank + 1) * divSize;		// ���� ��� ���� ��ġ
		const unsigned int nextRankPosY = nextRankPos / width;		// ���� ��� ���� ���� ��ġ
		divSize = nextRankPos > MatrixC.getCapacity() ? MatrixC.getCapacity() - pos : divSize;	// ��� ũ��

		cout << "Computing on Process " << rank << " from " << processor_name << endl;

		// �ð� ���� ����
		double startwtime = 0.0, endwtime;
		if (rank == HOST) {
			startwtime = MPI_Wtime();
		}

		// ��� ����
		for (unsigned int y = posY, x = pos - posY * width, p = pos;
			y < nextRankPosY; y++) {									// ���� ���� ����
			for (x, p; x < MatrixC.getWidth() && p < nextRankPos; x++, p++) {	// ���ι��� ����
																		// Csub is used to store the element
				float Csub = 0;

				// Loop over all the sub-matrices of A and B
				for (unsigned int i = 0; i < width; i++) {
					Csub += MatrixA[y][i] * MatrixB[i][x];
				}
				MatrixC[y][x] = Csub;
			}
			x = 0;
		}

		cout << "Completed on process " << rank << " from " << processor_name << endl;

		/* �� �κк��� ���μ����� rank �� ���� ������ �ڵ尡 �޶�����. */
		// Host�� ����� ����
		if (rank == HOST) {	// rank �� 0 ���� ���μ����� ������ �ڵ�
			for (Rank i = 1; i < size; i++) {
				int currPos = rank * divSize;	// �޽����� ������ ��Ļ� ���� ��ġ
				int blockSize = (rank < size - 1) ? divSize : MatrixC.getCapacity() - currPos;		// �޽����� ���� ������ ��� ũ��
				MPI_Recv(&MatrixC.getData()[currPos], sizeof(float) * blockSize, MPI_FLOAT, i, submul, MPI_COMM_WORLD, &status);
			}

			// �ð� ���� ����
			endwtime = MPI_Wtime();
			cout << "Wall clock time: " << endwtime - startwtime << endl;
		}
		else {				// rank �� 0 �� �ƴ� ���μ����� ������ �ڵ�
			MPI_Send(&MatrixC.getData()[pos], sizeof(float) * divSize, MPI_FLOAT, HOST, submul, MPI_COMM_WORLD);
		}
	}
}