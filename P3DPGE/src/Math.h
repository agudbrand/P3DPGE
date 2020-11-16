#pragma once
#include "HandmadeMath.h"
#include "olcPixelGameEngine.h"
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

	//for debugging with floats or doubles
	static std::string append_decimal(std::string s) {
		while (s.back() != '.') {
			s.pop_back();
		}
		s.pop_back();
		return s;
	}
};

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
		//could probably add more invert functions that do over 2 axes but that may be too much
		//more complex operator so its seperated
		
		//Vector3				invertedComponents()		{ return Vector3(1/x, 1/y, 1/z); }
		//?^

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

//collection of 3 points forming the basis of meshes
struct Triangle {
	Vector3 points[3];
	Vector3 projectedPoints[3];

	Triangle() {}
	Triangle(Vector3 p1, Vector3 p2, Vector3 p3) {
		points[0] = p1;
		points[1] = p2;
		points[2] = p3;
		copy_points();
	}

	void copy_points() {
		for (int p = 0; p < 3; p++) { projectedPoints[p] = points[p]; }
	}

	Vector3 get_normal() {
		Vector3 l1 = points[1] - points[0];
		Vector3 l2 = points[2] - points[0];
		return l2.cross(l1).normalized();
	}

	Vector3 get_proj_normal() {
		Vector3 l1 = projectedPoints[1] - projectedPoints[0];
		Vector3 l2 = projectedPoints[2] - projectedPoints[0];
		return l2.cross(l1).normalized();
	}
};

namespace Math {
	static Vector3 vi2dToVector3(olc::vi2d vector, float z = 0) {
		return Vector3((float)vector.x, (float)vector.y, z);
	}

	//this is a function from Javid's videos,
	//i do NOT know how this works but I plan to figure it out eventually
	//for now, using it is going to be a bit odd.
	//in essence, all it does is point a vector towards another vector and
	//therefor is necessary for the camera.
	static mat<float, 4, 4> PointAt(Vector3 pos, Vector3 target, Vector3 up) {
		//get new forward direction
		Vector3 newFor = target - pos;
		newFor = newFor.normalized();

		//get new up direction
		Vector3 a = newFor * up.dot(newFor);
		Vector3 newUp = up - a;
		newUp = newUp.normalized();

		//new right direction
		Vector3 newRight = newUp.cross(newFor);

		mat<float, 4, 4> m{
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

	//this function is really complex and i just pulled it from Javid's video
	//hopefully later i'll try to understand it better
	//TODO(+rs, sushi, 11/15/2020, Implement Clipping Algorithm) mesh Javid's clipping algorithm with what we already have set up, also rewatch his video to fix the camera not moving the clipping plane.
	static int ClipTriangles(Vector3 plane_p, Vector3 plane_n, Triangle in_tri, Triangle out_tri1, Triangle out_tri2) {
		plane_n.normalize();

		//temp storage to classify points on either side of plane
		Vector3* inside_points[3];  int nInsidePointCount = 0;
		Vector3* outside_points[3]; int nOutsidePointCount = 0;

		auto dist = [&](Vector3& p)
		{
			Vector3 n = p.normalized();
			return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dot(plane_p));
		};
		
		//signed distance of each point in triangle to plane
		float d0 = -dist(in_tri.projectedPoints[0]);
		float d1 = -dist(in_tri.projectedPoints[1]);
		float d2 = -dist(in_tri.projectedPoints[2]);

		if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.projectedPoints[0]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.projectedPoints[0]; }
		if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.projectedPoints[1]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.projectedPoints[1]; }
		if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.projectedPoints[2]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.projectedPoints[2]; }

		//classify points and break input triangle into smaller trangles if
		//required. there are four possible outcomes

		//all points lie outside the plane
		if (nInsidePointCount == 0) { return 0; }
		//all points lie inside the plane so do nothing and allow triangle to pass
		if (nInsidePointCount == 3) { out_tri1 = in_tri; return 1; }
		if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
			//the inside point is valid so we keep it
			out_tri1.projectedPoints[0] = *inside_points[0];

			//but the two new points are not where the original triangle intersects with the plane
			out_tri1.projectedPoints[1] = VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[0]);
			out_tri1.projectedPoints[2] = VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[1]);

			return 1; //return new triangle
		}
		if (nInsidePointCount == 2 && nOutsidePointCount == 1) {
			//triangle will be clipped and becomes a quad which is 
			//cut into two more triagles.

			out_tri1.projectedPoints[0] = *inside_points[0];
			out_tri1.projectedPoints[1] = *inside_points[1];
			out_tri1.projectedPoints[2] = VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[0]);

			out_tri2.projectedPoints[0] = *inside_points[1];
			out_tri2.projectedPoints[1] = out_tri1.projectedPoints[2];
			out_tri2.projectedPoints[2] = VectorPlaneIntersect(plane_p, plane_n, *inside_points[1], *outside_points[0]);
			return 2;

		}

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
