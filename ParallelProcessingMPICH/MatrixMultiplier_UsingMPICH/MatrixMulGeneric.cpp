#include "stdafx.h"
#include "MatrixMul.h"

#include "Matrix.cpp"

namespace MatrixSpace
{
	// rank �� 0 ���� ���μ����� ������ �ڵ�
	template<class NUM, class NUMPTR = NUM*>
	void matrixMultiplyOnHost(Matrix<NUM>& MatrixC, const Matrix<NUM>& MatrixA, const Matrix<NUM>& MatrixB)
	{
		int size;
		char processor_name[MPI_MAX_PROCESSOR_NAME];
		int namelen;

		MPI_Status status;
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		
		MPI_Get_processor_name(processor_name, &namelen);

		cout << "Computing result using MPICH ..." << endl;

		// ���� ���� ���� ���� �ʱ�ȭ
		const unsigned int capacity = MatrixA.getCapacity();					// ����� �� ��ü ����
		const unsigned int width = MatrixA.getWidth();							// ����� ���� �ʺ�
		const unsigned int startPos = getStartPosition(capacity, HOST, size);	// ��� ���� ��ġ
		const unsigned int startPosY = startPos / width;						// ��� ���� ���� ��ġ
		const unsigned int endPos = getEndPosition(capacity, HOST, size);		// ��� �� ��ġ
		const unsigned int endPosY = endPos / width;							// ��� �� ���� ��ġ

		cout << "Computing on Process " << HOST << " from " << processor_name << endl;

		// �ð� ���� ����
		double startwtime = 0.0, endwtime;
		startwtime = MPI_Wtime();

		// Clients�� �ʱⰪ ����
		unsigned int heightA = MatrixA.getHeight();
		unsigned int widthA = MatrixA.getWidth();
		NUM* dataA = MatrixA.getData();
		unsigned int heightB = MatrixB.getHeight();
		unsigned int widthB = MatrixB.getWidth();
		NUM* dataB = MatrixB.getData();

		for (Rank i = 1; i < size; i++) {
			MPI_Send(&heightA, 1, MPI_INT, i, initHeightA, MPI_COMM_WORLD);
			MPI_Send(&widthA, 1, MPI_INT, i, initWidthA, MPI_COMM_WORLD);
			MPI_Send(dataB, capacity, MPI_FLOAT, i, initDataA, MPI_COMM_WORLD);
			MPI_Send(&heightB, 1, MPI_INT, i, initHeightB, MPI_COMM_WORLD);
			MPI_Send(&widthB, 1, MPI_INT, i, initWidthB, MPI_COMM_WORLD);
			MPI_Send(dataA, capacity, MPI_FLOAT, i, initDataB, MPI_COMM_WORLD);
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

		cout << "Completed on process " << HOST << " from " << processor_name << endl;

		// Host�� ����� ����
		for (Rank i = 1; i < size; i++) {
			int currPos = getStartPosition(capacity, i, size);	// �޽����� ������ ��Ļ� ���� ��ġ
			int blockSize = getBlockSize(capacity, i, size);	// �޽����� ���� ������ ��� ũ��
			MPI_Recv(&MatrixC.getData()[currPos], blockSize, MPI_FLOAT, i, submul, MPI_COMM_WORLD, &status);
		}

		// �ð� ���� ����
		endwtime = MPI_Wtime();
		cout << "Wall clock time: " << endwtime - startwtime << endl;
	}

	// rank �� 0 �� �ƴ� ���μ����� ������ �ڵ�
	template<class NUM>
	void matrixMultiplyOnClient(Rank rank)
	{
		int size;
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
		const unsigned int capacity = MatrixA.getCapacity();					// ����� �� ��ü ����
		const unsigned int width = MatrixA.getWidth();							// ����� ���� �ʺ�
		const unsigned int startPos = getStartPosition(capacity, rank, size);	// ��� ���� ��ġ
		const unsigned int startPosY = startPos / width;						// ��� ���� ���� ��ġ
		const unsigned int endPos = getEndPosition(capacity, rank, size);		// ��� �� ��ġ
		const unsigned int endPosY = endPos / width;							// ��� �� ���� ��ġ

		cout << "Computing on Process " << rank << " from " << processor_name << endl;

		// Host�κ��� �ʱⰪ ���� ����
		unsigned int heightA;
		unsigned int widthA;
		MPI_Recv(&heightA, 1, MPI_INT, HOST, initHeightA, MPI_COMM_WORLD, &status);
		MPI_Recv(&widthA, 1, MPI_INT, HOST, initWidthA, MPI_COMM_WORLD, &status);
		Matrix<NUM> MatrixA(heightA, widthA);
		MPI_Recv(MatrixA.getData(), capacity, MPI_FLOAT, HOST, initDataA, MPI_COMM_WORLD, &status);

		unsigned int heightB;
		unsigned int widthB;
		MPI_Recv(&MatrixB.getHeight(), 1, MPI_INT, HOST, initHeightB, MPI_COMM_WORLD, &status);
		MPI_Recv(&MatrixB.getWidth(), 1, MPI_INT, HOST, initWidthB, MPI_COMM_WORLD, &status);
		Matrix<NUM> MatrixB(heightB, widthB);
		MPI_Recv(MatrixB.getData(), capacity, MPI_FLOAT, HOST, initDataB, MPI_COMM_WORLD, &status);

		Matrix<NUM> MatrixC(heightA, widthB);

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

		// Host�� ����� ����
		int blockSize = getBlockSize(capacity, HOST, size);	// �޽����� ���� ������ ��� ũ��
		MPI_Send(&MatrixC.getData()[startPos], blockSize, MPI_FLOAT, HOST, submul, MPI_COMM_WORLD);
	}
}
