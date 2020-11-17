#pragma once
//#include "HandmadeMath.h"
#include "olcPixelGameEngine.h"
#include <boost/qvm/mat.hpp>
#include <boost/qvm/vec.hpp>
#include <boost/qvm/map_vec_mat.hpp>
#include <boost/qvm/mat_operations.hpp>
#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>

//math constants
#define M_PI 3.14159265359f
#define M_E 2.71828f

//vector constants
#define V3NULL	Vector3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min())
#define V3ZERO	Vector3()
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
		Vector3 operator *  (const mat<float, 4, 4> rhs)      { mat<float, 1, 4> v{ x,y,z,1 }; return GetM1x4ToVector3(v * rhs); }
		Vector3 operator /  (const float& rhs)			const { return Vector3(this->x / rhs, this->y / rhs, this->z / rhs); }
		Vector3 operator /  (const Vector3& rhs)		const { return Vector3(this->x / rhs.x, this->y / rhs.y, this->z / rhs.z); }
		Vector3 operator += (const Vector3& rhs)			  { this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; return *this; }
		Vector3 operator -= (const Vector3& rhs)			  { this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; return *this; }
		Vector3 operator *= (const float& rhs)				  { this->x *= rhs; this->y *= rhs; this->z *= rhs; return *this; }
		Vector3 operator /= (const float& rhs)				  { this->x /= rhs; this->y /= rhs; this->z /= rhs; return *this; }
		Vector3 operator +  ()							const { return { +x, +y, +z }; }
		Vector3 operator -  ()							const { return { -x, -y, -z }; }
		bool operator	 == (const Vector3& rhs)		const { return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z); }
		bool operator	 != (const Vector3& rhs)		const { return (this->x != rhs.x || this->y != rhs.y || this->z != rhs.z); }
	
		float				dot(const Vector3& rhs)		const { return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z; }
		Vector3				cross(const Vector3& rhs)	const { return Vector3(this->y * rhs.z - rhs.y * this->z, this->x * rhs.z - rhs.x * this->z, this->x * rhs.y - rhs.x * this->y); }
		float				mag()						const { return std::sqrtf(x * x + y * y + z * z); }
		const std::string	str()						const { return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + "," + std::to_string(this->z) + ")"; }
		Vector3				normalized()					  { return *this == V3ZERO ? V3ZERO : *this / this->mag(); }
		void				normalize()						  { *this == V3ZERO ? *this = V3ZERO : *this = *this / this->mag(); }
		Vector3				clampMag(float& rhs)			  { return this->normalized() * rhs; }
		float				distanceTo(Vector3& rhs)		  { return (*this - rhs).mag(); }
		float				projectOn(Vector3& rhs)			  { return this->dot(rhs.normalized()); }
		Vector3				componentOn(Vector3& rhs)		  { return rhs.normalized() * this->dot(rhs.normalized()) ; }
		Vector3				xComp()							  { return Vector3(x, 0, 0); }
		Vector3				yComp()							  { return Vector3(0, y, 0); }
		Vector3				zComp()							  { return Vector3(0, 0, z); }
		Vector3				xInvert()						  { return Vector3(-x,  y,  z); }
		Vector3				yInvert()						  { return Vector3( x, -y,  z); }
		Vector3				zInvert()						  { return Vector3( x,  y, -z); }
		olc::vd2d			Vector3Tovd2d()					  { return olc::vd2d(x, y); }
		
		//Vector3				invertedComponents()		{ return Vector3(1/x, 1/y, 1/z); }

		//conversions between qvm's matrices and our vectors and a special mult function
		mat<float,1,4> ConvertToM4x4() {
			mat<float, 1, 4> m;
			m.a[0][0] = x; m.a[0][1] = y; m.a[0][2] = z; m.a[0][3] = 1;
			return m;
		}

		void M1x4ToVector3(mat<float,1,4> m) {
			x = m.a[0][0]; y = m.a[0][1]; z = m.a[0][2];
		}

		Vector3 GetM1x4ToVector3(mat<float, 1, 4> m) {
			x = m.a[0][0]; y = m.a[0][1]; z = m.a[0][2];
			return Vector3(m.a[0][0], m.a[0][1], m.a[0][2]);
		}

		mat<float, 1, 4> proj_mult(mat<float, 1, 4> v, mat<float, 4, 4> m) {
			mat<float, 1, 4> vm = v * m;
			if (vm.a[0][3] != 0) { vm.a[0][0] /= vm.a[0][3]; vm.a[0][1] /= vm.a[0][3]; vm.a[0][2] /= vm.a[0][3]; }
			return vm;
		}

		//// Functions pertaining to matrix-vertex manipulation ////

		//translate vector with given translation
		void translateV3(Vector3 translation) {
			mat<float, 4, 4> tv {
				1,			   0,			  0,			 0,
				0,			   1,			  0,			 0,
				0,			   0,			  1,			 0,
				translation.x, translation.y, translation.z, 1
			};
			this->M1x4ToVector3(proj_mult(ConvertToM4x4(), tv));
		}

		//scale object
		void scaleV3(Vector3 scale) {
			mat<float, 4, 4> sv{
				scale.x,0,		  0,	   0,
				0,		 scale.y, 0,	   0,
				0,		 0,		  scale.z, 0,
				0,		 0,		  0,	   1
			};
			this->M1x4ToVector3(proj_mult(ConvertToM4x4(), sv));
		}

		//covert point to WorldSpace
		void LocalToWorld(Vector3 pos) {
			mat<float, 4, 4> wtl{
				1,	   0,	  0,	 0,
				0,	   1,	  0,	 0,
				0,	   0,	  1,	 0,
				pos.x, pos.y, pos.z, 1
			};
			this->M1x4ToVector3(proj_mult(ConvertToM4x4(), wtl));
		}

		//convert point to LocalSpace
		void WorldToLocal(Vector3 pos) {
			mat<float, 4, 4> ltw{
				1,	   0,	  0,	 0,
				0,	   1,	  0,	 0,
				0,	   0,	  1,	 0,
				pos.x, pos.y, pos.z, 1
			};
			this->M1x4ToVector3(proj_mult(ConvertToM4x4(), inverse(ltw)));

		}

		//basic euler rotations locally
		void rotateV3_X(float theta, Vector3 pos, Vector3 offset) {
			theta = Math::to_radians(theta);
			WorldToLocal(pos + offset);
			mat<float, 4, 4> rvx{
				1,		0,			0,           0,
				0,		cos(theta),	-sin(theta), 0,
				0,		sin(theta),	cos(theta),  0,
				0,		0,			0,			 1
			};
			this->M1x4ToVector3(proj_mult(ConvertToM4x4(), rvx));
			LocalToWorld(pos + offset);
		}

		void rotateV3_Y(float theta, Vector3 pos, Vector3 offset) {
			theta = Math::to_radians(theta);
			WorldToLocal(pos + offset);
			mat<float, 4, 4> rvy{
				cos(theta),	0,	sin(theta),  0,
				0,			1,	0,			 0,
				-sin(theta),0,	cos(theta),  0,
				0,			0,	0,			 1
			};
			this->M1x4ToVector3(proj_mult(ConvertToM4x4(), rvy));
			LocalToWorld(pos + offset);
		}

		void rotateV3_Z(float theta, Vector3 pos, Vector3 offset) {
			theta = Math::to_radians(theta);
			WorldToLocal(pos + offset);
			mat<float, 4, 4> rvz{
				cos(theta), -sin(theta),	0, 0,
				sin(theta), cos(theta),		0, 0,
				0,			0,				1, 0,
				0,			0,				0, 1
			};
			this->M1x4ToVector3(proj_mult(ConvertToM4x4(), rvz));
			LocalToWorld(pos + offset);
		}

		

		//projects a mesh's points to the screen
		void ProjToScreen(mat<float, 4, 4> ProjMat, olc::PixelGameEngine* p, Vector3 pos) {
			this->M1x4ToVector3(proj_mult(ConvertToM4x4(), ProjMat));
			x += 1.0f; y += 1.0f;
			x *= 0.5f * (float)p->ScreenWidth();
			y *= 0.5f * (float)p->ScreenHeight();
		}
};



namespace Math {
	

	//for debugging with floats or doubles
	static std::string append_decimal(std::string s) {
		while (s.back() != '.') {
			s.pop_back();
		}
		s.pop_back();
		return s;
	}

	static Vector3 vi2dToVector3(olc::vi2d vector, float z = 0) {
		return Vector3((float)vector.x, (float)vector.y, z);
	}

	//this function returns a matrix that tells a vector how to look at a specific point in space.
	static mat<float, 4, 4> PointAt(Vector3& pos, Vector3& target, Vector3& up) {
		up.normalize();

		//get new forward direction
		Vector3 newFor = target - pos;
		newFor.normalize();

		//get right direction
		Vector3 newRight = up.cross(newFor);
		newRight.normalize();

		//get up direction
		Vector3 newUp = newRight.cross(newFor);
		newUp.normalize();

		//make point at matrix
		mat<float, 4, 4> m {
			newRight.x, newRight.y, newRight.z, 0,
			newUp.x,	newUp.y,	newUp.z,	0,
			newFor.x,	newFor.y,	newFor.z,	0,
			pos.x,		pos.y,		pos.z,		1
		};

		return m;
	}

	static float DistPointToPlane(Vector3 point, Vector3 plane, Vector3 plane_p) { return (plane.x * point.x + plane.y * point.y + plane.z * point.z - plane.dot(plane_p)); }

	//where a line intersects with a plane
	static Vector3 VectorPlaneIntersect(Vector3 plane_p, Vector3 plane_n, Vector3 line_start, Vector3 line_end) {
		plane_n.normalize();
		float plane_d = -plane_n.dot(plane_p);
		float ad = line_start.dot(plane_n);
		float bd = line_end.dot(plane_n);
		float t = (-plane_d - ad) / (bd - ad);
		Vector3 line_start_to_end = line_end - line_start;
		Vector3 line_to_intersect = line_start_to_end * t;
		return line_start + line_to_intersect;
	}
	
	//TODO(, sushi) rename these functions to something not retarded
	static mat<float, 4, 4> GetRotateV3_X(float theta) {
		theta = Math::to_radians(theta);
		mat<float, 4, 4> rvx{
			1,		0,			0,           0,
			0,		cos(theta),	-sin(theta), 0,
			0,		sin(theta),	cos(theta),  0,
			0,		0,			0,			 1
		};
		return rvx;
	}

	static mat<float, 4, 4> GetRotateV3_Y(float theta) {
		theta = Math::to_radians(theta);
		mat<float, 4, 4> rvy{
			cos(theta),	0,	sin(theta),  0,
			0,			1,	0,			 0,
			-sin(theta),0,	cos(theta),  0,
			0,			0,	0,			 1
		};
		return rvy;
	}

	static mat<float, 4, 4> GetRotateV3_Z(float theta) {
		theta = Math::to_radians(theta);
		mat<float, 4, 4> rvz{
			cos(theta), -sin(theta),	0, 0,
			sin(theta), cos(theta),		0, 0,
			0,			0,				1, 0,
			0,			0,				0, 1
		};
		return rvz;
	}
};
