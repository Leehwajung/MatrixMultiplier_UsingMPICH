#include "stdafx.h"
#include "Matrix.h"

namespace MatrixSpace
{
	template<class NUM>
	Matrix<NUM>::Matrix(unsigned int height, unsigned int width)
		:m_height(height), m_width(width)
	{
		this->m_data = new NUM[getCapacity()];
	}

	template<class NUM>
	Matrix<NUM>::Matrix(unsigned int height, unsigned int width, const NUM data[])
		:Matrix(height, width)
	{
		setData(data);
	}

	template<class NUM>
	Matrix<NUM>::Matrix(Matrix& matrix)
		:Matrix(matrix->getData(), matrix->getWidth(), matrix->getHeight())
	{
	}

	template<class NUM>
	Matrix<NUM>::~Matrix()
	{
		delete[] this->m_data;
	}

	template<class NUM>
	NUM* Matrix<NUM>::getData()
	{
		return m_data;
	}

	template<class NUM>
	NUM Matrix<NUM>::getData(unsigned int y, unsigned int x)
	{
		return *this[y][x];
	}

	template<class NUM>
	unsigned int Matrix<NUM>::getWidth()
	{
		return m_width;
	}

	template<class NUM>
	unsigned int Matrix<NUM>::getHeight()
	{
		return m_height;
	}

	template<class NUM>
	unsigned int Matrix<NUM>::getCapacity()
	{
		return m_height * m_width;
	}

	template<class NUM>
	void Matrix<NUM>::setData(const NUM data[])
	{
		for (unsigned int i; i < m_height * m_width; i++) {
			this->m_data[i] = data[i];
		}
	}

	template<class NUM>
	void Matrix<NUM>::setData(const NUM& data, unsigned int y, unsigned int x)
	{
		m_data[y * m_width + x] = data;
	}

	template<class NUM>
	Matrix<NUM>& Matrix<NUM>::operator=(const Matrix<NUM>& rightSide)
	{
		if (getCapacity() != rightSide.getCapacity()) {
			delete[] m_data;
			m_data = new NUM[rightSide.getCapacity()];
		}

		m_height = rightSide.m_height;
		m_width = rightSide.m_width;
		for (unsigned int i; i < getCapacity(); i++) {
			m_data[i] = rightSide.m_data[i];
		}
	}

	template<class NUM>
	NUM* Matrix<NUM>::operator[](unsigned int y)
	{
		return &m_data[y * m_width];
	}
}