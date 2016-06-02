#include "stdafx.h"
#include "MatrixMul.h"

namespace MatrixSpace
{
	// 행렬 블록 곱셈
	template<class NUM>
	void matrixMultiply(Matrix<NUM>& MatrixC, 
		const Matrix<NUM>& MatrixA, const Matrix<NUM>& MatrixB,
		const unsigned int startPos, const unsigned int endPos)
	{
		const unsigned int width = MatrixA.getWidth();		// 행렬 가로 너비
		const unsigned int startPosY = startPos / width;	// 블록 시작 세로 위치
		const unsigned int endPosY = endPos / width;		// 블록 끝 세로 위치

		for (unsigned int y = startPosY, x = startPos - startPosY * width, p = startPos;
			y <= endPosY; y++) {								// 세로 방향 루프
			for (x, p; x < width && p <= endPos; x++, p++) {	// 가로방향 루프
				// Csub is used to store the element
				NUM Csub = 0;

				// Loop over all the sub-matrices of A and B
				for (unsigned int i = 0; i < width; i++) {
					Csub += MatrixA[y][i] * MatrixB[i][x];
				}
				MatrixC[y][x] = Csub;
			}
			x = 0;
		}
	}
}
