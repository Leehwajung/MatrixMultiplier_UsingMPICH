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

		/* 여기까지 모든 프로그램이 동일하게 수행된다.
		* rank 는 각 프로세스를 구별하기 위해 사용된다.
		* 주로 rank 0 은 특별한 용도로 사용된다.
		*/

		if (rank == HOST) {
			cout << "Computing result using MPICH ..." << endl;
		}

		// 문제 분할 관련 변수 초기화
		const unsigned int capacity = MatrixA.getCapacity();					// 행렬 셀 전체 개수
		const unsigned int width = MatrixA.getWidth();							// 행렬 가로 너비
		const unsigned int startPos = getStartPosition(capacity, rank, size);	// 블록 시작 위치
		const unsigned int startPosY = startPos / width;						// 블록 시작 세로 위치
		const unsigned int endPos = getEndPosition(capacity, rank, size);		// 블록 끝 위치
		const unsigned int endPosY = endPos / width;							// 블록 끝 세로 위치

		cout << "Computing on Process " << rank << " from " << processor_name << endl;

		// 시간 측정 시작
		double startwtime = 0.0, endwtime;
		if (rank == HOST) {
			startwtime = MPI_Wtime();
			for (Rank i = 1; i < size; i++) {		//클라이언트로 배열전송
				int currPos = getStartPosition(capacity, i, size);	// 메시지를 저장할 행렬상 시작 위치
				int blockSize = getBlockSize(capacity, i, size);	// 메시지를 받을 데이터 블록 크기
				MPI_Send(&MatrixA.getData()[startPos], getBlockSize(capacity, rank, size), MPI_FLOAT, i, submul, MPI_COMM_WORLD);
				MPI_Send(&MatrixB.getData()[startPos], getBlockSize(capacity, rank, size), MPI_FLOAT, i, submul+1, MPI_COMM_WORLD);
			}
		}

		// 행렬 곱셈
		for (unsigned int y = startPosY, x = startPos - startPosY * width, p = startPos;
			y <= endPosY; y++) {								// 세로 방향 루프
			for (x, p; x < width && p <= endPos; x++, p++) {	// 가로방향 루프
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

		/* 이 부분부터 프로세스의 rank 에 따라 수행할 코드가 달라진다. */
		// Host로 결과값 전달
		if (rank == HOST) {	// rank 가 0 번인 프로세스가 수행할 코드
			for (Rank i = 1; i < size; i++) {
				int currPos = getStartPosition(capacity, i, size);	// 메시지를 저장할 행렬상 시작 위치
				int blockSize = getBlockSize(capacity, i, size);	// 메시지를 받을 데이터 블록 크기
				MPI_Recv(&MatrixC.getData()[currPos], blockSize, MPI_FLOAT, i, submul, MPI_COMM_WORLD, &status);
			}

			// 시간 측정 종료
			endwtime = MPI_Wtime();
			cout << "Wall clock time: " << endwtime - startwtime << endl;

			// 결과 행렬 출력
			cout << endl;
			for (unsigned int i = 0; i < MatrixC.getHeight(); i++) {
				for (unsigned int j = 0; j < MatrixC.getWidth(); j++) {
					cout << MatrixC[i][j] << " ";
				}
				cout << endl;
			}
			cout << endl;
		}
		else {				// rank 가 0 이 아닌 프로세스가 수행할 코드
			MPI_Send(&MatrixC.getData()[startPos], getBlockSize(capacity, rank, size), MPI_FLOAT, HOST, submul, MPI_COMM_WORLD);
		}
	}
}
