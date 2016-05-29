// main.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "Matrix.cpp"

#define INITWIDTH	10	// 행렬 가로 및 세로 너비 기본값

using namespace std;
using namespace MatrixSpace;


template<class NUM>
void printMatrix(Matrix<NUM>& matrix);


// rank 가 0 번인 프로세스가 수행할 코드
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

	// 명령 인수
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
				case '0':	// A 출력
					print[0] = true;
					break;
				case 'b':
				case '1':	// B 출력
					print[1] = true;
					break;
				case 'c':
				case '2':	// C 출력
					print[2] = true;
					break;
				case 'w':
				case '3':	// A, B, C 전체 출력
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

	// 행렬 생성
	Matrix<float> A(height, correspondence);
	Matrix<float> B(correspondence, width);
	Matrix<float> C(height, width);
	
	// 행렬 초기화
	if (isSquare && !debug) {	// 정방형 행렬 초기화
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				A[i][j] = (float)rand() / 100.0;	// 앞행렬 임의값 초기화
				B[i][j] = (float)rand() / 100.0;	// 뒷행렬 임의값 초기화
			}
		}
	}
	else if (!debug) {			// 비정방형 행렬 초기화
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < correspondence; j++) {
				A[i][j] = (float)rand() / 100.0;	// 앞행렬 임의값 초기화
			}
		}
		for (int i = 0; i < correspondence; i++) {
			for (int j = 0; j < width; j++) {
				B[i][j] = (float)rand() / 100.0;	// 뒷행렬 임의값 초기화
			}
		}
	}
	else {						// -d 옵션 (debug 옵션) 초기화
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < correspondence; j++) {
				A[i][j] = (float)i;	// 앞행렬 초기화
			}
		}
		for (int i = 0; i < correspondence; i++) {
			for (int j = 0; j < width; j++) {
				B[i][j] = (float)j;	// 뒷행렬 초기화
			}
		}
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				C[i][j] = 0;		// 결과 행렬의 초기 값은 0
			}
		}
	}

	// 입력 행렬 출력
	if (print[0]) {
		cout << "Matrix A: " << endl;
		printMatrix(A);
	}
	if (print[1]) {
		cout << "Matrix B: " << endl;
		printMatrix(B);
	}

	// 행렬 곱셈
	MPI_Get_processor_name(processor_name, &namelen);
	cout << "Computing result using MPICH ..." << endl;
	
	cout << "Processing on Process " << rank << " from " << processor_name << endl;
	double wallClockTime = matrixMultiplyUsingMPICH_Host(C, A, B);
	cout << "Completed on process " << rank << " from " << processor_name << endl;

	cout << "Wall clock time: " << wallClockTime << endl;

	// 결과 행렬 출력
	if (print[2]) {
		cout << endl;
		cout << "Matrix C (Result): " << endl;
		printMatrix(C);
	}
}

// rank 가 0 이 아닌 프로세스가 수행할 코드
void satellite_main(int argc, char *argv[], const Rank rank)
{
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;

	MPI_Get_processor_name(processor_name, &namelen);

	cout << "Computing on Process " << rank << " from " << processor_name << endl;
	matrixMultiplyUsingMPICH_Satellite();
	cout << "Completed on process " << rank << " from " << processor_name << endl;
}

// 프로그램 메인
void main(int argc, char *argv[])
{
	Rank rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* 여기까지 모든 프로그램이 동일하게 수행된다.
	 * rank 는 각 프로세스를 구별하기 위해 사용된다.
	 * 주로 rank 0 은 특별한 용도로 사용된다.
	 */

	/* 이 부분부터 프로세스의 rank 에 따라 수행할 코드가 달라진다. */
	if (rank == HOST) {	// rank 가 0 번인 프로세스가 수행할 코드
		host_main(argc, argv);
	}
	else {				// rank 가 0 이 아닌 프로세스가 수행할 코드
		satellite_main(argc, argv, rank);
	}

	MPI_Finalize();
}

// 행렬 출력
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
