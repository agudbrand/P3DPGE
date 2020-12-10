#pragma once
#include "Time.h"
#include "Vector.h"
#include "Matrix.h"

#include <math.h>
#include <algorithm>
#include <numeric>

#include "boost/qvm/mat_access.hpp"
#include "boost/qvm/mat.hpp"
#include "boost/qvm/vec.hpp"
#include "boost/qvm/map_vec_mat.hpp"
#include "boost/qvm/map_mat_mat.hpp"
#include "boost/qvm/mat_operations.hpp"
#include "boost/qvm/vec_mat_operations.hpp"
#include "../internal/olcPixelGameEngine.h"

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
#define V3RIGHT		Vector3(1, 0, 0)
#define V3LEFT		Vector3(-1, 0, 0)
#define V3UP		Vector3(0, 1, 0)
#define V3DOWN		Vector3(0, -1, 0)
#define V3FORWARD	Vector3(0, 0, 1)
#define V3BACKWARD	Vector3(0, 0, -1)

//qol defines
#define RANDCOLOR olc::Pixel(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1)
#define RANDVEC(a) Vector3(rand() % a + 1, rand() % a + 1, rand() % a + 1)

//physics constants
#define GRAVITY 9.81f

//useful math things
//averages vectors over a interval i and returns that average
//pass in vector v and interval i
#define V_AVG(i, v) ([&] { \
		static std::vector<Vector3> vectors; \
		static Vector3 nv; \
		static int iter = 0; \
		if(i == vectors.size()){ \
			vectors.erase(vectors.begin()); \
			vectors.push_back(v); \
			iter++; \
		} \
		else{ \
			vectors.push_back(v); \
			iter++; \
		}\
		if(iter == i){ \
			nv = Math::averageVector3(vectors); \
			iter = 0; \
		} \
		return nv; \
		}())

//averages vectors but consistently returns the value
#define V_AVGCON(i, v) ([&] { \
		static std::vector<Vector3> vectors; \
		static Vector3 nv; \
		if(i == vectors.size()){ \
			vectors.erase(vectors.begin()); \
			vectors.push_back(v); \
		} \
		else{ \
			vectors.push_back(v); \
		} \
		nv = Math::averageVector3(vectors); \
		return nv; \
		}())

#define F_AVG(i, f) ([&] { \
		static std::vector<float> floats; \
		static float nf; \
		static int iter = 0; \
		if(i == floats.size()){ \
			floats.erase(floats.begin()); \
			floats.push_back(f); \
			iter++; \
		} \
		else{ \
			floats.push_back(f); \
			iter++; \
		}\
		if(iter == i){ \
			nf = Math::average(floats, floats.size()); \
			iter = 0; \
		} \
		return nf; \
		}())

namespace Math {
	static Vector3 proj_points_persistant[3];

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

	//append all trailing zeros
	static std::string append_zeroes(std::string s) {
		while (s.back() == '0') {
			s.pop_back();
		}
		s.pop_back();
		return s;
	}

	//append to two decimal places
	static std::string append_two_decimal(std::string s) {
		if (s.length() >= 2) {
			while (s.at(s.length() - 4) != '.') {
				s.pop_back();
			}
			s.pop_back();
		}
		return s;
	}

	static std::string str2f(float s) {
		char buffer[50];
		std::snprintf(buffer, 50, "%-.2f", s);
		return std::string(buffer);
	}

	//round a float to two decimal places
	static float round2f(float f) { return (float)((int)(f * 100 + .5)) / 100; }

	static Vector3 round2v(Vector3 v) {
		return Vector3(
			(float)((int)(v.x * 100 + .5)) / 100,
			(float)((int)(v.y * 100 + .5)) / 100,
			(float)((int)(v.z * 100 + .5)) / 100);
	}

	//average any std container probably
	template<class FWIt>
	static float average(FWIt a, const FWIt b, int size) { return std::accumulate(a, b, 0.0) / size; }

	template<class T>
	static double average(const T& container, int size) { return average(std::begin(container), std::end(container), size); }

	static Vector3 averageVector3(std::vector<Vector3> v) {
		Vector3 n;
		for (Vector3 e : v) { n += e; }
		if (v.size() != 0) return n / v.size();
		else return V3ZERO;
	}

	//conversions
	static Vector3 vi2dToVector3(olc::vi2d vector, float z = 0) { return Vector3((float)vector.x, (float)vector.y, z); }

	static Vector3 M1x4ToVector3(mat<float, 1, 4> m) { return Vector3(m.a[0][0], m.a[0][1], m.a[0][2]); }

	//interpolating
	static float lerpf(float p1, float p2, float t) { return (1.f - t) * p1 + t * p2; }

	static Vector3 lerpv3(Vector3 v1, Vector3 v2, float t) { return  v1 * (1.f - t) + v2 * t; }

	//matrix stuff
	static mat<float, 1, 4> Vector3ToM1x4(Vector3 v) {
		mat<float, 1, 4> m;
		m.a[0][0] = v.x; m.a[0][1] = v.y; m.a[0][2] = v.z; m.a[0][3] = 1;
		return m;
	}

	static mat<float, 4, 4> GetTranslate(Vector3 translation) {
		mat<float, 4, 4> tv{
			1,			   0,			  0,			 0,
			0,			   1,			  0,			 0,
			0,			   0,			  1,			 0,
			translation.x, translation.y, translation.z, 1
		};
		return tv;
	}

	static mat<float, 4, 4> GetScaleMat(Vector3 scale) {
		mat<float, 4, 4> sv{
			scale.x, 0,		  0,	   0,
			0,		 scale.y, 0,	   0,
			0,		 0,		  scale.z, 0,
			0,		 0,		  0,	   1
		};
		return sv;
	}

	static mat<float, 4, 4> M3x3ToM4x4(mat<float, 3, 3> inMat) {
		return mat<float, 4, 4>{
			inMat.a[0][0], inMat.a[0][1], inMat.a[0][2], 0,
				inMat.a[1][0], inMat.a[1][1], inMat.a[1][2], 0,
				inMat.a[2][0], inMat.a[2][1], inMat.a[2][2], 0,
				0, 0, 0, 1
		};
	}

	static mat<float, 4, 4> WorldMatrix4x4(Vector3 translation, Vector3 rotation, Vector3 scale) {
		mat<float, 4, 4> matrix = boost::qvm::rot_mat_xyx<4, float>(rotation.x, rotation.y, rotation.z);
		matrix.a[0][3] = translation.x; matrix.a[1][3] = translation.y; matrix.a[2][3] = translation.z;
		matrix.a[0][0] *= scale.x; matrix.a[1][1] *= scale.y; matrix.a[2][2] *= scale.z;
		return matrix;
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

	static float DistTwoPoints(Vector3 a, Vector3 b) {
		return sqrtf(
			(a.x - b.x) * (a.x - b.x) +
			(a.y - b.y) * (a.y - b.y) +
			(a.z - b.z) * (a.z - b.z)
		);
	}

	static float DistPointToPlane(Vector3 point, Vector3 plane_n, Vector3 plane_p) {
		return (plane_n.x * point.x + plane_n.y * point.y + plane_n.z * point.z - plane_n.dot(plane_p));
	}

	//where a line intersects with a plane
	static Vector3 VectorPlaneIntersect(Vector3 plane_p, Vector3 plane_n, Vector3 line_start, Vector3 line_end, float& t) {
		plane_n.normalize();
		float plane_d = -plane_n.dot(plane_p);
		float ad = line_start.dot(plane_n);
		float bd = line_end.dot(plane_n);
		t = (-plane_d - ad) / (bd - ad);
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

	static mat<float, 4, 4> GetLocalToWorld(static Vector3 pos) {
		mat<float, 4, 4> wtl{
			1,	   0,	  0,	 0,
			0,	   1,	  0,	 0,
			0,	   0,	  1,	 0,
			pos.x, pos.y, pos.z, 1
		};
		return wtl;
	}

	static mat<float, 4, 4> GetWorldToLocal(Vector3 pos) {
		mat<float, 4, 4> ltw{
			1,	   0,	  0,	 0,
			0,	   1,	  0,	 0,
			0,	   0,	  1,	 0,
			pos.x, pos.y, pos.z, 1
		};
		return inverse(ltw);
	}

	static mat<float, 1, 4> ProjMult(mat<float, 1, 4> v, mat<float, 4, 4> m) {
		mat<float, 1, 4> vm = v * m;
		if (vm.a[0][3] != 0) { vm.a[0][0] /= vm.a[0][3]; vm.a[0][1] /= vm.a[0][3]; vm.a[0][2] /= vm.a[0][3]; }
		return vm;
	}

	//optional overload that 'returns' the w value
	static mat<float, 1, 4> ProjMult(mat<float, 1, 4> v, mat<float, 4, 4> m, float& w) {
		mat<float, 1, 4> vm = v * m;
		if (vm.a[0][3] != 0) { vm.a[0][0] /= vm.a[0][3]; vm.a[0][1] /= vm.a[0][3]; vm.a[0][2] /= vm.a[0][3]; w = vm.a[0][3]; }
		return vm;
	}

	static mat<float, 1, 4> unProjMult(mat<float, 1, 4> v, mat<float, 4, 4> m) {
		mat<float, 1, 4> vm = v * m;
		if (vm.a[0][3] != 0) { vm.a[0][0] /= vm.a[0][3]; vm.a[0][1] /= vm.a[0][3]; vm.a[0][2] /= vm.a[0][3]; }
		return vm;
	}

	//return where two lines intersect on the x axis with slope and the y-intercept
	static Vector3 LineIntersect2(float slope1, float ycross1, float slope2, float ycross2) {
		mat<float, 2, 2> lhs{ slope1, ycross1, slope2, ycross2 };
		mat<float, 2, 1> rhs{ 1, 1 };
		mat<float, 2, 1> det = inverse(lhs) * rhs;
		float x = 1 / (det.a[0][1]) * det.a[0][0];
		float y = slope1 * x + ycross1;
		return Vector3(x, y);
	}

	//returns where two lines intersect in 3D space
	static Vector3 LineIntersect3(Vector3 adir, Vector3 ap, Vector3 bdir, Vector3 bp) {
	}

	//returns area of a triangle of sides a and b
	static float TriangleArea(Vector3 a, Vector3 b) { return a.cross(b).mag() / 2; }

	//};

	//namespace Render {
		//the input vector should be in world space
	static Vector3 WorldToCamera(Vector3 vertex, mat<float, 4, 4> viewMatrix) {
		mat<float, 1, 4> vm = vertex.ConvertToM1x4() * viewMatrix;
		if (vm.a[0][3] != 0) { vm.a[0][0] /= vm.a[0][3]; vm.a[0][1] /= vm.a[0][3]; vm.a[0][2] /= vm.a[0][3]; }
		return Vector3(vm);
	}

	//the input vectors should be in view/camera space
	//returns true if the line can be rendered after clipping, false otherwise
	static bool ClipLineToZPlanes(Vector3& lineStart, Vector3& lineEnd, float nearZ, float farZ) {
		//clip to the near plane
		Vector3 planePoint = Vector3(0, 0, nearZ);
		Vector3 planeNormal = V3FORWARD;
		float d = planeNormal.dot(planePoint);
		bool startBeyondPlane = planeNormal.dot(lineStart) - d < 0;
		bool endBeyondPlane = planeNormal.dot(lineEnd) - d < 0;
		float t;
		if (startBeyondPlane && !endBeyondPlane) {
			lineStart = Math::VectorPlaneIntersect(planePoint, planeNormal, lineStart, lineEnd, t);
		}
		else if (!startBeyondPlane && endBeyondPlane) {
			lineEnd = Math::VectorPlaneIntersect(planePoint, planeNormal, lineStart, lineEnd, t);
		}
		else if (startBeyondPlane && endBeyondPlane) {
			return false;
		}

		//clip to the far plane
		planePoint = Vector3(0, 0, farZ);
		planeNormal = V3BACKWARD;
		d = planeNormal.dot(planePoint);
		startBeyondPlane = planeNormal.dot(lineStart) - d < 0; //TODO(r,delle) test whether this is right
		endBeyondPlane = planeNormal.dot(lineEnd) - d < 0;
		if (startBeyondPlane && !endBeyondPlane) {
			lineStart = Math::VectorPlaneIntersect(planePoint, planeNormal, lineStart, lineEnd, t);
		}
		else if (!startBeyondPlane && endBeyondPlane) {
			lineEnd = Math::VectorPlaneIntersect(planePoint, planeNormal, lineStart, lineEnd, t);
		}
		else if (startBeyondPlane && endBeyondPlane) {
			return false;
		}
		return true;
	}

	//the input matrixes should be in view/camera space
	static Vector3 CameraToScreen(Vector3 csVertex, mat<float, 4, 4> projectionMatrix, Vector2 dimensions) {
		mat<float, 1, 4> vm = csVertex.ConvertToM1x4() * projectionMatrix;
		if (vm.a[0][3] != 0) { vm.a[0][0] /= vm.a[0][3]; vm.a[0][1] /= vm.a[0][3]; vm.a[0][2] /= vm.a[0][3]; }
		Vector3 out(vm);
		out.x += 1.0f; out.y += 1.0f;
		out.x *= 0.5f * dimensions.x;
		out.y *= 0.5f * dimensions.y;
		return out;
	}

	//cohen-sutherland algorithm https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
	//the input vectors should be in screen space
	//returns true if the line can be rendered after clipping, false otherwise
	static bool ClipLineToBorderPlanes(Vector3& ssLineStart, Vector3& ssLineEnd, Vector2 screenDimensions) {
		//clip to the vertical and horizontal planes
		const int CLIP_INSIDE = 0;
		const int CLIP_LEFT = 1;
		const int CLIP_RIGHT = 2;
		const int CLIP_BOTTOM = 4;
		const int CLIP_TOP = 8;
		auto ComputeOutCode = [&](Vector3& vertex) {
			int code = CLIP_INSIDE;
			if (vertex.x < 0) {
				code |= CLIP_LEFT;
			}
			else if (vertex.x > screenDimensions.x) {
				code |= CLIP_RIGHT;
			}
			if (vertex.y < 0) { //these are inverted because we are in screen space
				code |= CLIP_TOP;
			}
			else if (vertex.y > screenDimensions.y) {
				code |= CLIP_BOTTOM;
			}
			return code;
		};

		int lineStartCode = ComputeOutCode(ssLineStart);
		int lineEndCode = ComputeOutCode(ssLineEnd);

		//loop until all points are within or outside the screen zone
		while (true) {
			if (!(lineStartCode | lineEndCode)) {
				//both points are inside the screen zone
				return true;
			}
			else if (lineStartCode & lineEndCode) {
				//both points are in the same outside zone
				return false;
			}
			else {
				float x, y;
				//select one of the points outside
				int code = lineEndCode > lineStartCode ? lineEndCode : lineStartCode;

				//clip the points the the screen bounds by finding the intersection point
				if (code & CLIP_TOP) { //point is above screen
					x = ssLineStart.x + (ssLineEnd.x - ssLineStart.x) * (-ssLineStart.y) / (ssLineEnd.y - ssLineStart.y);
					y = 0;
				}
				else if (code & CLIP_BOTTOM) { //point is below screen
					x = ssLineStart.x + (ssLineEnd.x - ssLineStart.x) * (screenDimensions.y - ssLineStart.y) / (ssLineEnd.y - ssLineStart.y);
					y = screenDimensions.y;
				}
				else if (code & CLIP_RIGHT) { //point is right of screen
					y = ssLineStart.y + (ssLineEnd.y - ssLineStart.y) * (screenDimensions.x - ssLineStart.x) / (ssLineEnd.x - ssLineStart.x);
					x = screenDimensions.x;
				}
				else if (code & CLIP_LEFT) { //point is left of screen
					y = ssLineStart.y + (ssLineEnd.y - ssLineStart.y) * (-ssLineStart.x) / (ssLineEnd.x - ssLineStart.x);
					x = 0;
				}

				//update the vector's points and restart loop
				if (code == lineStartCode) {
					ssLineStart.x = x;
					ssLineStart.y = y;
					lineStartCode = ComputeOutCode(ssLineStart);
				}
				else {
					ssLineEnd.x = x;
					ssLineEnd.y = y;
					lineEndCode = ComputeOutCode(ssLineEnd);
				}
			}
		}
	}
};

//// Non-Vector vs Vector Interactions ////

inline Vector2 Vector3::toVector2() const {
	return Vector2(x, y);
}

//// Old Vector3-QVM interactions ... To be removed ////

inline Vector3::Vector3(const Vector2& vector2) {
	this->x = vector2.x; this->y = vector2.y; this->z = 0;
}

inline Vector3::Vector3(mat<float, 1, 4> m) {
	this->x = m.a[0][0]; this->y = m.a[0][1]; this->z = m.a[0][2];
}

inline Vector3::Vector3(vec<float, 3> vector) {
	this->x = vector.a[0]; this->y = vector.a[1]; this->z = vector.a[2];
}

inline void    Vector3::operator =	(const mat<float, 1, 4>& rhs) {
	this->x = rhs.a[0][0]; this->y = rhs.a[0][1]; this->z = rhs.a[0][2];
}

inline Vector3 Vector3::operator *  (const mat<float, 4, 4> rhs) {
	mat<float, 1, 4> v{ x,y,z,1 }; return GetM1x4ToVector3(v * rhs);
}

inline Vector3 Vector3::operator *= (const mat<float, 4, 4> rhs) {
	mat<float, 1, 4> v{ x,y,z,1 }; return GetM1x4ToVector3(v * rhs);
}

inline mat<float, 1, 4> Vector3::ConvertToM1x4() {
	mat<float, 1, 4> m;
	m.a[0][0] = x; m.a[0][1] = y; m.a[0][2] = z; m.a[0][3] = 1;
	return m;
}

inline vec<float, 3> Vector3::ConvertToVec3() {
	return vec<float, 3>{x, y, z};
}

inline void Vector3::M1x4ToVector3(mat<float, 1, 4> m) {
	x = m.a[0][0]; y = m.a[0][1]; z = m.a[0][2];
}

inline Vector3 Vector3::GetM1x4ToVector3(mat<float, 1, 4> m) {
	return Vector3(m.a[0][0], m.a[0][1], m.a[0][2]);
}

inline mat<float, 1, 4> Vector3::proj_mult(mat<float, 1, 4> v, mat<float, 4, 4> m) {
	mat<float, 1, 4> vm = v * m;
	if (vm.a[0][3] != 0) { vm.a[0][0] /= vm.a[0][3]; vm.a[0][1] /= vm.a[0][3]; vm.a[0][2] /= vm.a[0][3]; }
	return vm;
}

inline mat<float, 1, 4> Vector3::proj_mult(mat<float, 1, 4> v, mat<float, 4, 4> m, float& w) {
	mat<float, 1, 4> vm = v * m;
	if (vm.a[0][3] != 0) { vm.a[0][0] /= vm.a[0][3]; vm.a[0][1] /= vm.a[0][3]; vm.a[0][2] /= vm.a[0][3]; w = vm.a[0][3]; }
	return vm;
}

inline mat<float, 1, 4> Vector3::unproj_mult(mat<float, 1, 4> v, mat<float, 4, 4> m) {
	mat<float, 1, 4> vm = v * m;
	if (vm.a[0][3] != 0) { vm.a[0][0] /= vm.a[0][3]; vm.a[0][1] /= vm.a[0][3]; vm.a[0][2] /= vm.a[0][3]; }
	return vm;
}

inline void Vector3::translateV3(Vector3 translation) {
	mat<float, 4, 4> tv{
			1,			   0,			  0,			 0,
			0,			   1,			  0,			 0,
			0,			   0,			  1,			 0,
			translation.x, translation.y, translation.z, 1
	};
	M1x4ToVector3(ConvertToM1x4() * tv);
}

inline void Vector3::scaleV3(Vector3 scale) {
	mat<float, 4, 4> sv{
			scale.x, 0,		  0,	   0,
			0,		 scale.y, 0,	   0,
			0,		 0,		  scale.z, 0,
			0,		 0,		  0,	   1
	};
	M1x4ToVector3(proj_mult(ConvertToM1x4(), sv));
}

inline void Vector3::LocalToWorld(Vector3 pos) {
	mat<float, 4, 4> wtl{
			1,	   0,	  0,	 0,
			0,	   1,	  0,	 0,
			0,	   0,	  1,	 0,
			pos.x, pos.y, pos.z, 1
	};
	M1x4ToVector3(proj_mult(ConvertToM1x4(), wtl));
}

inline void Vector3::WorldToLocal(Vector3 pos) {
	mat<float, 4, 4> ltw{
			1,	   0,	  0,	 0,
			0,	   1,	  0,	 0,
			0,	   0,	  1,	 0,
			pos.x, pos.y, pos.z, 1
	};
	M1x4ToVector3(proj_mult(ConvertToM1x4(), inverse(ltw)));
}

inline void Vector3::rotateV3_X(float theta, Vector3 pos, Vector3 offset) {
	theta = Math::to_radians(theta);
	WorldToLocal(pos + offset);
	mat<float, 4, 4> rvx{
		1,		0,			0,           0,
		0,		cos(theta),	-sin(theta), 0,
		0,		sin(theta),	cos(theta),  0,
		0,		0,			0,			 1
	};
	M1x4ToVector3(ConvertToM1x4() * rvx);
	LocalToWorld(pos + offset);
}

inline void Vector3::rotateV3_Y(float theta, Vector3 pos, Vector3 offset) {
	theta = Math::to_radians(theta);
	WorldToLocal(pos + offset);
	mat<float, 4, 4> rvy{
		cos(theta),	0,	sin(theta),  0,
		0,			1,	0,			 0,
		-sin(theta),0,	cos(theta),  0,
		0,			0,	0,			 1
	};
	M1x4ToVector3(ConvertToM1x4() * rvy);
	LocalToWorld(pos + offset);
}

inline void Vector3::rotateV3_Z(float theta, Vector3 pos, Vector3 offset) {
	theta = Math::to_radians(theta);
	WorldToLocal(pos + offset);
	mat<float, 4, 4> rvz{
		cos(theta), -sin(theta),	0, 0,
		sin(theta), cos(theta),		0, 0,
		0,			0,				1, 0,
		0,			0,				0, 1
	};
	M1x4ToVector3(ConvertToM1x4() * rvz);
	LocalToWorld(pos + offset);
}

inline void Vector3::ProjToScreen(mat<float, 4, 4> ProjMat, olc::PixelGameEngine* p) {
	M1x4ToVector3(proj_mult(ConvertToM1x4(), ProjMat));
	x += 1.0f; y += 1.0f;
	x *= 0.5f * (float)p->ScreenWidth();
	y *= 0.5f * (float)p->ScreenHeight();
}

inline void Vector3::ProjToScreen(mat<float, 4, 4> ProjMat, olc::PixelGameEngine* p, float& w) {
	float _w;
	M1x4ToVector3(proj_mult(ConvertToM1x4(), ProjMat, _w));
	x += 1.f; y += 1.f;
	x *= 0.5f * (float)p->ScreenWidth();
	y *= 0.5f * (float)p->ScreenHeight();
	w = _w;
}

inline void Vector3::ScreenToWorld(mat<float, 4, 4> ProjMat, mat<float, 4, 4> view, olc::PixelGameEngine* p) {
	x /= .5f * (float)p->ScreenWidth();
	y /= .5f * (float)p->ScreenHeight();
	x -= 1.f; y -= 1.f; z = -1.f;
	M1x4ToVector3(unproj_mult(ConvertToM1x4(), inverse(ProjMat)));
	M1x4ToVector3(unproj_mult(ConvertToM1x4(), inverse(view)));
}

//attached to entities to allow different forms of checking sides of more complex objects
struct Edge {
	Vector3 p[2];
	//if lead is true then p[1] is the right most point.
	//ditto for high but on y
	bool lead, high;

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
	bool within_range(Vector3 point) { return (point.y < p[high].y&& point.y > p[!high].y); }
	bool within_range(float y_point) { return (y_point < p[high].y&& y_point > p[!high].y); }
	bool within_domain(Vector3 point) { return (point.x < p[lead].x&& point.x > p[!lead].x); }
	bool within_domain(float x_point) { return (x_point < p[lead].x&& x_point > p[!lead].x); }

	//returns edge's normal
	//returns a normal rotated -90
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
		Vector3 cross = Math::LineIntersect2(slope(), ycross(), e.slope(), e.ycross());
		if (within_domain(cross) && within_range(cross) &&
			e.within_domain(cross) && e.within_range(cross)) {
			return true;
		}
		else {
			return false;
		}
	}

	std::string str() { return "{(" + p[0].str() + "), (" + p[1].str() + ")}"; }
	std::string str2f() { return "{(" + p[0].str2f() + "), (" + p[1].str2f() + ")}"; }
};

struct Edge3 {
	Vector3 p[2];
	//if lead is true then p[1] is the right most point.
	//ditto for high but on y and for deep on z
	bool lead, high, deep;

	Edge3() { }
	Edge3(Vector3 point1, Vector3 point2) {
		p[0] = point1;
		p[1] = point2;
		if (point1.x > point2.x) { lead = false; }
		else { lead = true; }
		if (point1.y > point2.y) { high = false; }
		else { high = true; }
		if (point1.z > point2.z) { deep = false; }
		else { deep = true; }
	}

	std::string str() { return "{(" + p[0].str() + "), (" + p[1].str() + ")}"; }
	std::string str2f() { return "{(" + p[0].str2f() + "), (" + p[1].str2f() + ")}"; }

	Vector3 edge_midpoint() {
		return Vector3((p[0].x + p[1].x) / 2, (p[0].y + p[1].y) / 2, (p[0].z + p[1].z) / 2);
	}

	Vector3 direction() { return p[1] - p[0]; }
};

namespace Debug {
	static std::string M1x4ToString(mat<float, 1, 4> matrix) {
		return "(" + std::to_string(matrix.a[0][0]) + ", " + std::to_string(matrix.a[0][1]) + ", " + std::to_string(matrix.a[0][2]) + ", " + std::to_string(matrix.a[0][3]) + ")";
	}
};