#pragma once

namespace MatrixSpace
{
	template<class NUM>
	class Matrix
	{
	// Methods
	public:
		// Constructors
		Matrix(unsigned int height, unsigned int width);
		Matrix(unsigned int height, unsigned int width, const NUM data[]);
		Matrix(Matrix& matrix);

		// Destructor
		~Matrix();

		// Accessors
		unsigned int getHeight() const;
		unsigned int getWidth() const;
		unsigned int getCapacity() const;
		NUM* getData();
		NUM getData(unsigned int y, unsigned int x) const;

		// Mutators
		void setData(const NUM data[]);
		void setData(const NUM& data, unsigned int y, unsigned int x);

		// Operators
		Matrix<NUM>& operator =(const Matrix<NUM>& rightSide);
		NUM* operator [](unsigned int y);
		const NUM* operator [](unsigned int y) const;

	// Fields
	private:
		unsigned int m_height;
		unsigned int m_width;
		NUM *m_data;
	};
}
