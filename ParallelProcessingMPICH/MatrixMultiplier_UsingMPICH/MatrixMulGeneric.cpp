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

		Rank i;

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
		if (rank == HOST) {		//ȣ��Ʈ( Ÿ�̸� �۵� �� ������ ����)
			startwtime = MPI_Wtime();
			for (i = 1; i < size; i++) {		//Ŭ���̾�Ʈ�� �迭����
				int currPos = getStartPosition(capacity, i, size);	// �޽����� ������ ��Ļ� ���� ��ġ
				int blockSize = getBlockSize(capacity, i, size);	// �޽����� ���� ������ ��� ũ��
				MPI_Send(&i, 1, MPI_INT, i, submul - 1, MPI_COMM_WORLD);
				MPI_Send(&MatrixA.getData()[currPos], getBlockSize(capacity, rank, size), MPI_FLOAT, i, submul - 1, MPI_COMM_WORLD);
				MPI_Send(&MatrixB.getData()[currPos], getBlockSize(capacity, rank, size), MPI_FLOAT, i, submul - 1, MPI_COMM_WORLD);
			}	//end for
		}	//end if

		else{	//Ŭ���̾�Ʈ(������ ����)
			MPI_Recv(&i, getBlockSize(capacity, rank, size), MPI_INT, HOST, submul - 1, MPI_COMM_WORLD, &status);	//ȣ��Ʈ������ i�� �������� ���� ���
			int currPos = getStartPosition(capacity, i, size);	// �޽����� ������ ��Ļ� ���� ��ġ
			int blockSize = getBlockSize(capacity, i, size);	// �޽����� ���� ������ ��� ũ��
			MPI_Recv(&MatrixA.getData()[currPos], getBlockSize(capacity, rank, size), MPI_FLOAT, HOST, submul - 1, MPI_COMM_WORLD, &status);
			MPI_Recv(&MatrixB.getData()[currPos], getBlockSize(capacity, rank, size), MPI_FLOAT, HOST, submul - 1, MPI_COMM_WORLD, &status);
		}	//end else

		// ��� ����
		for (unsigned int y = startPosY, x = startPos - startPosY * width, p = startPos;
			y <= endPosY; y++) {								// ���� ���� ����
			for (x, p; x < width && p <= endPos; x++, p++) {	// ���ι��� ����
				// Csub is used to store the element
				float Csub = 0;

				// Loop over all the sub-matrices of A and B
				for (unsigned int i = 0; i < width; i++) {
					Csub += MatrixA[y][i] * MatrixB[i][x];
				}	//end for
				MatrixC[y][x] = Csub;
			}	//end for
			x = 0;
		}	//end for

		cout << "Completed on process " << rank << " from " << processor_name << endl;

		/* �� �κк��� ���μ����� rank �� ���� ������ �ڵ尡 �޶�����. */
		// Host�� ����� ����
		if (rank == HOST) {	// rank �� 0 ���� ���μ����� ������ �ڵ� (Ŭ���̾�Ʈ���� ������ �ް� ��� �� �ð� ���� ����)
			for (i = 1; i < size; i++) {
				int currPos = getStartPosition(capacity, i, size);	// �޽����� ������ ��Ļ� ���� ��ġ
				int blockSize = getBlockSize(capacity, i, size);	// �޽����� ���� ������ ��� ũ��
				MPI_Recv(&MatrixC.getData()[currPos], blockSize, MPI_FLOAT, i, submul, MPI_COMM_WORLD, &status);
			}		//end for

			// �ð� ���� ����
			endwtime = MPI_Wtime();
			cout << "Wall clock time: " << endwtime - startwtime << endl;

			// ��� ��� ���
			cout << endl;
			for (unsigned int i = 0; i < MatrixC.getHeight(); i++) {
				for (unsigned int j = 0; j < MatrixC.getWidth(); j++) {
					cout << MatrixC[i][j] << " ";
				}	//end for
				cout << endl;
			}	//end for
			cout << endl;
		}//end if

		else {				// rank �� 0 �� �ƴ� ���μ����� ������ �ڵ� (ȣ��Ʈ�� ��� ��� ����)
			MPI_Send(&MatrixC.getData()[startPos], getBlockSize(capacity, rank, size), MPI_FLOAT, HOST, submul, MPI_COMM_WORLD);
		}		//end else
	}
}
