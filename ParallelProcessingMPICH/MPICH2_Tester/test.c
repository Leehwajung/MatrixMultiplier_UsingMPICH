// test.c : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define MAX_DATA 100

# define arr_len 100	//배열의 길이 

int main(int argc, char **argv)
{
	int rank;
	int size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;
	char buff[MAX_DATA];
	int i;

	int arr_1[arr_len][arr_len];	// 배열 1
	int arr_2[arr_len][arr_len];	// 배열 2 
	int result[arr_len][arr_len];	// 결과값 담을 배열

	LARGE_INTEGER tFreq, tStart, tEnd;	//시간 관련 변수들
	float tElapsedTime;		//시간 관련 변수들

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	for (int i = 0; i<arr_len; i++)		//배열1,2 초기화
		for (int j = 0; j<arr_len; j++){
			arr_1[i][j] = rand() % 3 + 1;
			arr_2[i][j] = rand() % 3 + 1;
		}

	printf("array initialization ended. start array multiplication.");


	QueryPerformanceFrequency(&tFreq);        // 주파수 측정
	QueryPerformanceCounter(&tStart);            // 카운트 시작
	for (int k = 0; k<arr_len; k++){				//결과 배열 y값 반복문
		for (int i = 0; i<arr_len; i++){		//결과배열 x값(배열 1 y, 배열 2 x) 반복문 
			int buf = 0;	//배열 곱셈 시, 곱셈과 덧셈하는 부분 저장하는 변수

			for (int j = 0; j<arr_len; j++)		// 배열 1 x, 배열 2 y값 반복문
				buf = buf + (arr_1[i][j] * arr_2[j][i]);		//배열 1줄 곱셈연산
			result[k][i] = buf;	//1줄 곱셈한 결과 결과 배열에 저장
		}
	}

	QueryPerformanceCounter(&tEnd);            // 카운트 종료
	tElapsedTime = ((tEnd.QuadPart - tStart.QuadPart) / (float)tFreq.QuadPart);
	// 측정 시간 저장
	printf("\n multiplication ended.\n");
	printf("measure time : %lf s", tElapsedTime);
	
	
	/*
	if (rank == 0) {
		printf_s("### Process %d from %s ###\n", rank, processor_name);
		fflush(stdout);
		for (i = 1; i < size; i++) {
			MPI_Recv(buff, MAX_DATA, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
			printf_s("%s\n", buff); fflush(stdout);
		}
	}
	else {
		sprintf_s(buff, MAX_DATA, "### Process %d from %s ###", rank, processor_name);
		fflush(stdout);
		MPI_Send(buff, MAX_DATA, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}
	*/
	MPI_Finalize();
	
	return 0;
}	//end main

