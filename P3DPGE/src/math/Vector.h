#pragma once
#include "../Debug.h"


struct Matrix;
namespace olc {
	template<typename V>
	struct v2d_generic;
}
#define Vector2 olc::vf2d

namespace boost {
	namespace qvm {
		template<typename M, int, int>
		class mat;

		template<typename V, int>
		class vec;
	}
}
#define mat boost::qvm::mat
#define vec boost::qvm::vec

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
	float	distanceTo(Vector3& rhs) const;
	float	projectOn(Vector3& rhs) const;
	Vector3 componentOn(Vector3& rhs) const;
	Vector3	xComp() const;
	Vector3 yComp() const;
	Vector3 zComp() const;
	Vector3 xInvert() const; //TODO(delle) rename this to negatedX
	Vector3 yInvert() const; //TODO(delle) rename this to negatedY
	Vector3 zInvert() const; //TODO(delle) rename this to negatedZ

	//Non-Vector vs Vector interactions //TODO(delle) define these in Math.h
	Vector3(const Vector2& vector2);
	Vector3(Matrix matrix);

	void    operator =	(const Matrix& rhs);
	Vector3 operator *  (const Matrix& rhs) const;
	void    operator *= (const Matrix& rhs);
	bool    operator == (const Matrix& rhs) const;
	bool    operator != (const Matrix& rhs) const;

	Vector2 toVector2() const; 
	Matrix ToM1x3() const;
	Matrix ToM1x4(float w) const;




	//qvm stuff to convert
	Vector3(mat<float, 1, 4> m);
	Vector3(vec<float, 3> vector);

	void    operator =	(const mat<float, 1, 4>& rhs);
	Vector3 operator *  (const mat<float, 4, 4> rhs);
	Vector3 operator *= (const mat<float, 4, 4> rhs);

	mat<float, 1, 4> ConvertToM1x4();
	vec<float, 3> ConvertToVec3();
	void M1x4ToVector3(mat<float, 1, 4> m);
	Vector3 GetM1x4ToVector3(mat<float, 1, 4> m);
	mat<float, 1, 4> proj_mult(mat<float, 1, 4> v, mat<float, 4, 4> m);
	mat<float, 1, 4> proj_mult(mat<float, 1, 4> v, mat<float, 4, 4> m, float& w);
	mat<float, 1, 4> unproj_mult(mat<float, 1, 4> v, mat<float, 4, 4> m);
	void translateV3(Vector3 translation);
	void scaleV3(Vector3 scale);
	void LocalToWorld(Vector3 pos);
	void WorldToLocal(Vector3 pos);
	void rotateV3_X(float theta, Vector3 pos, Vector3 offset);
	void rotateV3_Y(float theta, Vector3 pos, Vector3 offset);
	void rotateV3_Z(float theta, Vector3 pos, Vector3 offset);
	void ProjToScreen(mat<float, 4, 4> ProjMat, olc::PixelGameEngine* p);
	void ProjToScreen(mat<float, 4, 4> ProjMat, olc::PixelGameEngine* p, float& w);
	void ScreenToWorld(mat<float, 4, 4> ProjMat, mat<float, 4, 4> view, olc::PixelGameEngine* p);
};

struct Vector4 : public Vector3 {
	float w;
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
	return { -x, -y, -z }; //TODO(delle) test this
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
	std::snprintf(buffer, 50, "(%-.2f, %-.2f, %-.2f)", this->x, this->y, this->z);
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
	if (*this != Vector3(0,0,0)) {
		*this /= this->mag();
	}
}

inline Vector3 Vector3::normalized() const {
	if (*this != Vector3(0, 0, 0)) {
		return *this / this->mag();
	}
	return *this; //TODO(delle) test this
}

inline Vector3 Vector3::clampMag(const float& rhs) const {
	return this->normalized() * rhs;
}

inline float Vector3::distanceTo(Vector3& rhs) const {
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