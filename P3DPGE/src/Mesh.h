#pragma once
#include "Math.h"
#include "olcPixelGameEngine.h"

//this will currently only be set up to facilitate 2D
//also this could probably be stored in math
//this is primarily for calculations and doesn't actually do anything

//collection of 3 points forming the basis of meshes
struct Triangle {
	//this can probably be different but it works for now
	Vector3 points[3];
	Vector3 proj_points[3];
	Vector3 proj_persistent[3];

	//maybe edges can be cleared when they're not actually needed,
	//and only spawned when used?
	Edge edges[3];
	
	bool selected = false;
	olc::Pixel color = olc::WHITE;
	olc::Pixel debug_color = olc::RED;

	Triangle() {}
	Triangle(Vector3 p1, Vector3 p2, Vector3 p3) {
		points[0] = p1;
		points[1] = p2;
		points[2] = p3;
		copy_points();
		
		edges[0] = Edge(p1, p2);
		edges[1] = Edge(p2, p3);
		edges[2] = Edge(p3, p1);
	}
	
	void copy_persistent() {
		for (int p = 0; p < 3; p++) { proj_persistent[p] = proj_points[p]; }
	}
	
	void copy_points() {
		for (int p = 0; p < 3; p++) { proj_points[p] = points[p]; }
	}
	
	void update_edges() {
		edges[0].update(proj_persistent[0], proj_persistent[1]);
		edges[1].update(proj_persistent[1], proj_persistent[2]);
		edges[2].update(proj_persistent[2], proj_persistent[0]);
	}
	
	void set_color(olc::Pixel newColor) {
		color = newColor; 
	}

	olc::Pixel get_color() {
		if (selected) { return debug_color; }
		else { return color; }
	}

	Vector3 get_normal() {
		Vector3 l1 = points[1] - points[0];
		Vector3 l2 = points[2] - points[0];
		return l1.cross(l2).yInvert().normalized();
	}
	
	Vector3 get_proj_normal() {
		Vector3 l1 = proj_points[1] - proj_points[0];
		Vector3 l2 = proj_points[2] - proj_points[0];
		return l1.cross(l2).yInvert().normalized();
	}
	
	//checks if a triangle contains a point in screen space
	//this works by forming a line between each point on the triangle
	//and checks to see if the test point is within the region enclosed
	//by those three lines
	bool contains_point(Vector3 point) {
		
		update_edges();
		
		//if normal points up then bool is true
		bool norms[3] = {
			(edges[0].edge_normal().y < 0) ? true : false,
			(edges[1].edge_normal().y < 0) ? true : false,
			(edges[2].edge_normal().y < 0) ? true : false
		};
		
		bool truths[3];
		for (int b = 0; b < 3; b++) {
			std::cout << "Norm " << norms[b] << std::endl;
			if (norms[b]) {
				truths[b] = edges[b].above_edge(point);
			}
			else {
				truths[b] = edges[b].below_edge(point);
			}
		}
		
		std::cout << truths[0] << " " << truths[1] << " " << truths[2] << std::endl;
		
		bool the_final_truth = true;
		for (bool b : truths) {
			if (!b) { 
				the_final_truth = false; 
			}
		}
		
		if (the_final_truth) { return true; }
		else { return false; }
		
	}

	//debug
	void display_edges(olc::PixelGameEngine* p) {
		update_edges();

		int n = 1;
		for (Edge e : edges) {
			p->DrawString((e.edge_midpoint() + e.edge_normal() * -10).Vector3Tovd2d(), std::to_string(n));
			n++;
		}
		
	}

	Vector3 midpoint() {
		float x_mid = (points[0].x + points[1].x + points[2].x) / 3;
		float y_mid = (points[0].y + points[1].y + points[2].y) / 3;
		float z_mid = (points[0].z + points[1].z + points[2].z) / 3;

		return Vector3(x_mid, y_mid, z_mid);
	}
};

//TODO(,delle,11/22/20) abstract out Mesh and split into Mesh2D and Mesh3D
/*
struct Mesh{
	Entity* entity;
	
	virtual void Draw(olc::PixelGameEngine* p, bool wireframe = false) = 0;
};
*/

//a collection of triangles that make up the geomery of objects in space
class Mesh {
	public:
	std::vector<Triangle> triangles;
	std::vector<Triangle> drawnTriangles;

	Vector3 camPos;
	mat<float, 4, 4> ProjMat;
	mat<float, 4, 4> view;
	
	
	Mesh() { triangles = std::vector<Triangle>(); }
	
	//for manually making Triangles
	Mesh(std::vector<Vector3> points) {
		if (points.size() % 3 != 0) { throw "Invalid number of points"; }
		else {
			for (int v = 0; v < points.size(); v += 3){
				triangles.push_back(Triangle(points[v], points[v + 1], points[v + 2]));
			}
		}
	}
	
	void Update(Vector3 camPos, mat<float, 4, 4> ProjMat, mat<float, 4, 4> view) {
		this->camPos = camPos;
		this->ProjMat = ProjMat;
		this->view = view;
	}

	virtual void Draw(olc::PixelGameEngine* p, Vector3 pos, bool wireframe = false, olc::Pixel color = olc::WHITE) {
		std::vector<Triangle> visibleTriangles;
		
		drawnTriangles.clear();
		//temp lighting set up
		Vector3 light_direction(0, 0, 1);
		light_direction = light_direction.normalized();
		
		//store triangles we want to draw for sorting and copy world points to projected points
		for (auto& t : triangles) {
			t.copy_points();
			if (t.get_normal().dot(t.midpoint() - camPos) < 0) {
				float dp = light_direction.dot(t.get_normal());
				t.set_color(olc::Pixel(50 * abs(dp), 75 * abs(dp), 200 * abs(dp)));
				visibleTriangles.push_back(t);
			}
		}

		//project triangles to screen and add them to the draw vector
		for (Triangle t : visibleTriangles) {
			for (Vector3& n : t.proj_points) {
				n.M1x4ToVector3(n.proj_mult(n.ConvertToM1x4(), view));
			}
			
			int clippedTriangles = 0;
			Triangle clipped[2];
			clippedTriangles = ClipTriangles(Vector3(0, 0, 0.01), Vector3(0, 0, 1), t, clipped[0], clipped[1]);
			
			for (int i = 0; i < clippedTriangles; i++) {
				for (Vector3& n : clipped[i].proj_points) {
					n.ProjToScreen(ProjMat, p);
				}
				clipped[i].set_color(t.get_color());
				drawnTriangles.push_back(clipped[i]);
			}
		}
		
		//sorting is done based off world z not camera z
		std::sort(drawnTriangles.begin(), drawnTriangles.end(), [](Triangle& t1, Triangle& t2) {
			float mp1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3;
			float mp2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3;
			return mp1 > mp2;
			});

		//TODO(o, sushi) optimize this for loop 
		for (Triangle t : drawnTriangles) {
			t.copy_persistent();
			//t.display_edges(p);

			Triangle clipped[2];
			std::list<Triangle*> listTriangles;
			
			listTriangles.push_back(&t);
			int newTriangles = 1;
			
			for (int a = 0; a < 4; a++) {
				int trisToAdd = 0;
				while (newTriangles > 0) {
					Triangle test = *listTriangles.front();
					listTriangles.pop_front();
					newTriangles--;
					
					switch (a) {
					case 0:	trisToAdd = ClipTriangles(Vector3(0, 0, 0), Vector3(0, 1, 0), test, clipped[0], clipped[1]); break;
					case 1:	trisToAdd = ClipTriangles(Vector3(0, (float)p->ScreenHeight() - 1, 0), Vector3(0, -1, 0), test, clipped[0], clipped[1]); break;
					case 2:	trisToAdd = ClipTriangles(Vector3(0, 0, 0), Vector3(1, 0, 0), test, clipped[0], clipped[1]); break;
					case 3: trisToAdd = ClipTriangles(Vector3((float)p->ScreenHeight() - 1, 0, 0), Vector3(-1, 0, 0), test, clipped[0], clipped[1]); break;
					}

					for (int w = 0; w < trisToAdd; w++) { 
						clipped[w].set_color(test.get_color()); 
						listTriangles.push_back(&clipped[w]); 
					}
				}
				newTriangles = listTriangles.size();
			}
			
			for (Triangle* t : listTriangles) {
				p->FillTriangle(
					t->proj_points[0].x, t->proj_points[0].y,
					t->proj_points[1].x, t->proj_points[1].y,
					t->proj_points[2].x, t->proj_points[2].y,
					t->get_color());
			}
		}
		
		if (wireframe) {
			for (auto& t : drawnTriangles) {
				p->DrawTriangle(
					t.proj_points[0].x, t.proj_points[0].y,
					t.proj_points[1].x, t.proj_points[1].y,
					t.proj_points[2].x, t.proj_points[2].y,
					olc::WHITE);
				
			}
		}
	}//Draw
	
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
		float d0 = dist(in_tri.proj_points[0]);
		float d1 = dist(in_tri.proj_points[1]);
		float d2 = dist(in_tri.proj_points[2]);
		
		if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.proj_points[0]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.proj_points[0]; }
		if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.proj_points[1]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.proj_points[1]; }
		if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.proj_points[2]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.proj_points[2]; }
		
		//classify points and break input triangle into smaller trangles if
		//required. there are four possible outcomes
		
		//all points lie outside the plane
		if (nInsidePointCount == 0) { return 0; }
		//all points lie inside the plane so do nothing and allow triangle to pass
		if (nInsidePointCount == 3) { out_tri1 = in_tri; return 1; }
		if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
			//the inside point is valid so we keep it
			out_tri1.proj_points[0] = *inside_points[0];
			
			//but the two new points are not where the original triangle intersects with the plane
			out_tri1.proj_points[1] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[0]);
			out_tri1.proj_points[2] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[1]);
			
			return 1; //return new triangle
		}
		if (nInsidePointCount == 2 && nOutsidePointCount == 1) {
			//triangle will be clipped and becomes a quad which is
			//cut into two more triagles.
			
			out_tri1.proj_points[0] = *inside_points[0];
			out_tri1.proj_points[1] = *inside_points[1];
			out_tri1.proj_points[2] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[0]);
			
			out_tri2.proj_points[0] = *inside_points[1];
			out_tri2.proj_points[1] = out_tri1.proj_points[2];
			out_tri2.proj_points[2] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[1], *outside_points[0]);
			return 2;
		}
	}//ClipTriangles
};

/*
struct Mesh2D : public Mesh{
	
};
*/

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
	BoxMesh(Vector3 dimensions, Vector3 position) {
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
		
		for (Triangle& t : triangles) {
			t.set_color(olc::Pixel(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1));
		}
	}
};
