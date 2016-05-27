#include "stdafx.h"
#include "MatrixMul.h"

namespace MatrixSpace
{
	unsigned int getStartPosition(unsigned int capacity, Rank rank, unsigned int rankSize)
	{
		int defaultProcsNum = rankSize - capacity % rankSize;	// 기본 블록크기로 할당할 프로세스 개수
		
		if (rank <= defaultProcsNum) {	// 기본 크기
			return rank * (capacity / rankSize);
		}
		else {							// 남는 셀은 끝 블록부터 하나씩 줌
			return rank * (capacity / rankSize) + (rank - defaultProcsNum);
		}
	}

	unsigned int getEndPosition(unsigned int capacity, Rank rank, unsigned int rankSize)
	{
		if (rank < (int)rankSize - 1) {	// 끝 위치는 다음 시작 위치의 바로 앞
			return getStartPosition(capacity, rank + 1, rankSize) - 1;
			//return getStartPosition(capacity, rank, rankSize) + getBlockSize(capacity, rank, rankSize);
		}
		else {							// 마지막 블록의 끝은 셀 전체 개수보다 하나 작음
			return capacity - 1;
		}
	}

	unsigned int getBlockSize(unsigned int capacity, Rank rank, unsigned int rankSize)
	{
		int defaultProcsNum = rankSize - capacity % rankSize;	// 기본 블록크기로 할당할 프로세스 개수

		if (rank < defaultProcsNum) {
			return capacity / rankSize;
		}
		else {
			return capacity / rankSize + 1;
		}
	}
}