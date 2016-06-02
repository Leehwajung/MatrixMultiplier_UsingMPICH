#include "stdafx.h"
#include "MatrixMul.h"

namespace MatrixSpace
{
	// ��� ��� ����
	template<class NUM>
	void matrixMultiply(Matrix<NUM>& MatrixC, 
		const Matrix<NUM>& MatrixA, const Matrix<NUM>& MatrixB,
		const unsigned int startPos, const unsigned int endPos)
	{
		const unsigned int width = MatrixA.getWidth();		// ��� ���� �ʺ�
		const unsigned int startPosY = startPos / width;	// ��� ���� ���� ��ġ
		const unsigned int endPosY = endPos / width;		// ��� �� ���� ��ġ

		for (unsigned int y = startPosY, x = startPos - startPosY * width, p = startPos;
			y <= endPosY; y++) {								// ���� ���� ����
			for (x, p; x < width && p <= endPos; x++, p++) {	// ���ι��� ����
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
