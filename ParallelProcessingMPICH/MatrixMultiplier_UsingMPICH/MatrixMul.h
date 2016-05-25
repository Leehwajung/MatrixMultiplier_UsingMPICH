#pragma once

namespace MatrixSpace
{
#define HOST	(Rank)0		// Host Process (Rank 0)

	typedef int	Rank;		// Process Number

	enum Message
	{
		total,
		submul
	};

	template<class NUM>
	void matrixMultiplyUsingMPICH(Matrix<NUM>& MatrixC, const Matrix<NUM>& MatrixA, const Matrix<NUM>& MatrixB);
}
