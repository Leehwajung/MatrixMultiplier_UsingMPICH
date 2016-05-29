#include "stdafx.h"
#include "MatrixMul.h"

#include "Matrix.cpp"
#include "MatrixMulGeneric.cpp"

namespace MatrixSpace
{
	// rank 가 0 번인 프로세스가 수행할 코드
	double matrixMultiplyUsingMPICH_Host(Matrix<float>& MatrixC, const Matrix<float>& MatrixA, const Matrix<float>& MatrixB, const Rank rank)
	{
		MPI_Status status;
		int size;
		
		MPI_Comm_size(MPI_COMM_WORLD, &size);

		// Satellites로 전달할 초기값
		unsigned int heightA = MatrixA.getHeight();
		unsigned int widthA = MatrixA.getWidth();
		const float *dataA = MatrixA.getData();
		unsigned int heightB = MatrixB.getHeight();
		unsigned int widthB = MatrixB.getWidth();
		const float *dataB = MatrixB.getData();

		// 문제 분할 관련 변수 초기화
		const unsigned int capacity = MatrixA.getCapacity();						// 앞행렬 셀 전체 개수
		const unsigned int endPositionHost = getEndPosition(capacity, HOST, size);	// 블록 끝 위치
		unsigned int *startPosition = new unsigned int[size];
		unsigned int *blockSize = new unsigned int[size];
		for (Rank i = 0; i < size; i++) {
			startPosition[i] = getStartPosition(capacity, i, size);	// 블록 시작 위치
			blockSize[i] = getBlockSize(capacity, i, size);			// 데이터 블록 크기
		}

		// 결과 행렬
		Matrix<float> MatrixResult(heightA, widthA);

		// 시간 측정 시작
		double startwtime = 0.0, endwtime;
		startwtime = MPI_Wtime();

		// Satellites로 초기값 전달
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

		// 행렬 곱셈 (블록 단위, Host도 행렬 곱셈 참여)
		matrixMultiply(MatrixResult, MatrixA, MatrixB, startPosition[HOST], endPositionHost);

		// Satellites로부터 결과값을 전달 받아 합침
		for (Rank i = 1; i < size; i++) {
			MPI_Recv(&MatrixResult.getData()[startPosition[i]], blockSize[i], MPI_FLOAT, i, SubResult, MPI_COMM_WORLD, &status);
		}

		// 시간 측정 종료
		endwtime = MPI_Wtime();

		// 반환 행렬에 결과 복사
		MatrixC = MatrixResult;
		
		// 메모리 해제
		delete[] startPosition;
		delete[] blockSize;

		// 측정된 시간 반환
		return endwtime - startwtime;
	}

	// rank 가 0 이 아닌 프로세스가 수행할 코드
	void matrixMultiplyUsingMPICH_Satellite()
	{
		MPI_Status status;

		// Host로부터 전달 받을 초기값
		unsigned int heightA;
		unsigned int widthA;
		unsigned int heightB;
		unsigned int widthB;
		unsigned int startPosition;
		unsigned int blockSize;

		// Host로부터 초기값을 전달 받음
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

		// 결과 행렬
		Matrix<float> MatrixC(heightA, widthB);

		// 행렬 곱셈 (블록 단위)
		matrixMultiply(MatrixC, MatrixA, MatrixB, startPosition, startPosition + blockSize - 1);

		// Host로 결과값 전달
		MPI_Send(&MatrixC.getData()[startPosition], blockSize, MPI_FLOAT, HOST, SubResult, MPI_COMM_WORLD);
	}

	unsigned int getStartPosition(unsigned int capacity, Rank rank, unsigned int rankSize)
	{
		int defaultProcsNum = rankSize - capacity % rankSize;	// 기본 블록크기로 할당할 프로세스 개수

		if (rank <= defaultProcsNum) {	// 기본 크기
			return rank * (capacity / rankSize);
		}
		else {							// 남는 셀은 끝 블록부터 하나씩 줌
			return rank * (capacity / rankSize) + (rank - defaultProcsNum);
		}
	}

	unsigned int getEndPosition(unsigned int capacity, Rank rank, unsigned int rankSize)
	{
		if (rank < (int)rankSize - 1) {	// 끝 위치는 다음 시작 위치의 바로 앞
			return getStartPosition(capacity, rank + 1, rankSize) - 1;
		}
		else {							// 마지막 블록의 끝은 셀 전체 개수보다 하나 작음
			return capacity - 1;
		}
		//return getStartPosition(capacity, rank, rankSize) + getBlockSize(capacity, rank, rankSize) - 1;
	}

	unsigned int getBlockSize(unsigned int capacity, Rank rank, unsigned int rankSize)
	{
		int defaultProcsNum = rankSize - capacity % rankSize;	// 기본 블록크기로 할당할 프로세스 개수

		if (rank < defaultProcsNum) {
			return capacity / rankSize;
		}
		else {
			return capacity / rankSize + 1;
		}
	}
}
