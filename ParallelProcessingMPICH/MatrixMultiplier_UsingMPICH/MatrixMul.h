#pragma once

namespace MatrixSpace
{
#define HOST	(Rank)0		// Host Process (Rank 0)

	typedef int	Rank;		// Process Number

	enum Message
	{
		initHeightA,
		initWidthA,
		initDataA,
		initHeightB,
		initWidthB,
		initDataB,
		submul
	};

	template<class NUM>
	void matrixMultiplyOnHost(Matrix<NUM>& MatrixC, const Matrix<NUM>& MatrixA, const Matrix<NUM>& MatrixB);

	template<class NUM>
	void matrixMultiplyOnClient(Rank rank);

	unsigned int getStartPosition(unsigned int capacity, Rank rank, unsigned int rankSize);

	unsigned int getEndPosition(unsigned int capacity, Rank rank, unsigned int rankSize);

	unsigned int getBlockSize(unsigned int capacity, Rank rank, unsigned int rankSize);
}
