#include "stdafx.h"
#include "MatrixMul.h"

#include "Matrix.cpp"
#include "MatrixMulGeneric.cpp"

namespace MatrixSpace
{
	// rank �� 0 ���� ���μ����� ������ �ڵ�
	double matrixMultiplyUsingMPICH_Host(Matrix<float>& MatrixC, const Matrix<float>& MatrixA, const Matrix<float>& MatrixB, const Rank rank)
	{
		MPI_Status status;
		int size;
		
		MPI_Comm_size(MPI_COMM_WORLD, &size);

		// Satellites�� ������ �ʱⰪ
		unsigned int heightA = MatrixA.getHeight();
		unsigned int widthA = MatrixA.getWidth();
		const float *dataA = MatrixA.getData();
		unsigned int heightB = MatrixB.getHeight();
		unsigned int widthB = MatrixB.getWidth();
		const float *dataB = MatrixB.getData();

		// ���� ���� ���� ���� �ʱ�ȭ
		const unsigned int capacity = MatrixA.getCapacity();						// ����� �� ��ü ����
		const unsigned int endPositionHost = getEndPosition(capacity, HOST, size);	// ��� �� ��ġ
		unsigned int *startPosition = new unsigned int[size];
		unsigned int *blockSize = new unsigned int[size];
		for (Rank i = 0; i < size; i++) {
			startPosition[i] = getStartPosition(capacity, i, size);	// ��� ���� ��ġ
			blockSize[i] = getBlockSize(capacity, i, size);			// ������ ��� ũ��
		}

		// ��� ���
		Matrix<float> MatrixResult(heightA, widthA);

		// �ð� ���� ����
		double startwtime = 0.0, endwtime;
		startwtime = MPI_Wtime();

		// Satellites�� �ʱⰪ ����
		for (Rank i = 1; i < size; i++) {
			MPI_Send(&heightA, 1, MPI_INT, i, InitHeightA, MPI_COMM_WORLD);
			MPI_Send(&widthA, 1, MPI_INT, i, InitWidthA, MPI_COMM_WORLD);
			MPI_Send((void*)dataA, capacity, MPI_FLOAT, i, InitDataA, MPI_COMM_WORLD);
			MPI_Send(&heightB, 1, MPI_INT, i, InitHeightB, MPI_COMM_WORLD);
			MPI_Send(&widthB, 1, MPI_INT, i, InitWidthB, MPI_COMM_WORLD);
			MPI_Send((void*)dataB, capacity, MPI_FLOAT, i, InitDataB, MPI_COMM_WORLD);
			MPI_Send(&startPosition[i], 1, MPI_INT, i, StartPos, MPI_COMM_WORLD);
			MPI_Send(&blockSize[i], 1, MPI_INT, i, BlockSize, MPI_COMM_WORLD);
		}

		// ��� ���� (��� ����, Host�� ��� ���� ����)
		matrixMultiply(MatrixResult, MatrixA, MatrixB, startPosition[HOST], endPositionHost);

		// Satellites�κ��� ������� ���� �޾� ��ħ
		for (Rank i = 1; i < size; i++) {
			MPI_Recv(&MatrixResult.getData()[startPosition[i]], blockSize[i], MPI_FLOAT, i, SubResult, MPI_COMM_WORLD, &status);
		}

		// �ð� ���� ����
		endwtime = MPI_Wtime();

		// ��ȯ ��Ŀ� ��� ����
		MatrixC = MatrixResult;
		
		// �޸� ����
		delete[] startPosition;
		delete[] blockSize;

		// ������ �ð� ��ȯ
		return endwtime - startwtime;
	}

	// rank �� 0 �� �ƴ� ���μ����� ������ �ڵ�
	void matrixMultiplyUsingMPICH_Satellite()
	{
		MPI_Status status;

		// Host�κ��� ���� ���� �ʱⰪ
		unsigned int heightA;
		unsigned int widthA;
		unsigned int heightB;
		unsigned int widthB;
		unsigned int startPosition;
		unsigned int blockSize;

		// Host�κ��� �ʱⰪ�� ���� ����
		MPI_Recv(&heightA, 1, MPI_INT, HOST, InitHeightA, MPI_COMM_WORLD, &status);
		MPI_Recv(&widthA, 1, MPI_INT, HOST, InitWidthA, MPI_COMM_WORLD, &status);
		Matrix<float> MatrixA(heightA, widthA);
		MPI_Recv(MatrixA.getData(), MatrixA.getCapacity(), MPI_FLOAT, HOST, InitDataA, MPI_COMM_WORLD, &status);
		MPI_Recv(&heightB, 1, MPI_INT, HOST, InitHeightB, MPI_COMM_WORLD, &status);
		MPI_Recv(&widthB, 1, MPI_INT, HOST, InitWidthB, MPI_COMM_WORLD, &status);
		Matrix<float> MatrixB(heightB, widthB);
		MPI_Recv(MatrixB.getData(), MatrixB.getCapacity(), MPI_FLOAT, HOST, InitDataB, MPI_COMM_WORLD, &status);
		MPI_Recv(&startPosition, 1, MPI_INT, HOST, StartPos, MPI_COMM_WORLD, &status);
		MPI_Recv(&blockSize, 1, MPI_INT, HOST, BlockSize, MPI_COMM_WORLD, &status);

		// ��� ���
		Matrix<float> MatrixC(heightA, widthB);

		// ��� ���� (��� ����)
		matrixMultiply(MatrixC, MatrixA, MatrixB, startPosition, startPosition + blockSize - 1);

		// Host�� ����� ����
		MPI_Send(&MatrixC.getData()[startPosition], blockSize, MPI_FLOAT, HOST, SubResult, MPI_COMM_WORLD);
	}

	unsigned int getStartPosition(unsigned int capacity, Rank rank, unsigned int rankSize)
	{
		int defaultProcsNum = rankSize - capacity % rankSize;	// �⺻ ���ũ��� �Ҵ��� ���μ��� ����

		if (rank <= defaultProcsNum) {	// �⺻ ũ��
			return rank * (capacity / rankSize);
		}
		else {							// ���� ���� �� ��Ϻ��� �ϳ��� ��
			return rank * (capacity / rankSize) + (rank - defaultProcsNum);
		}
	}

	unsigned int getEndPosition(unsigned int capacity, Rank rank, unsigned int rankSize)
	{
		if (rank < (int)rankSize - 1) {	// �� ��ġ�� ���� ���� ��ġ�� �ٷ� ��
			return getStartPosition(capacity, rank + 1, rankSize) - 1;
		}
		else {							// ������ ����� ���� �� ��ü �������� �ϳ� ����
			return capacity - 1;
		}
		//return getStartPosition(capacity, rank, rankSize) + getBlockSize(capacity, rank, rankSize) - 1;
	}

	unsigned int getBlockSize(unsigned int capacity, Rank rank, unsigned int rankSize)
	{
		int defaultProcsNum = rankSize - capacity % rankSize;	// �⺻ ���ũ��� �Ҵ��� ���μ��� ����

		if (rank < defaultProcsNum) {
			return capacity / rankSize;
		}
		else {
			return capacity / rankSize + 1;
		}
	}
}
