// test.c : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <stdlib.h>
#include <time.h>
#include <windows.h>

# define arr_len 10	//배열의 길이 

int main(int argc, char **argv)
{
	int rank;
	int size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;
	char buff[arr_len];
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


	if (rank == 0){		//프로세스 0번일 때 
		printf("HELLO wORLD");
		fflush(stdout);
		for (int i = 0; i<arr_len; i++)		//배열1,2 초기화
			for (int j = 0; j<arr_len; j++){
				arr_1[i][j] = rand() % 3 + 1;
				arr_2[i][j] = rand() % 3 + 1;
			}

		printf("array initialization ended. start array multiplication.");
		fflush(stdout);


		QueryPerformanceFrequency(&tFreq);        // 주파수 측정
		QueryPerformanceCounter(&tStart);            // 카운트 시작

		printf("카운트 시작완료");
		fflush(stdout);


		for (i = 1; i < size; i++) {		//배열 전송
			MPI_Send(arr_1, arr_len*arr_len, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(arr_2, arr_len*arr_len, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
		printf("전송완료");
		fflush(stdout);


		for (int k = 0; k < arr_len; k++){				//결과 배열 y값 반복문
			for (int i = 0; i < arr_len; i++){		//결과배열 x값(배열 1 y, 배열 2 x) 반복문 
				if (k%size == rank){
					int buf = 0;
					for (int j = 0; j < arr_len; j++)
						buf = buf + (arr_1[i][j] * arr_2[j][i]);		//배열 1줄 곱셈연산
					result[k][i] = buf;

				}

			}
		}


		printf("프로세스 0 값 계산 완료");
		fflush(stdout);


		for (int k = 0; k < arr_len; k++)
			if (k%size != 0){
				for (int i = 0; i < arr_len; i++){
					MPI_Recv(buff, arr_len, MPI_INT, 0, k%size, MPI_COMM_WORLD, &status);
					result[k][i] = buff[i];
				}
			}

		printf("받아오기 완료");
		fflush(stdout);

		QueryPerformanceCounter(&tEnd);            // 카운트 종료
		tElapsedTime = ((tEnd.QuadPart - tStart.QuadPart) / (float)tFreq.QuadPart);
		// 측정 시간 저장
		printf("\n multiplication ended.\n");
		fflush(stdout);
		printf("measure time : %lf s", tElapsedTime);
		fflush(stdout);
	}		//end if
	
	else{		//프로세서가 0이 아닐 경우
		for (i = 1; i < size; i++) {		//프로세서0에서 전송한 배열 받음
			MPI_Recv(arr_1, arr_len*arr_len, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(arr_2, arr_len*arr_len, MPI_INT, i, 1, MPI_COMM_WORLD, &status);

		}


		printf("123 받기 완료");
		fflush(stdout);

		for (int k = 0; k<arr_len; k++){				//결과 배열 y값 반복문
			for (int i = 0; i<arr_len; i++){		//결과배열 x값(배열 1 y, 배열 2 x) 반복문 
				if (k%size == 0){
					buff[i] = 0;	//배열 곱셈 시, 곱셈과 덧셈하는 부분 저장하는 변수
					for (int j = 0; j < arr_len; j++)
						buff[i] = buff[i] + (arr_1[i][j] * arr_2[j][i]);		//배열 1줄 곱셈연산


				}
			}
			MPI_Send(buff, arr_len, MPI_INT, 0, k, MPI_COMM_WORLD);		//프로세서 0에 result 전송

		}
		printf("123 보내기 완료");
		fflush(stdout);

	}		//end else

	MPI_Finalize();
	
	return 0;
}	//end main

