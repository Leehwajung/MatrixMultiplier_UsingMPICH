// test.c : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"


#include <stdlib.h>
#include <sys\timeb.h>
#include <time.h>
#include <windows.h>

#define MAX_DATA 1000

void getDiffTime(struct _timeb t1, struct _timeb t2);

int main(int argc, char **argv)
{
	int rank;
	int size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;
	//int matrix[2][MAX_DATA][MAX_DATA];
	int* result;
	int i;
	int buf[MAX_DATA];
	MPI_Status status;
	int *arr1, *arr2;
	struct _timeb t1, t2, dt;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);
	//srand(time(0));
	arr1 = malloc(sizeof(int)*MAX_DATA*MAX_DATA);
	arr2 = malloc(sizeof(int)*MAX_DATA*MAX_DATA);
	result = malloc(sizeof(int)*MAX_DATA*MAX_DATA);
	if (rank == 0) { // rank �� 0 ���� ���μ����� ������ �ڵ�
					 //printf("### Process %d from %s ###\n", rank, processor_name); fflush(stdout);

		_ftime_s(&t1);//���۽ð�
		for (int i = 0; i < MAX_DATA; i++) {//��� �ʱ�ȭ
			for (int j = 0; j < MAX_DATA; j++) {
				//matrix[0][i][j]=rand() % 10;
				//matrix[1][i][j]=rand() % 10;
				arr1[i*MAX_DATA + j] = rand() % 10;
				arr2[i*MAX_DATA + j] = rand() % 10;
			}
		}

		for (i = 1; i<size; i++) {
			MPI_Send(arr1, MAX_DATA*MAX_DATA, MPI_INT, i, 0, MPI_COMM_WORLD);//��� ����
			MPI_Send(arr2, MAX_DATA*MAX_DATA, MPI_INT, i, 1, MPI_COMM_WORLD);//��� ����
																			 //MPI_Recv(buff, MAX_DATA, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
																			 //printf("%s\n", buff); fflush(stdout);
		}

		for (int i = 0; i < MAX_DATA; i++) {//���μ��� 0�� ���
			for (int j = 0; j < MAX_DATA; j++) {
				if (i % size == rank) {
					result[i*MAX_DATA + j] = 0;
					//for (int w = 0; w < MAX_DATA; w++) {
					for (int k = 0; k < MAX_DATA; k++) {
						result[i*MAX_DATA + j] = result[i*MAX_DATA + j] + arr1[i*MAX_DATA + k] * arr2[k*MAX_DATA + j];
					}
					//}
				}
			}
		}


		for (i = 1; i<size; i++) {//���μ��� ��� ��ŭ
			for (int k = i; k < MAX_DATA; k = k + size) {//���μ����� �Ҵ�� ��� �ޱ�
				MPI_Recv(buf, MAX_DATA, MPI_INT, i, k, MPI_COMM_WORLD, &status);//��� ��� �ޱ�
				for (int j = 0; j < MAX_DATA; j++) {
					result[k*MAX_DATA + j] = buf[j];
				}
			}
		}

		/*
		for (int i = 0; i < MAX_DATA; i++) {//���1 ���
		for (int j = 0; j < MAX_DATA; j++) {
		printf("%d ", arr1[i*MAX_DATA + j]);
		}
		printf("\n");
		}
		printf("\n\n\n\n");
		for (int i = 0; i < MAX_DATA; i++) {//���2 ���
		for (int j = 0; j < MAX_DATA; j++) {
		printf("%d ", arr2[i*MAX_DATA + j]);
		}
		printf("\n");
		}
		printf("\n\n\n\n");
		for (int i = 0; i < MAX_DATA; i++) {//��� ���
		for (int j = 0; j < MAX_DATA; j++) {
		printf("%d ", result[i*MAX_DATA + j]);
		}
		printf("\n");
		}*/

		_ftime_s(&t2);//����ð�
		printf("��� ũ��:%d\n���μ��� ��:%d\n�ð�:",MAX_DATA, size);
		getDiffTime(t1, t2);

	}
	else { //rank �� 0 �� �ƴ� ���μ����� ������ �ڵ�
		MPI_Recv(arr1, MAX_DATA*MAX_DATA, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(arr2, MAX_DATA*MAX_DATA, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		for (int i = 0; i < MAX_DATA; i++) {
			for (int j = 0; j < MAX_DATA; j++) {
				if (i % size == rank) {
					buf[j] = 0;
					for (int k = 0; k < MAX_DATA; k++) {
						buf[j] = buf[j] + arr1[i*MAX_DATA + k] * arr2[k*MAX_DATA + j];
					}
				}

			}
			MPI_Send(buf, MAX_DATA, MPI_INT, 0, i, MPI_COMM_WORLD);
		}
	}




	free(arr1);
	free(arr2);
	free(result);
	MPI_Finalize();
}

void getDiffTime(struct _timeb t1, struct _timeb t2) {
	int milltm = t2.millitm - t1.millitm;
	int sec = t2.time - t1.time;
	float r;
	if (milltm < 0)
		milltm += 1000;
	r = sec + milltm*0.001;
	printf("%f��", r);
}


