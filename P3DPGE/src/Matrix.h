#pragma once
#include "Debug.h"

struct Vector3;

struct Matrix {
	uint32 rows;
	uint32 cols;
	uint32 elementCount; //this might be convinient to have, but it increases size by 8 bytes?
	float* data;

	Matrix(uint32 inRows, uint32 inCols);
	Matrix(uint32 inRows, uint32 inCols, float inData[]);
	Matrix(uint32 inRows, uint32 inCols, std::initializer_list<float> list);
	Matrix(const Matrix& m);
	Matrix(Vector3 v); //TODO(delle) define this in Math.h
	Matrix(Vector3 v, float w); //TODO(delle) define this in Math.h
	~Matrix();

	float&	operator () (uint32 row, uint32 col);
	void	operator =	(const Matrix& rhs);
	Matrix  operator *  (const float& rhs) const;
	void	operator *= (const float& rhs);
	Matrix  operator /  (const float& rhs) const;
	void	operator /= (const float& rhs);
	Matrix  operator +  (const Matrix& rhs) const;
	void	operator += (const Matrix& rhs);
	Matrix  operator -  (const Matrix& rhs) const;
	void	operator -= (const Matrix& rhs);
	Matrix  operator *  (const Matrix& rhs) const;
	void	operator *= (const Matrix& rhs);
	Matrix  operator ^  (const Matrix& rhs) const;
	void	operator ^= (const Matrix& rhs);
	Matrix  operator %  (const Matrix& rhs) const; 
	void	operator %= (const Matrix& rhs);
	bool	operator == (const Matrix& rhs) const;
	bool	operator != (const Matrix& rhs) const;
	friend Matrix operator* (const float& lhs, const Matrix& rhs) { return rhs * lhs; }

	const std::string str() const;
	const std::string str2F() const;
	void Copy(const Matrix& m);
	Matrix Transpose();
	Matrix Submatrix(std::initializer_list<float> inRows, std::initializer_list<float> inCols);
	float Determinant();
	Matrix Inverse();

	static Matrix Identity(int rows, int cols);
};

//// Constructors ////

inline Matrix::Matrix(uint32 inRows, uint32 inCols) : rows(inRows), cols(inCols) {
	ASSERT(inRows != 0 && inCols != 0, "Matrix constructor was given zero size");
	this->elementCount = inRows * inCols;
	this->data = new float[elementCount]();
}

inline Matrix::Matrix(uint32 inRows, uint32 inCols, float inData[]) : rows(inRows), cols(inCols) {
	ASSERT(inRows != 0 && inCols != 0, "Matrix constructor was given zero size");
	this->elementCount = inRows * inCols;
	uint32 inCount = sizeof(*inData) / sizeof(float);
	ASSERT(inCount <= elementCount, "Matrix constructor was given too many elements for given dimensions");
	this->data = new float[elementCount]();
	for (uint32 i = 0; i < inCount; ++i) {
		this->data[i] = inData[i];
	}

}

inline Matrix::Matrix(uint32 inRows, uint32 inCols, std::initializer_list<float> list) : rows(inRows), cols(inCols) {
	ASSERT(inRows != 0 && inCols != 0, "Matrix constructor was given zero size");
	this->elementCount = inRows * inCols;
	uint32 inCount = list.size();
	ASSERT(inCount <= elementCount, "Matrix constructor was given too many elements for given dimensions");
	this->data = new float[elementCount]();
	int i = 0;
	for (auto& f : list) {
		this->data[i] = f;
		++i;
	}
}

inline Matrix::Matrix(const Matrix& m) {
	this->rows = m.rows;
	this->cols = m.cols;
	this->elementCount = rows * cols;
	this->data = new float[elementCount]();
	Copy(m);
}

inline Matrix::~Matrix() {
	delete[] data;
}

//// Operators ////

//element accessor: matrix(row,col)
inline float& Matrix::operator () (uint32 row, uint32 col) {
	ASSERT(row <= rows && col <= cols, "Matrix subscript out of bounds");
	return data[cols*row + col];
}

//deletes current data, copies properties from rhs, creates a new copy of the data from rhs
inline void	   Matrix::operator =  (const Matrix& rhs) {
	if (this->data != rhs.data) {
		this->rows = rhs.rows;
		this->cols = rhs.cols;
		this->elementCount = rows * cols;
		this->~Matrix();
		this->data = new float[elementCount]();
		Copy(rhs);
	}
}

//scalar multiplication
inline Matrix  Matrix::operator *  (const float& rhs) const {
	Matrix newMatrix(*this);
	for (int i = 0; i < newMatrix.elementCount; ++i) {
		newMatrix.data[i] *= rhs;
	}
	return newMatrix;
}

//scalar multiplication and assignment
inline void    Matrix::operator *= (const float& rhs) {
	for (int i = 0; i < elementCount; ++i) {
		this->data[i] *= rhs;
	}
}

//scalar division
inline Matrix  Matrix::operator /  (const float& rhs) const {
	ASSERT(rhs != 0, "Matrix elements cant be divided by zero");
	Matrix newMatrix(*this);
	for (int i = 0; i < newMatrix.elementCount; ++i) {
		newMatrix.data[i] /= rhs;
	}
	return newMatrix;
}

//scalar division and assignment
inline void    Matrix::operator /= (const float& rhs){
	ASSERT(rhs != 0, "Matrix elements cant be divided by zero");
	for (int i = 0; i < elementCount; ++i) {
		this->data[i] /= rhs;
	}
}

//element-wise addition
inline Matrix  Matrix::operator +  (const Matrix& rhs) const{
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix addition requires the same dimensions");
	Matrix newMatrix(*this);
	for (int i = 0; i < newMatrix.elementCount; ++i) {
		newMatrix.data[i] += rhs.data[i];
	}
	return newMatrix;
}

//element-wise addition and assignment
inline void    Matrix::operator += (const Matrix& rhs){
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix addition requires the same dimensions");
	for (int i = 0; i < elementCount; ++i) {
		this->data[i] += rhs.data[i];
	}
}

//element-wise substraction
inline Matrix  Matrix::operator -  (const Matrix& rhs) const{
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix subtraction requires the same dimensions");
	Matrix newMatrix(*this);
	for (int i = 0; i < newMatrix.elementCount; ++i) {
		newMatrix.data[i] -= rhs.data[i];
	}
	return newMatrix;
}

//element-wise substraction and assignment
inline void    Matrix::operator -= (const Matrix& rhs){
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix subtraction requires the same dimensions");
	for (int i = 0; i < elementCount; ++i) {
		this->data[i] -= rhs.data[i];
	}
}

//TODO(o,delle) look into optimizing this by transposing to remove a loop, see Unreal implementation
inline Matrix  Matrix::operator *  (const Matrix& rhs) const{
	ASSERT(cols == rhs.rows, "Matrix multiplication requires the columns of the left matrix to equal the rows of the right matrix");
	Matrix newMatrix(rows, rhs.cols);
	for (int i = 0; i < this->rows; ++i) { //i=m
		for (int j = 0; j < rhs.cols; ++j) { //j=p
			for (int k = 0; k < rhs.rows; ++k) { //k=n
				newMatrix.data[rhs.cols * i + j] += this->data[this->cols * i + k] * rhs.data[rhs.cols * k + j];
			}
		}
	}
	return newMatrix;
}

inline void    Matrix::operator *= (const Matrix& rhs){
	ASSERT(cols == rhs.rows, "Matrix multiplication requires the columns of the left matrix to equal the rows of the right matrix");
	Matrix newMatrix(rows, rhs.cols);
	for (int i = 0; i < this->rows; ++i) { //i=m
		for (int j = 0; j < rhs.cols; ++j) { //j=p
			for (int k = 0; k < rhs.rows; ++k) { //k=n
				newMatrix.data[rhs.cols * i + j] += this->data[this->cols * i + k] * rhs.data[rhs.cols * k + j];
			}
		}
	}
	*this = newMatrix;
}

//element-wise multiplication
inline Matrix  Matrix::operator ^  (const Matrix& rhs) const{
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix element-wise multiplication requires the same dimensions");
	Matrix newMatrix(*this);
	for (int i = 0; i < newMatrix.elementCount; ++i) {
		newMatrix.data[i] *= rhs.data[i];
	}
	return newMatrix;
} 

//element-wise multiplication and assignment
inline void    Matrix::operator ^= (const Matrix& rhs){
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix element-wise multiplication requires the same dimensions");
	for (int i = 0; i < elementCount; ++i) {
		this->data[i] *= rhs.data[i];
	}
}

//element-wise division
inline Matrix  Matrix::operator %  (const Matrix& rhs) const{
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix element-wise division requires the same dimensions");
	Matrix newMatrix(*this);
	for (int i = 0; i < newMatrix.elementCount; ++i) {
		ASSERT(rhs.data[i] != 0, "Matrix element-wise division doesnt allow zeros in the right matrix");
		newMatrix.data[i] /= rhs.data[i];
	}
	return newMatrix;
} 

//element-wise division and assignment
inline void    Matrix::operator %= (const Matrix& rhs){
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix element-wise division requires the same dimensions");
	for (int i = 0; i < elementCount; ++i) {
		ASSERT(rhs.data[i] != 0, "Matrix element-wise division doesnt allow zeros in the right matrix");
		this->data[i] /= rhs.data[i];
	}
}

inline bool	   Matrix::operator	== (const Matrix& rhs) const { 
	if (this->rows != rhs.rows || this->cols != rhs.cols || this->elementCount != rhs.elementCount) {
		return false;
	}
	for (int i = 0; i < elementCount; ++i) {
		if (this->data[i] != rhs.data[i]) {
			return false;
		}
	}
	return true;
}

inline bool    Matrix::operator	!= (const Matrix& rhs) const { 
	return !(*this == rhs); 
}

//// Functions ////

//TODO(c,delle) clean up Matrix.str() and Matrix.str2F()
const std::string Matrix::str() const {
	if (rows == 0 || cols == 0) {
		return "|Zero dimension matrix|";
	}

	std::string str = std::to_string(rows) + "x" + std::to_string(cols) + " Matrix:\n|";
	if (rows == 1) {
		for (int i = 0; i < cols-1; ++i) {
			char buffer[15];
			std::snprintf(buffer, 15, "%+.6f", data[i]);
			str += std::string(buffer) + ", ";
		}
		char buffer[15];
		std::snprintf(buffer, 15, "%+.6f", data[elementCount - 1]);
		str += std::string(buffer) + "|";
		return str;
	}

	for (int i = 0; i < elementCount-1; ++i) {
		char buffer[15];
		std::snprintf(buffer, 15, "%+.6f", data[i]);
		str += std::string(buffer);
		if ((i+1) % cols != 0) {
			str += ", ";
		} else {
			str += "|\n|";
		}
	}
	char buffer[15];
	std::snprintf(buffer, 15, "%+.6f", data[elementCount - 1]);
	str += std::string(buffer) + "|";
	return str;
};

const std::string Matrix::str2F() const {
	if (rows == 0 || cols == 0) {
		return "|Zero dimension matrix|";
	}

	std::string str = std::to_string(rows) + "x" + std::to_string(cols) + " Matrix:\n|";
	if (rows == 1) {
		for (int i = 0; i < cols - 1; ++i) {
			char buffer[15];
			std::snprintf(buffer, 15, "%+.2f", data[i]);
			str += std::string(buffer) + ", ";
		}
		char buffer[15];
		std::snprintf(buffer, 15, "%+.2f", data[elementCount - 1]);
		str += std::string(buffer) + "|";
		return str;
	}

	for (int i = 0; i < elementCount - 1; ++i) {
		char buffer[15];
		std::snprintf(buffer, 15, "%+.2f", data[i]);
		str += std::string(buffer);
		if ((i + 1) % cols != 0) {
			str += ", ";
		} else {
			str += "|\n|";
		}
	}
	char buffer[15];
	std::snprintf(buffer, 15, "%+.2f", data[elementCount - 1]);
	str += std::string(buffer) + "|";
	return str;
};

//copys the data from one matrix to the other
//REQUIRES both to have the same dimensions
inline void Matrix::Copy(const Matrix& m) {
	ASSERT(rows == m.rows && cols == m.cols, "Cant copy matrices of unequal dimensions");
	float* p = data + (elementCount);
	float* q = m.data + (elementCount);
	while (p > data) {
		*--p = *--q;
	}
}

//converts the rows into columns and vice-versa
inline Matrix Matrix::Transpose() {
	Matrix newMatrix(cols, rows);
	for (int i = 0; i < elementCount; ++i) {
		newMatrix.data[i] = data[cols * (i%rows) + (i/rows)];
	}
	return newMatrix;
}

//returns a matrix only with the specified rows and cols
//inline Matrix Matrix::Submatrix(std::initializer_list<float> inRows, std::initializer_list<float> inCols) {
//	Matrix newMatrix(inRows.size(), inCols.size());
//	for (int i = 0; i < newMatrix.elementCount; ++i) {
//		newMatrix.data[i]
//	}
//	return newMatrix;
//}