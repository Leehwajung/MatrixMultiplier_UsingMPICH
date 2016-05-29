#pragma once

namespace MatrixSpace
{
#define HOST	(Rank)0		// Host Process (Rank 0)

	typedef int	Rank;		// Process Number

	enum Message
	{
		InitHeightA,
		InitWidthA,
		InitDataA,
		InitHeightB,
		InitWidthB,
		InitDataB,
		StartPos,
		BlockSize,
		SubResult
	};

	template<class NUM>
	void matrixMultiply(Matrix<NUM>& MatrixC, const Matrix<NUM>& MatrixA, const Matrix<NUM>& MatrixB, const unsigned int startPos, const unsigned int endPos);

	double matrixMultiplyUsingMPICH_Host(Matrix<float>& MatrixC, const Matrix<float>& MatrixA, const Matrix<float>& MatrixB, const Rank rank = HOST);

	void matrixMultiplyUsingMPICH_Satellite();

	unsigned int getStartPosition(unsigned int capacity, Rank rank, unsigned int rankSize);

	unsigned int getEndPosition(unsigned int capacity, Rank rank, unsigned int rankSize);

	unsigned int getBlockSize(unsigned int capacity, Rank rank, unsigned int rankSize);
}
