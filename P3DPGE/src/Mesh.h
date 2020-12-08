#pragma once
#include "Math.h"
#include "internal/olcPixelGameEngine.h"

//this will currently only be set up to facilitate 2D
//also this could probably be stored in math
//this is primarily for calculations and doesn't actually do anything

class Entity;

//collection of 3 points forming the basis of meshes
struct Triangle {
	//this can probably be different but it works for now
	Vector3 points[3];
	Vector3 proj_points[3];

	Vector3 tex_points[3];
	Entity* e;

	//maybe edges can be cleared when they're not actually needed,
	//and only spawned when used?
	Edge edges[3];
	
	bool selected = false;
	olc::Pixel color = olc::WHITE;
	olc::Pixel debug_color = olc::RED;

	Triangle() {}
	//constructing triangles without attached entity
	Triangle(Vector3 p1, Vector3 p2, Vector3 p3) {
		points[0] = p1;
		points[1] = p2;
		points[2] = p3;
		copy_points();

		edges[0] = Edge(p1, p2);
		edges[1] = Edge(p2, p3);
		edges[2] = Edge(p3, p1);
	}
	Triangle(Vector3 p1, Vector3 p2, Vector3 p3, Entity* e) {
		points[0] = p1;
		points[1] = p2;
		points[2] = p3;
		copy_points();
		
		edges[0] = Edge(p1, p2);
		edges[1] = Edge(p2, p3);
		edges[2] = Edge(p3, p1);

		this->e = e;
	}
	//for constructing a triangle with texture points
	Triangle(Vector3 p1, Vector3 p2, Vector3 p3, Vector3 t1, Vector3 t2, Vector3 t3, Entity* e) {
		points[0] = p1;
		points[1] = p2;
		points[2] = p3;
		copy_points();

		tex_points[0] = t1;
		tex_points[1] = t2;
		tex_points[2] = t3;

		edges[0] = Edge(p1, p2);
		edges[1] = Edge(p2, p3);
		edges[2] = Edge(p3, p1);

		this->e = e;
	}
	
	void copy_points() {
		for (int p = 0; p < 3; p++) { proj_points[p] = points[p]; }
	}

	void update_edges() {
		edges[0].update(proj_points[0], proj_points[1]);
		edges[1].update(proj_points[1], proj_points[2]);
		edges[2].update(proj_points[2], proj_points[0]);
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


	float get_area() { return Math::TriangleArea(points[1] - points[0], points[2] - points[0]); }

	//TODO(m, sushi) make this take just a line when we get forward declaring
	bool line_intersect(Edge3* e) {
		float t = 0;

		Vector3 i = Math::VectorPlaneIntersect(points[0], get_normal(), e->p[0], e->p[1], t);
		
		float a1 = Triangle(i, points[0], points[2]).get_area();
		float a2 = Triangle(i, points[2], points[1]).get_area();
		float a3 = Triangle(i, points[1], points[0]).get_area();

		float ta = Math::roundf_two((a1 + a2 + a3));

		if (ta > Math::roundf_two(get_area())) { return false; }
		else { return true; }
	
	}

	//debug
	void display_edges(olc::PixelGameEngine* p) {
		update_edges();

		int n = 1;
		for (Edge e : edges) {
			p->DrawString((e.edge_midpoint() + e.edge_normal() * -10).toVector2(), std::to_string(n));
			n++;
		}
		
	}

	Vector3 midpoint() {
		float x_mid = (points[0].x + points[1].x + points[2].x) / 3;
		float y_mid = (points[0].y + points[1].y + points[2].y) / 3;
		float z_mid = (points[0].z + points[1].z + points[2].z) / 3;

		return Vector3(x_mid, y_mid, z_mid);
	}

	std::string str() { return "{(" + points[0].str() + "), (" + points[1].str() + "), (" + points[2].str() + ")}"; }
	std::string rndstr() { return "{(" + points[0].str2f() + "), (" + points[1].str2f() + "), (" + points[2].str2f() + ")}"; }
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

	//for making a single triangle. debug type shtuff
	Mesh(Triangle t) { triangles.push_back(t); }
};

/*
struct Mesh2D : public Mesh{
	
};
*/

struct CircleMesh : public Mesh {
	float radius;
	
	CircleMesh(float radius) {
		this->radius = radius;
	}
};

struct BoxMesh : public Mesh {
	BoxMesh(Vector3 dimensions, Vector3 position, Entity* e) {
		//vertices making up the box
		Vector3 p1 = position + dimensions.xInvert().yInvert().zInvert();
		Vector3 p2 = position + dimensions.yInvert().zInvert();
		Vector3 p3 = position + dimensions.xInvert().zInvert();
		Vector3 p4 = position + dimensions.xInvert().yInvert();
		Vector3 p5 = position + dimensions.xInvert();
		Vector3 p6 = position + dimensions.yInvert();
		Vector3 p7 = position + dimensions.zInvert();
		Vector3 p8 = position + dimensions;
		
		//TODO(c, sushi) do this better later
		//west
		triangles.push_back(Triangle(p3, p1, p4, Vector3(0, 1, 1), Vector3(0, 0, 1), Vector3(1, 0, 1), e)); 
		triangles.push_back(Triangle(p3, p4, p5, Vector3(0, 1, 1), Vector3(1, 0, 1), Vector3(1, 1, 1), e)); 
		//top	
		triangles.push_back(Triangle(p4, p1, p2, Vector3(0, 1, 1), Vector3(0, 0, 1), Vector3(1, 0, 1), e)); 
		triangles.push_back(Triangle(p4, p2, p6, Vector3(0, 1, 1), Vector3(1, 0, 1), Vector3(1, 1, 1), e)); 
		//east	
		triangles.push_back(Triangle(p8, p6, p2, Vector3(0, 1, 1), Vector3(0, 0, 1), Vector3(1, 0, 1), e)); 
		triangles.push_back(Triangle(p8, p2, p7, Vector3(0, 1, 1), Vector3(1, 0, 1), Vector3(1, 1, 1), e)); 
		//bottom
		triangles.push_back(Triangle(p3, p5, p8, Vector3(0, 1, 1), Vector3(0, 0, 1), Vector3(1, 0, 1), e)); 
		triangles.push_back(Triangle(p3, p8, p7, Vector3(0, 1, 1), Vector3(1, 0, 1), Vector3(1, 1, 1), e)); 
		//south	
		triangles.push_back(Triangle(p5, p4, p6, Vector3(0, 1, 1), Vector3(0, 0, 1), Vector3(1, 0, 1), e)); 
		triangles.push_back(Triangle(p5, p6, p8, Vector3(0, 1, 1), Vector3(1, 0, 1), Vector3(1, 1, 1), e)); 
		//north	
		triangles.push_back(Triangle(p7, p2, p1, Vector3(0, 1, 1), Vector3(0, 0, 1), Vector3(1, 0, 1), e)); 
		triangles.push_back(Triangle(p7, p1, p3, Vector3(0, 1, 1), Vector3(1, 0, 1), Vector3(1, 1, 1), e)); 
	}
};
