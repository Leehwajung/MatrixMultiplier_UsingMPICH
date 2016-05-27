#include "stdafx.h"
#include "MatrixMul.h"

namespace MatrixSpace
{
	unsigned int getStartPosition(unsigned int capacity, Rank rank, unsigned int rankSize)
	{
		int defaultProcsNum = rankSize - capacity % rankSize;	// �⺻ ���ũ��� �Ҵ��� ���μ��� ����
		
		if (rank <= defaultProcsNum) {	// �⺻ ũ��
			return rank * (capacity / rankSize);
		}
		else {							// ���� ���� �� ��Ϻ��� �ϳ��� ��
			return rank * (capacity / rankSize) + (rank - defaultProcsNum);
		}
	}

	unsigned int getEndPosition(unsigned int capacity, Rank rank, unsigned int rankSize)
	{
		if (rank < (int)rankSize - 1) {	// �� ��ġ�� ���� ���� ��ġ�� �ٷ� ��
			return getStartPosition(capacity, rank + 1, rankSize) - 1;
			//return getStartPosition(capacity, rank, rankSize) + getBlockSize(capacity, rank, rankSize);
		}
		else {							// ������ ����� ���� �� ��ü �������� �ϳ� ����
			return capacity - 1;
		}
	}

	unsigned int getBlockSize(unsigned int capacity, Rank rank, unsigned int rankSize)
	{
		int defaultProcsNum = rankSize - capacity % rankSize;	// �⺻ ���ũ��� �Ҵ��� ���μ��� ����

		if (rank < defaultProcsNum) {
			return capacity / rankSize;
		}
		else {
			return capacity / rankSize + 1;
		}
	}
}