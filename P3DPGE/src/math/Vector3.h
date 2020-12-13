#pragma once
#include "../Debug.h"

struct MatrixN;
struct Matrix3;
struct Matrix4;
struct Vector4;

namespace olc {
	template<typename V>
	struct v2d_generic;
}
#define Vector2 olc::vf2d

struct Vector3 {
	float x;
	float y;
	float z;

	Vector3();
	Vector3(float inX, float inY, float inZ);
	Vector3(float inX, float inY);
	Vector3(const Vector3& v);

	void    operator =	(const Vector3& rhs);
	Vector3 operator *  (const float& rhs) const;
	void    operator *= (const float& rhs);
	Vector3 operator /  (const float& rhs) const;
	void    operator /= (const float& rhs);
	Vector3 operator +  (const Vector3& rhs) const;
	void    operator += (const Vector3& rhs);
	Vector3 operator -  (const Vector3& rhs) const;
	void    operator -= (const Vector3& rhs);
	Vector3 operator *  (const Vector3& rhs) const;
	void    operator *= (const Vector3& rhs);
	Vector3 operator /  (const Vector3& rhs) const;
	void    operator /= (const Vector3& rhs);
	Vector3 operator -  () const;
	bool    operator == (const Vector3& rhs) const;
	bool    operator != (const Vector3& rhs) const;
	friend Vector3 operator * (const float& lhs, const Vector3& rhs) { return   rhs * lhs; }

	const std::string str() const;
	const std::string str2f() const;
	Vector3 copy() const;
	float	dot(const Vector3& rhs) const;
	Vector3 cross(const Vector3& rhs) const;
	float	mag() const;
	void	normalize();
	Vector3	normalized() const;
	Vector3 clampMag(const float& rhs) const;
	float	distanceTo(const Vector3& rhs) const;
	float	projectOn(Vector3& rhs) const;
	Vector3 componentOn(Vector3& rhs) const;
	Vector3	xComp() const;
	Vector3 yComp() const;
	Vector3 zComp() const;
	Vector3 xInvert() const;
	Vector3 yInvert() const;
	Vector3 zInvert() const;

	void LocalToWorld(Vector3 pos);
	void WorldToLocal(Vector3 pos);
	void ScreenToWorld(Matrix4 ProjMat, Matrix4 view);

	//Non-Vector vs Vector interactions defined in Math.h
	Vector3(const Vector2& v);

	Vector3 operator *  (const Matrix3& rhs) const;
	void    operator *= (const Matrix3& rhs);
	Vector3 operator *  (const Matrix4& rhs) const;
	void    operator *= (const Matrix4& rhs);

	Vector4 ToVector4() const;
	Vector2 toVector2() const; 
	MatrixN ToM1x3() const;
	MatrixN ToM1x4(float w) const;
	Vector3 ProjectionMultiply(Matrix4 projection) const;
};

//// Constructors ////

inline Vector3::Vector3() {
	this->x = 0; this->y = 0; this->z = 0;
}

inline Vector3::Vector3(float inX, float inY, float inZ) {
	this->x = inX; this->y = inY; this->z = inZ;
}

inline Vector3::Vector3(float inX, float inY) {
	this->x = inX; this->y = inY; this->z = 0;
}

inline Vector3::Vector3(const Vector3& v) {
	this->x = v.x; this->y = v.y; this->z = v.z;
}

//// Operators ////

inline void    Vector3::operator =	(const Vector3& rhs) {
	this->x = rhs.x; this->y = rhs.y; this->z = rhs.z;
}

inline Vector3 Vector3::operator *  (const float& rhs) const {
	return Vector3(this->x * rhs, this->y * rhs, this->z * rhs);
}

inline void    Vector3::operator *= (const float& rhs) {
	this->x *= rhs; this->y *= rhs; this->z *= rhs;
}

inline Vector3 Vector3::operator /  (const float& rhs) const {
	return Vector3(this->x / rhs, this->y / rhs, this->z / rhs);
}

inline void    Vector3::operator /= (const float& rhs) {
	this->x /= rhs; this->y /= rhs; this->z /= rhs;
}

inline Vector3 Vector3::operator +  (const Vector3& rhs) const {
	return Vector3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
}

inline void    Vector3::operator += (const Vector3& rhs) {
	this->x += rhs.x; this->y += rhs.y; this->z += rhs.z;
}

inline Vector3 Vector3::operator -  (const Vector3& rhs) const {
	return Vector3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
}

inline void    Vector3::operator -= (const Vector3& rhs) {
	this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z;
}

inline Vector3 Vector3::operator *  (const Vector3& rhs) const {
	return Vector3(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z);
}

inline void    Vector3::operator *= (const Vector3& rhs) {
	this->x *= rhs.x; this->y *= rhs.y; this->z *= rhs.z;
}

inline Vector3 Vector3::operator /  (const Vector3& rhs) const {
	return Vector3(this->x / rhs.x, this->y / rhs.y, this->z / rhs.z);
}

inline void    Vector3::operator /= (const Vector3& rhs) {
	this->x /= rhs.x; this->y /= rhs.y; this->z /= rhs.z;
}

inline Vector3 Vector3::operator -  () const {
	return Vector3( -x, -y, -z );
}

inline bool    Vector3::operator == (const Vector3& rhs) const {
	return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
}

inline bool    Vector3::operator != (const Vector3& rhs) const {
	return this->x != rhs.x || this->y != rhs.y || this->z != rhs.z;
}

//// Functions ////

inline const std::string Vector3::str() const {
	return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + "," + std::to_string(this->z) + ")";
}

inline const std::string Vector3::str2f() const {
	char buffer[50];
	std::snprintf(buffer, 50, "(%+.2f, %+.2f, %+.2f)", this->x, this->y, this->z);
	return std::string(buffer);
}

inline Vector3 Vector3::copy() const {
	return Vector3(x, y, z);
}

inline float Vector3::dot(const Vector3& rhs) const {
	return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
}

inline Vector3 Vector3::cross(const Vector3& rhs) const {
	return Vector3(this->y * rhs.z - rhs.y * this->z, this->x * rhs.z - rhs.x * this->z, this->x * rhs.y - rhs.x * this->y);
}

inline float Vector3::mag() const {
	return std::sqrtf(x * x + y * y + z * z);
}

inline void Vector3::normalize() {
	if (*this != Vector3(0, 0, 0)) {
		*this /= this->mag();
	}
}

inline Vector3 Vector3::normalized() const {
	if (*this != Vector3(0, 0, 0)) {
		return *this / this->mag();
	}
	return Vector3(*this);
}

inline Vector3 Vector3::clampMag(const float& rhs) const {
	if (this->mag() < rhs) { return *this; }
	return this->normalized() * rhs;
}

inline float Vector3::distanceTo(const Vector3& rhs) const {
	return (*this - rhs).mag();
}

inline float Vector3::projectOn(Vector3& rhs) const {
	return this->dot(rhs.normalized());
}

inline Vector3 Vector3::componentOn(Vector3& rhs) const {
	Vector3 rhsNormalized = rhs.normalized();
	return rhsNormalized * this->dot(rhsNormalized);
}

inline Vector3 Vector3::xComp() const {
	return Vector3(x, 0, 0);
}

inline Vector3 Vector3::yComp() const {
	return Vector3(0, y, 0);
}

inline Vector3 Vector3::zComp() const {
	return Vector3(0, 0, z);
}

inline Vector3 Vector3::xInvert() const {
	return Vector3(-x, y, z);
}

inline Vector3 Vector3::yInvert() const {
	return Vector3(x, -y, z);
}

inline Vector3 Vector3::zInvert() const {
	return Vector3(x, y, -z);
}