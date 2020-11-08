#pragma once
#include "HandmadeMath.h"
#include <math.h>
#include <iostream>
#include <string>

//math constants
#define M_PI HMM_PI32
#define M_E 2.71828f

//vector constants
#define V3ZERO	Vector3(0, 0, 0)
#define V3ONE	Vector3(1, 1, 1)
#define V3XU	Vector3(1, 0, 0)
#define V3YU	Vector3(0, 1, 0)
#define V3ZU	Vector3(0, 0, 1)

//physics constants
#define GRAVITY 9.81f

class Vector3 {
	public:
		float x;
		float y;
		float z;

		Vector3() {
			this->x = 0;
			this->y = 0;
			this->z = 0;
		}

		Vector3(float x, float y, float z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}

		Vector3 operator +  (const Vector3& rhs)	const { return Vector3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z); }
		Vector3 operator -  (const Vector3& rhs)	const { return Vector3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z); }
		Vector3 operator *  (const float& rhs)		const { return Vector3(this->x * rhs, this->y * rhs, this->z * rhs); }
		Vector3 operator *  (const Vector3& rhs)	const { return Vector3(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z); }
		Vector3 operator /  (const float& rhs)		const { return Vector3(this->x / rhs, this->y / rhs, this->z / rhs); }
		Vector3 operator /  (const Vector3& rhs)	const { return Vector3(this->x / rhs.x, this->y / rhs.y, this->z / rhs.z); }
		Vector3 operator += (const Vector3& rhs)	{ this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; return *this; }
		Vector3 operator -= (const Vector3& rhs)	{ this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; return *this; }
		Vector3 operator *= (const float& rhs)		{ this->x *= rhs; this->y *= rhs; this->z *= rhs; return *this; }
		Vector3 operator /= (const float& rhs)		{ this->x /= rhs; this->y /= rhs; this->z /= rhs; return *this; }
		Vector3 operator +  ()					    const { return { +x, +y, +z }; }
		Vector3 operator -  ()					    const { return { -x, -y, -z }; }
		bool operator	 == (const Vector3& rhs)    const { return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z); }
		bool operator	 != (const Vector3& rhs)    const { return (this->x != rhs.x || this->y != rhs.y || this->z != rhs.z); }
	
		float		dot(const Vector3& rhs)   const { return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z; }
		Vector3		cross(const Vector3& rhs) const { return Vector3(this->y * rhs.z - rhs.z * this->z, this->x * rhs.z - rhs.x * this->z, this->x * rhs.y - rhs.x * this->y); }
		float		mag() const { return std::sqrt(x * x + y * y + z * z); }
		const std::string str() const { return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + ")"; }
		Vector3 normalized() { return *this / this->mag(); }
		Vector3 clampMag(float& rhs) { return this->normalized() * rhs; }
		float distanceTo(Vector3& rhs) { return std::abs((*this - rhs).mag()); }
		float projectOn(Vector3& rhs) { return this->dot(rhs.normalized()); }
		Vector3 componentOn(Vector3& rhs) { return rhs.normalized() * this->dot(rhs.normalized()) ; }
		//TODO: perpendicular

};

//TODO: maybe make this
/*class Matrix4 {
	public:

		float mat[4][4];

		Matrix4() { 
			mat[4][4] = { 0 }; 
		}

		Matrix4 operator +  (const Matrix4& rhs) const {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {

				}
			}
		}

};*/
