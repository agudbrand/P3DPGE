#pragma once
#include <math.h>
#include <algorithm>
#include "Time.h"
#include "boost/qvm/mat.hpp"
#include "boost/qvm/vec.hpp"
#include "boost/qvm/map_vec_mat.hpp"
#include "boost/qvm/mat_operations.hpp"
#include "olcPixelGameEngine.h"

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
	
	Vector3(float x, float y) {
		this->x = x;
		this->y = y;
		this->z = 0;
	}
	
	Vector3 operator +  (const Vector3& rhs)		const { return Vector3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z); }
	Vector3 operator -  (const Vector3& rhs)		const { return Vector3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z); }
	Vector3 operator *  (const float& rhs)			const { return Vector3(this->x * rhs, this->y * rhs, this->z * rhs); }
	Vector3 operator *  (const Vector3& rhs)		const { return Vector3(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z); }
	Vector3 operator *  (const mat<float, 4, 4> rhs) { mat<float, 1, 4> v{ x,y,z,1 }; return GetM1x4ToVector3(v * rhs); }
	Vector3 operator /  (const float& rhs)			const { return Vector3(this->x / rhs, this->y / rhs, this->z / rhs); }
	Vector3 operator /  (const Vector3& rhs)		const { return Vector3(this->x / rhs.x, this->y / rhs.y, this->z / rhs.z); }
	Vector3 operator += (const Vector3& rhs) { this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; return *this; }
	Vector3 operator -= (const Vector3& rhs) { this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; return *this; }
	Vector3 operator *= (const mat<float, 4, 4> rhs) { mat<float, 1, 4> v{ x,y,z,1 }; return GetM1x4ToVector3(v * rhs); }
	Vector3 operator /= (const float& rhs) { this->x /= rhs; this->y /= rhs; this->z /= rhs; return *this; }
	Vector3 operator +  ()							const { return { +x, +y, +z }; }
	Vector3 operator -  ()							const { return { -x, -y, -z }; }
	bool operator	 == (const Vector3& rhs)		const { return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z); }
	bool operator	 != (const Vector3& rhs)		const { return (this->x != rhs.x || this->y != rhs.y || this->z != rhs.z); }
	
	float				dot(const Vector3& rhs)		const { return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z; }
	Vector3				cross(const Vector3& rhs)	const { return Vector3(this->y * rhs.z - rhs.y * this->z, this->x * rhs.z - rhs.x * this->z, this->x * rhs.y - rhs.x * this->y); }
	float				mag()						const { return std::sqrtf(x * x + y * y + z * z); }
	const std::string	str()						const { return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + "," + std::to_string(this->z) + ")"; }
	Vector3				normalized() { return *this == V3ZERO ? V3ZERO : *this / this->mag(); }
	void				normalize() { *this == V3ZERO ? *this = V3ZERO : *this = *this / this->mag(); }
	Vector3				clampMag(float& rhs) { return this->normalized() * rhs; }
	float				distanceTo(Vector3& rhs) { return (*this - rhs).mag(); }
	float				projectOn(Vector3& rhs) { return this->dot(rhs.normalized()); }
	Vector3				componentOn(Vector3& rhs) { return rhs.normalized() * this->dot(rhs.normalized()); }
	Vector3				xComp() { return Vector3(x, 0, 0); }
	Vector3				yComp() { return Vector3(0, y, 0); }
	Vector3				zComp() { return Vector3(0, 0, z); }
	Vector3				xInvert() { return Vector3(-x, y, z); }
	Vector3				yInvert() { return Vector3(x, -y, z); }
	Vector3				zInvert() { return Vector3(x, y, -z); }
	olc::vd2d			Vector3Tovd2d() { return olc::vd2d(x, y); }
	
	//conversions between qvm's matrices and our vectors and a special mult function
	mat<float, 1, 4> ConvertToM1x4() {
		mat<float, 1, 4> m;
		m.a[0][0] = x; m.a[0][1] = y; m.a[0][2] = z; m.a[0][3] = 1;
		return m;
	}
	
	void M1x4ToVector3(mat<float, 1, 4> m) {
		x = m.a[0][0]; y = m.a[0][1]; z = m.a[0][2];
	}
	
	Vector3 GetM1x4ToVector3(mat<float, 1, 4> m) {
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
		mat<float, 4, 4> tv{
			1,			   0,			  0,			 0,
			0,			   1,			  0,			 0,
			0,			   0,			  1,			 0,
			translation.x, translation.y, translation.z, 1
		};
		this->M1x4ToVector3(proj_mult(ConvertToM1x4(), tv));
	}
	
	//scale object
	void scaleV3(Vector3 scale) {
		mat<float, 4, 4> sv{
			scale.x,0,		  0,	   0,
			0,		 scale.y, 0,	   0,
			0,		 0,		  scale.z, 0,
			0,		 0,		  0,	   1
		};
		this->M1x4ToVector3(proj_mult(ConvertToM1x4(), sv));
	}
	
	//covert point to WorldSpace
	void LocalToWorld(Vector3 pos) {
		mat<float, 4, 4> wtl{
			1,	   0,	  0,	 0,
			0,	   1,	  0,	 0,
			0,	   0,	  1,	 0,
			pos.x, pos.y, pos.z, 1
		};
		this->M1x4ToVector3(proj_mult(ConvertToM1x4(), wtl));
	}
	
	//convert point to LocalSpace
	void WorldToLocal(Vector3 pos) {
		mat<float, 4, 4> ltw{
			1,	   0,	  0,	 0,
			0,	   1,	  0,	 0,
			0,	   0,	  1,	 0,
			pos.x, pos.y, pos.z, 1
		};
		this->M1x4ToVector3(proj_mult(ConvertToM1x4(), inverse(ltw)));
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
		this->M1x4ToVector3(proj_mult(ConvertToM1x4(), rvx));
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
		this->M1x4ToVector3(proj_mult(ConvertToM1x4(), rvy));
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
		this->M1x4ToVector3(proj_mult(ConvertToM1x4(), rvz));
		LocalToWorld(pos + offset);
	}
	
	//projects a mesh's points to the screen
	void ProjToScreen(mat<float, 4, 4> ProjMat, olc::PixelGameEngine* p) {
		this->M1x4ToVector3(proj_mult(ConvertToM1x4(), ProjMat));
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

	//conversions
	static Vector3 vi2dToVector3(olc::vi2d vector, float z = 0) {
		return Vector3((float)vector.x, (float)vector.y, z);
	}

	static Vector3 M1x4ToVector3(mat<float, 1, 4> m) {
		return Vector3(m.a[0][0], m.a[0][1], m.a[0][2]);
	}

	static mat<float, 1, 4> Vector3ToM1x4(Vector3 v) {
		mat<float, 1, 4> m;
		m.a[0][0] = v.x; m.a[0][1] = v.y; m.a[0][2] = v.z; m.a[0][3] = 1;
		return m;
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
	
	//return where two lines intersect on the x axis with slope and the y-intercept
	static Vector3 LineIntersect(float slope1, float ycross1, float slope2, float ycross2) {
		mat<float, 2, 2> lhs{ slope1, ycross1, slope2, ycross2 };
		mat<float, 2, 1> rhs{ 1, 1 };
		mat<float, 2, 1> det = inverse(lhs) * rhs;
		float x = 1 / (det.a[0][1]) * det.a[0][0];
		float y = slope1 * x + ycross1;
		return Vector3(x, y, 0);
	}
};

//attached to entities to allow different forms of checking sides of more complex objects
//NOTE: there's probably a better way to update the edges for triangles
//		currently Triangle has a function that calls Edge's update function
//		and the edge update function might not even be necessary but idk yet
//		Triangle can also just not store edges and only have them when it needs them,
//		but I don't know how much we're gonna be using them yet
//TODO(m, sushi) implement Edge in 3D
struct Edge {
	Vector3 p[2];
	//if lead is true then p[1] is the right most point.
	//ditto for high but on y
	bool lead;
	bool high;
	
	Edge() { }
	Edge(Vector3 point1, Vector3 point2) {
		p[0] = point1;
		p[1] = point2;
		if (point1.x > point2.x) { lead = false; }
		else { lead = true; }
		if (point1.y > point2.y) { high = false; }
		else { high = true; }
	}
	
	//TODO(?, sushi) find a good place to update edge, probably in Triangle?
	void update(Vector3 point1, Vector3 point2) {
		p[0] = point1;
		p[1] = point2;
		if (point1.x > point2.x) { lead = false; }
		else { lead = true; }
		if (point1.y > point2.y) { high = false; }
		else { high = true; }
	}
	
	float slope() {
		if (p[1].x == p[0].x || p[1].y == p[0].y) { return 0; }
		else { return (p[1].y - p[0].y) / (p[1].x - p[0].x); }
	}
	
	//y intercept and range/domain checks
	float ycross() { return p[!lead].y - slope() * p[!lead].x; }
	bool within_range(Vector3 point)  { return (point.y < p[high].y&& point.y > p[!high].y); }
	bool within_range(float y_point)  { return (y_point < p[high].y&& y_point > p[!high].y); }
	bool within_domain(Vector3 point) { return (point.x < p[lead].x&& point.x > p[!lead].x); }
	bool within_domain(float x_point) { return (x_point < p[lead].x&& x_point > p[!lead].x); }
	
	//returns edge's normal
	//this only works for 2D and returns a normal rotated -90
	Vector3 edge_normal() {
		Vector3 v = p[1] - p[0];
		return Vector3(v.y, -v.x, 0).normalized();
	}

	Vector3 edge_midpoint() {
		return Vector3((p[0].x + p[1].x) / 2, (p[0].y + p[1].y) / 2, 0);
	}

	//is a point on, above, or below edge
	bool on_edge(Vector3 point) {
		if ((point.y == slope() * point.x + ycross()) && within_domain(point)) { return true; }
		else { return false; }
	}
	
	//these signs may look wrong but its to accomidate for the top left coord (maybe)
	bool above_edge(Vector3 point) {
		int bp = 0;
		if (point.y < slope() * point.x + ycross()) { return true; }
		else { return false; }
	}
	
	bool below_edge(Vector3 point) {
		if (point.y > slope() * point.x + ycross()) { return true; }
		else { return false; }
	}
	
	bool right_of_edge(Vector3 point) {
		if ((point.x > (point.y - ycross()) / slope())) { return true; }
		else { return false; }
	}
	
	bool left_of_edge(Vector3 point) {
		if ((point.x < (point.y - ycross()) / slope())) { return true; }
		else { return false; }
	}
	
	//checks if two edges intersect by finding their line representation's
	//intersection and then seeing if that point lies on either of them
	bool edge_intersect(Edge e) {
		if (slope() == e.slope() && (!on_edge(e.p[0]) || !on_edge(e.p[1]))) { return false; }
		Vector3 cross = Math::LineIntersect(slope(), ycross(), e.slope(), e.ycross());
		if (within_domain(cross) && within_range(cross) &&
			e.within_domain(cross) && e.within_range(cross)) {
			return true;
		}
		else {
			return false;
		}
	}
};

