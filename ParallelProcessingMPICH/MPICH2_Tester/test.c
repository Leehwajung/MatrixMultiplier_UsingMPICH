// test.c : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include <stdlib.h>
#include <time.h>
#include <windows.h>

# define arr_len 250	//�迭�� ���� 

int main(int argc, char **argv)
{
	int rank;
	int size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;
	char buff[arr_len];
	int i;

	int arr_1[arr_len][arr_len];	// �迭 1
	int arr_2[arr_len][arr_len];	// �迭 2 
	int result[arr_len][arr_len];	// ����� ���� �迭

	LARGE_INTEGER tFreq, tStart, tEnd;	//�ð� ���� ������
	float tElapsedTime;		//�ð� ���� ������

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	for (int i = 0; i<arr_len; i++)		//�迭1,2 �ʱ�ȭ
		for (int j = 0; j<arr_len; j++){
			arr_1[i][j] = rand() % 3 + 1;
			arr_2[i][j] = rand() % 3 + 1;
		}

	printf("array initialization ended. start array multiplication.");


	QueryPerformanceFrequency(&tFreq);        // ���ļ� ����
	QueryPerformanceCounter(&tStart);            // ī��Ʈ ����

	if (rank == 0){		//���μ��� 0���� �� 



		for (i = 1; i < size; i++) {		//�迭 ����
			MPI_Send(arr_1, arr_len*arr_len, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(arr_2, arr_len*arr_len, MPI_INT, i, 1, MPI_COMM_WORLD);
		}


		for (int k = 0; k < arr_len; k++){				//��� �迭 y�� �ݺ���
			for (int i = 0; i < arr_len; i++){		//����迭 x��(�迭 1 y, �迭 2 x) �ݺ��� 
				if (k%size == rank){
					int buf = 0;
					for (int j = 0; j < arr_len; j++)
						buf = buf + (arr_1[i][j] * arr_2[j][i]);		//�迭 1�� ��������
					result[k][i] = buf;

				}

			}

		}

		for (int k = 0; k < arr_len; k++)
			if (k%size != 0){
				for (int i = 0; i < arr_len; i++){
					MPI_Recv(buff, arr_len, MPI_INT, 0, k, MPI_COMM_WORLD, &status);
					result[k][i] = buff[i];
				}
			}


		QueryPerformanceCounter(&tEnd);            // ī��Ʈ ����
		tElapsedTime = ((tEnd.QuadPart - tStart.QuadPart) / (float)tFreq.QuadPart);
		// ���� �ð� ����
		printf("\n multiplication ended.\n");
		printf("measure time : %lf s", tElapsedTime);
	}		//end if
	
	else{		//���μ����� 0�� �ƴ� ���
		for (i = 1; i < size; i++) {		//���μ���0���� ������ �迭 ����
			MPI_Recv(arr_1, arr_len*arr_len, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(arr_2, arr_len*arr_len, MPI_INT, i, 1, MPI_COMM_WORLD, &status);

		}
		for (int k = 0; k<arr_len; k++){				//��� �迭 y�� �ݺ���
			for (int i = 0; i<arr_len; i++){		//����迭 x��(�迭 1 y, �迭 2 x) �ݺ��� 
				if (k%size == 0){
					buff[i] = 0;	//�迭 ���� ��, ������ �����ϴ� �κ� �����ϴ� ����
					for (int j = 0; j < arr_len;j++)
						buff[i] = buff[i] + (arr_1[i][j] * arr_2[j][i]);		//�迭 1�� ��������


				}
								
			}
			MPI_Send(buff, arr_len, MPI_INT, 0, k, MPI_COMM_WORLD);		//���μ��� 0�� result ����

		}
	
	}		//end else

	
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

