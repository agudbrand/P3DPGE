#pragma once
#include "Math.h"
#include "olcPixelGameEngine.h"

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

//a collection of triangles that make up the geomery of objects in space
class Mesh {
public:
	std::vector<Triangle> triangles;

	std::vector<Triangle> drawnTriangles_Debug;

	Vector3 camPos;
	mat<float, 4, 4> ProjMat;
	mat<float, 4, 4> view;

	Mesh() { triangles = std::vector<Triangle>(); }

	void Update(Vector3 camPos, mat<float, 4, 4> ProjMat, mat<float, 4, 4> view) {
		this->camPos = camPos;
		this->ProjMat = ProjMat;
		this->view = view;
	}

	Vector3 getCamPos()				{ return camPos; }
	mat<float, 4, 4> getProjMat()	{ return ProjMat; }
	mat<float, 4, 4> getView()		{ return view; }

	virtual void Draw(olc::PixelGameEngine* p, Vector3 pos, bool wireframe = false, olc::Pixel color = olc::WHITE) {
		std::vector<Triangle> visibleTriangles;
		std::vector<Triangle> drawnTriangles;
		drawnTriangles_Debug.clear();
		//temp lighting set up
		Vector3 light_direction(0, 0, 1);
		light_direction = light_direction.normalized();
		//std::cout << t.get_normal().z << std::endl;

		//store triangles we want to draw for sorting and copy world points to projected points
		for (auto& t : triangles) {
			t.copy_points();
			if (t.get_proj_normal().dot(t.points[0] - camPos) > 0) { 
				visibleTriangles.push_back(t); 
			}
		}

		//project triangles to screen and add them to the 
		//draw vector
		for (Triangle t : visibleTriangles) {
			for (Vector3& n : t.projectedPoints) {
				n.M1x4ToVector3(n.proj_mult(n.ConvertToM4x4(), view));
			}

			int clippedTriangles = 0;
			Triangle clipped[2];
			clippedTriangles = ClipTriangles(Vector3(0, 0, 0.1), Vector3(0, 0, 1), t, clipped[0], clipped[1]);
			
			std::cout << clippedTriangles << std::endl;

			for (int i = 0; i < clippedTriangles; i++) {
				for (Vector3& n : clipped[i].projectedPoints) {
					n.ProjToScreen(ProjMat, p, pos);
				}
				drawnTriangles.push_back(clipped[i]);
				drawnTriangles_Debug.push_back(clipped[i]);
			}
		}

		std::sort(drawnTriangles.begin(), drawnTriangles.end(), [](Triangle& t1, Triangle& t2) {
			float mp1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3;
			float mp2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3;
			return mp1 > mp2;
			});

		for (Triangle t : drawnTriangles) {

			Triangle clipped[2];
			std::list<Triangle> listTriangles;

			listTriangles.push_back(t);
			int newTriangles = 1;

			for (int a = 0; a < 4; a++) {
				int trisToAdd = 0;
				while (newTriangles > 0) {
					Triangle test = listTriangles.front();
					listTriangles.pop_front();
					newTriangles--;

					switch (a) {
					case 0:	trisToAdd = ClipTriangles(Vector3(0, 0, 0), Vector3(0, 1, 0), test, clipped[0], clipped[1]); break;
					case 1:	trisToAdd = ClipTriangles(Vector3(0, (float)p->ScreenHeight() - 1, 0), Vector3(0, -1, 0), test, clipped[0], clipped[1]); break;
					case 2:	trisToAdd = ClipTriangles(Vector3(0, 0, 0), Vector3(1, 0, 0), test, clipped[0], clipped[1]); break;
					case 3: trisToAdd = ClipTriangles(Vector3((float)p->ScreenHeight() - 1, 0, 0), Vector3(-1, 0, 0), test, clipped[0], clipped[1]); break;
					}

					for (int w = 0; w < trisToAdd; w++) { listTriangles.push_back(clipped[w]); }
				}
				newTriangles = listTriangles.size();
			}

			float dp = light_direction.dot(t.get_normal());
			for (Triangle& t : listTriangles) {
				p->FillTriangle(
					t.projectedPoints[0].x, t.projectedPoints[0].y,
					t.projectedPoints[1].x, t.projectedPoints[1].y,
					t.projectedPoints[2].x, t.projectedPoints[2].y,
					olc::Pixel(25 * abs(dp), 150 * abs(dp), 255 * abs(dp)));
			}
			
		}

		if (wireframe) {
			for (auto& t : drawnTriangles) {
				p->DrawTriangle(
					t.projectedPoints[0].x, t.projectedPoints[0].y,
					t.projectedPoints[1].x, t.projectedPoints[1].y,
					t.projectedPoints[2].x, t.projectedPoints[2].y,
					olc::WHITE);
			}
		}
	}//Draw

	//virtual void Draw(olc::PixelGameEngine* p, )

	//this function is really complex and i just pulled it from Javid's video
	//hopefully later i'll try to understand it better
	//TODO(+rs, sushi, 11/15/2020, Implement Clipping Algorithm) mesh Javid's clipping algorithm with what we already have set up, also rewatch his video to fix the camera not moving the clipping plane.
	int ClipTriangles(Vector3 plane_p, Vector3 plane_n, Triangle in_tri, Triangle& out_tri1, Triangle& out_tri2) {
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
		float d0 = dist(in_tri.projectedPoints[0]);
		float d1 = dist(in_tri.projectedPoints[1]);
		float d2 = dist(in_tri.projectedPoints[2]);
		

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
			out_tri1.projectedPoints[1] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[0]);
			out_tri1.projectedPoints[2] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[1]);

			return 1; //return new triangle
		}
		if (nInsidePointCount == 2 && nOutsidePointCount == 1) {
			//triangle will be clipped and becomes a quad which is
			//cut into two more triagles.

			out_tri1.projectedPoints[0] = *inside_points[0];
			out_tri1.projectedPoints[1] = *inside_points[1];
			out_tri1.projectedPoints[2] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[0]);

			out_tri2.projectedPoints[0] = *inside_points[1];
			out_tri2.projectedPoints[1] = out_tri1.projectedPoints[2];
			out_tri2.projectedPoints[2] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[1], *outside_points[0]);
			return 2;
		}
	}//ClipTriangles
};

//TODO(c, sushi) adapt this to be drawn in an entity class
struct CircleMesh : public Mesh {
	float radius;

	CircleMesh(float radius) {
		this->radius = radius;
	}

	void Draw(olc::PixelGameEngine* p, Vector3 pos, bool wireframe = false, olc::Pixel color = olc::WHITE) override {
		p->FillCircle(pos.Vector3Tovd2d(), radius, color);
	}
};



struct BoxMesh : public Mesh {
	BoxMesh(Vector3 dimensions, Vector3 position){

		//vertices making up the box
		Vector3 p1 = position + dimensions.xInvert().yInvert().zInvert();
		Vector3 p2 = position + dimensions.yInvert().zInvert();
		Vector3 p3 = position + dimensions.xInvert().zInvert();
		Vector3 p4 = position + dimensions.xInvert().yInvert();
		Vector3 p5 = position + dimensions.xInvert();
		Vector3 p6 = position + dimensions.yInvert();
		Vector3 p7 = position + dimensions.zInvert();
		Vector3 p8 = position + dimensions;

		//west
		triangles.push_back(Triangle(p3, p1, p4));
		triangles.push_back(Triangle(p3, p4, p5));
		//top
		triangles.push_back(Triangle(p4, p1, p2));
		triangles.push_back(Triangle(p4, p2, p6));
		//east
		triangles.push_back(Triangle(p8, p6, p2));
		triangles.push_back(Triangle(p8, p2, p7));
		//bottom
		triangles.push_back(Triangle(p3, p5, p8));
		triangles.push_back(Triangle(p3, p8, p7));
		//south
		triangles.push_back(Triangle(p5, p4, p6));
		triangles.push_back(Triangle(p5, p6, p8));
		//north
		triangles.push_back(Triangle(p7, p2, p1));
		triangles.push_back(Triangle(p7, p1, p3));
	}
};

