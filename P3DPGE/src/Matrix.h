#pragma once
#include "Debug.h"

struct Matrix {
	uint32 rows;
	uint32 cols;
	uint32 elementCount; //this might be convinient to have, but it increases size by 8 bytes?
	float* data;

	Matrix(uint32 inRows, uint32 inCols);
	Matrix(uint32 inRows, uint32 inCols, float inData[]);
	Matrix(uint32 inRows, uint32 inCols, std::initializer_list<float> list);
	Matrix(const Matrix& m);
	~Matrix();

	float&	operator () (uint32 row, uint32 col);
	Matrix&	operator =	(const Matrix& rhs);
	Matrix  operator *  (const float& rhs) const;
	Matrix& operator *= (const float& rhs);
	Matrix  operator /  (const float& rhs) const;
	Matrix& operator /= (const float& rhs);
	Matrix  operator +  (const Matrix& rhs) const;
	Matrix& operator += (const Matrix& rhs);
	Matrix  operator -  (const Matrix& rhs) const;
	Matrix& operator -= (const Matrix& rhs);
	Matrix  operator *  (const Matrix& rhs) const;
	Matrix& operator *= (const Matrix& rhs);
	Matrix  operator ^  (const Matrix& rhs) const;
	Matrix& operator ^= (const Matrix& rhs);
	Matrix  operator %  (const Matrix& rhs) const; 
	Matrix& operator %= (const Matrix& rhs);
	bool	operator == (const Matrix& rhs) const;
	bool	operator != (const Matrix& rhs) const;

	void copy(const Matrix& m);
	const std::string str() const;
	const std::string str2f() const;
};

//// Constructors ////

inline Matrix::Matrix(uint32 inRows, uint32 inCols) : rows(inRows), cols(inCols) {
	ASSERT(inRows != 0 && inCols != 0, "Matrix constructor was given zero size");
	this->elementCount = inRows * inCols;
	this->data = new float[elementCount];
}

inline Matrix::Matrix(uint32 inRows, uint32 inCols, float inData[]) : rows(inRows), cols(inCols) {
	ASSERT(inRows != 0 && inCols != 0, "Matrix constructor was given zero size");
	this->elementCount = inRows * inCols;
	uint32 inCount = sizeof(*inData) / sizeof(float);
	ASSERT(inCount <= elementCount, "Matrix constructor was given too many elements for given dimensions");
	this->data = new float[elementCount];
	for (uint32 i = 0; i < inCount; ++i) {
		*(this->data+i) = inData[i];
	}

}

inline Matrix::Matrix(uint32 inRows, uint32 inCols, std::initializer_list<float> list) : rows(inRows), cols(inCols) {
	ASSERT(inRows != 0 && inCols != 0, "Matrix constructor was given zero size");
	this->elementCount = inRows * inCols;
	uint32 inCount = list.size();
	ASSERT(inCount <= elementCount, "Matrix constructor was given too many elements for given dimensions");
	this->data = new float[elementCount];
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
	this->data = new float[elementCount];
	copy(m);
}

inline Matrix::~Matrix() {
	delete data;
}

//// Operators ////

inline float& Matrix::operator () (uint32 row, uint32 col) {
	ASSERT(row <= rows && col <= cols, "Matrix subscript out of bounds");
	return data[cols*row + col];
}

inline Matrix& Matrix::operator =  (const Matrix& rhs) {
	if (this->data != rhs.data) {
		this->rows = rhs.rows;
		this->cols = rhs.cols;
		this->elementCount = rows * cols;
		if (data) { delete[] data; }
		this->data = new float[elementCount];
		copy(rhs);
	}
	return *this;
}


inline Matrix  Matrix::operator *  (const float& rhs) const {
	Matrix newMatrix(*this);
	for (uint32 i = 0; i < newMatrix.elementCount; ++i) {
		*(newMatrix.data+i) *= rhs;
	}
	return newMatrix;
}

inline Matrix& Matrix::operator *= (const float& rhs) {
	for (uint32 i = 0; i < elementCount; ++i) {
		*(this->data+i) *= rhs;
	}
}

inline Matrix  Matrix::operator /  (const float& rhs) const {
	Matrix newMatrix(*this);
	for (uint32 i = 0; i < newMatrix.elementCount; ++i) {
		*(newMatrix.data+i) /= rhs;
	}
	return newMatrix;
}

inline Matrix& Matrix::operator /= (const float& rhs){
	for (uint32 i = 0; i < sizeof(*data) / sizeof(float); ++i) {
		*(this->data+i) /= rhs;
	}
}

inline Matrix  Matrix::operator +  (const Matrix& rhs) const{}

inline Matrix& Matrix::operator += (const Matrix& rhs){}

inline Matrix  Matrix::operator -  (const Matrix& rhs) const{}

inline Matrix& Matrix::operator -= (const Matrix& rhs){}

//NOTE sushi: you will probably want to throw an exception if you attempt to 
//			  do an invalid matrix multiplication eg. 
//			  when you have matrix axb and matrix cxd, if b is not equal to c
//			  throw exception

inline Matrix  Matrix::operator *  (const Matrix& rhs) const{}

inline Matrix& Matrix::operator *= (const Matrix& rhs){}

//element-wise multiplication
inline Matrix  Matrix::operator ^  (const Matrix& rhs) const{} 

//element-wise multiplication and assignment
inline Matrix& Matrix::operator ^= (const Matrix& rhs){} 

//element-wise division
inline Matrix  Matrix::operator %  (const Matrix& rhs) const{} 

//element-wise division and assignment
inline Matrix& Matrix::operator %= (const Matrix& rhs){}

inline bool Matrix::operator			== (const Matrix& rhs) const { return false; }

inline bool Matrix::operator			!= (const Matrix& rhs) const { return false; }

//// Functions ////

//copys the data from one matrix to the other
//REQUIRES both to have the same dimensions
inline void Matrix::copy(const Matrix& m) {
	ASSERT(rows == m.rows && cols == m.cols, "Cant copy matrices of unequal dimensions");
	float* p = data + (elementCount);
	float* q = m.data + (elementCount);
	while (p > data) {
		*--p = *--q;
	}
}
