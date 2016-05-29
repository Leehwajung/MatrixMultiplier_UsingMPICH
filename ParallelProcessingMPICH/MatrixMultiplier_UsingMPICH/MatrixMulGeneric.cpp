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
		const unsigned int capacity = MatrixA.getCapacity();					// ��� �� ��ü ����
		const unsigned int width = MatrixA.getWidth();							// ��� ���� �ʺ�
		const unsigned int startPos = getStartPosition(capacity, rank, size);	// ��� ���� ��ġ
		const unsigned int startPosY = startPos / width;						// ��� ���� ���� ��ġ
		const unsigned int endPos = getEndPosition(capacity, rank, size);		// ��� �� ��ġ
		const unsigned int endPosY = endPos / width;							// ��� �� ���� ��ġ

		cout << "Computing on Process " << rank << " from " << processor_name << endl;

		// �ð� ���� ����
		double startwtime = 0.0, endwtime;
		if (rank == HOST) {
			startwtime = MPI_Wtime();
		}

		// ��� ����
		for (unsigned int y = startPosY, x = startPos - startPosY * width, p = startPos;
			y <= endPosY; y++) {								// ���� ���� ����
			for (x, p; x < width && p <= endPos; x++, p++) {	// ���ι��� ����
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
				int currPos = getStartPosition(capacity, i, size);	// �޽����� ������ ��Ļ� ���� ��ġ
				int blockSize = getBlockSize(capacity, i, size);	// �޽����� ���� ������ ��� ũ��
				MPI_Recv(&MatrixC.getData()[currPos], blockSize, MPI_FLOAT, i, submul, MPI_COMM_WORLD, &status);
			}

			// �ð� ���� ����
			endwtime = MPI_Wtime();
			cout << "Wall clock time: " << endwtime - startwtime << endl;

			// ��� ��� ���
			cout << endl;
			for (unsigned int i = 0; i < MatrixC.getHeight(); i++) {
				for (unsigned int j = 0; j < MatrixC.getWidth(); j++) {
					cout << MatrixC[i][j] << " ";
				}
				cout << endl;
			}
			cout << endl;
		}
		else {				// rank �� 0 �� �ƴ� ���μ����� ������ �ڵ�
			MPI_Send(&MatrixC.getData()[startPos], getBlockSize(capacity, rank, size), MPI_FLOAT, HOST, submul, MPI_COMM_WORLD);
		}
	}
}
