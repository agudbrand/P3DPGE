#pragma once
#include "HandmadeMath.h"
#include <boost/qvm/mat.hpp>
#include <boost/qvm/vec.hpp>
#include <boost/qvm/map_vec_mat.hpp>
#include <boost/qvm/mat_operations.hpp>
#include <math.h>
#include <iostream>
#include <string>

//math constants
#define M_PI HMM_PI32
#define M_E 2.71828f

//vector constants
#define V3NULL	Vector3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min())
#define V3ZERO	Vector3(0, 0, 0)
#define V3ONE	Vector3(1, 1, 1)
#define V3XU	Vector3(1, 0, 0)
#define V3YU	Vector3(0, 1, 0)
#define V3ZU	Vector3(0, 0, 1)

//physics constants
#define GRAVITY 9.81f

using namespace boost::qvm;

namespace Math {

	static float to_radians(float angle) { return angle * (M_PI / 180); }
	static float to_degrees(float angle) { return angle * (180 / M_PI); }

}


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

		Vector3 operator +  (const Vector3& rhs)		const { return Vector3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z); }
		Vector3 operator -  (const Vector3& rhs)		const { return Vector3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z); }
		Vector3 operator *  (const float& rhs)			const { return Vector3(this->x * rhs, this->y * rhs, this->z * rhs); }
		Vector3 operator *  (const Vector3& rhs)		const { return Vector3(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z); }
		Vector3 operator /  (const float& rhs)			const { return Vector3(this->x / rhs, this->y / rhs, this->z / rhs); }
		Vector3 operator /  (const Vector3& rhs)		const { return Vector3(this->x / rhs.x, this->y / rhs.y, this->z / rhs.z); }
		Vector3 operator += (const Vector3& rhs)		{ this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; return *this; }
		Vector3 operator -= (const Vector3& rhs)		{ this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; return *this; }
		Vector3 operator *= (const float& rhs)			{ this->x *= rhs; this->y *= rhs; this->z *= rhs; return *this; }
		Vector3 operator /= (const float& rhs)			{ this->x /= rhs; this->y /= rhs; this->z /= rhs; return *this; }
		Vector3 operator +  ()							const { return { +x, +y, +z }; }
		Vector3 operator -  ()							const { return { -x, -y, -z }; }
		bool operator	 == (const Vector3& rhs)		const { return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z); }
		bool operator	 != (const Vector3& rhs)		const { return (this->x != rhs.x || this->y != rhs.y || this->z != rhs.z); }
	
		float				dot(const Vector3& rhs)		const { return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z; }
		Vector3				cross(const Vector3& rhs)	const { return Vector3(this->y * rhs.z - rhs.z * this->z, this->x * rhs.z - rhs.x * this->z, this->x * rhs.y - rhs.x * this->y); }
		float				mag()						const { return std::sqrt(x * x + y * y + z * z); }
		const std::string	str()						const { return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + "," + std::to_string(this->z) + ")"; }
		Vector3				normalized()				{ return *this == V3ZERO ? V3ZERO : *this / this->mag(); }
		Vector3				clampMag(float& rhs)		{ return this->normalized() * rhs; }
		float				distanceTo(Vector3& rhs)	{ return std::abs((*this - rhs).mag()); }
		float				projectOn(Vector3& rhs)		{ return this->dot(rhs.normalized()); }
		Vector3				componentOn(Vector3& rhs)	{ return rhs.normalized() * this->dot(rhs.normalized()) ; }
		Vector3				xComp()						{ return Vector3(x, 0, 0); }
		Vector3				yComp()						{ return Vector3(0, y, 0); }
		Vector3				zComp()						{ return Vector3(0, 0, z); }

		//TODO: perpendicular

		//conversions between qvm's matrices and our vectors
		mat<float,1,4> ConvertToM4x4() {
			mat<float, 1, 4> m;
			m.a[0][0] = x; m.a[0][1] = y; m.a[0][2] = z; m.a[0][3] = 1;
			return m;
		}

		void M1x4ToVector3(boost::qvm::mat<float,1,4> m) {
			
			x = m.a[0][0]; y = m.a[0][1]; z = m.a[0][2];
			
		}

		// Functions pertaining to matrix-vertex manipulation //

		//translate vector with given translation
		void translateV3(Vector3 translation) {
			mat<float, 1, 4> v { x, y, z, 1 };
			mat<float, 4, 4> tv {
				1,			   0,			  0,			 0,
				0,			   1,			  0,			 0,
				0,			   0,			  1,			 0,
				translation.x, translation.y, translation.z, 1
			};
			this->M1x4ToVector3(v * tv);
		}

		//covert point to WorldSpace
		mat<float, 4, 4> LocalToWorld(Vector3 pos) {
			mat<float, 4, 4> wtl{
				1,	   0,	  0,	 0,
				0,	   1,	  0,	 0,
				0,	   0,	  1,	 0,
				pos.x, pos.y, pos.z, 1
			};
			return wtl;
		}

		//convert point to LocalSpace
		mat<float, 4, 4> WorldToLocal(Vector3 pos) {
			mat<float, 4, 4> ltw{
				1,	   0,	  0,	 0,
				0,	   1,	  0,	 0,
				0,	   0,	  1,	 0,
				pos.x, pos.y, pos.z, 1
			};
			return inverse(ltw);
		}

		//basic euler rotations
		void rotateV3_X(float theta, Vector3 pos) {
			theta = Math::to_radians(theta);
			mat<float, 1, 4> v { x, y, z, 1 };
			mat<float, 1, 4> local_v = v * WorldToLocal(pos);
			mat<float, 4, 4> rvx {
				1,		0,			0,           0,
				0,		cos(theta),	-sin(theta), 0,
				0,		sin(theta),	cos(theta),  0,
				0,		0,			0,			 1
			};
			this->M1x4ToVector3(local_v * rvx * LocalToWorld(pos));
		}

		void rotateV3_Y(float theta, Vector3 pos) {
			theta = Math::to_radians(theta);
			mat<float, 1, 4> v{ x, y, z, 1 };
			mat<float, 1, 4> local_v = v * WorldToLocal(pos);
			mat<float, 4, 4> rvx{
				cos(theta),	0,	sin(theta),  0,
				0,			1,	0,			 0,
				-sin(theta),0,	cos(theta),  0,
				0,			0,	0,			 1
			};
			this->M1x4ToVector3(local_v * rvx * LocalToWorld(pos));
		}

		void rotateV3_Z(float theta, Vector3 pos) {
			theta = Math::to_radians(theta);
			mat<float, 1, 4> v { x, y, z, 1 };
			mat<float, 1, 4> local_v = v * WorldToLocal(pos);
			mat<float, 4, 4> rvz { 
				cos(theta), -sin(theta),	0, 0,
				sin(theta), cos(theta),		0, 0,
				0,			0,				1, 0,
				0,			0,				0, 1
			};
			this->M1x4ToVector3(local_v * rvz * LocalToWorld(pos));
		}

		//TODO: figure out why this isn't projecting properly
		//temporary fix: press P
		void ProjToScreen(mat<float,4,4> ProjMat, olc::PixelGameEngine* p, Vector3 pos) {
			mat<float, 1, 4> v{ x, y, z, 1 };
			this->M1x4ToVector3(v * ProjMat);
		}
};
