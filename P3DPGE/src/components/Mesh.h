#pragma once
#include "Component.h"

#include "../math/Vector3.h"
#include "../geometry/Triangle.h"
#include "../animation/Armature.h"

struct Mesh : public Component {
	Armature* armature = nullptr;
	std::vector<Triangle> triangles;

	//single triangle mesh for testing
	Mesh(Triangle t) {
		t.sprite = new olc::Sprite(25, 25);
		triangles.push_back(t);
	}

	Mesh(std::vector<Triangle> triangles, Armature* armature = 0) {
		this->armature = armature;
		this->triangles = triangles;
	}

	~Mesh() {
		if(armature) {
			delete armature;
		}
	}

	static Mesh* CreateBox(Entity* e, Vector3 halfDims, Vector3 position) {
		std::vector<Triangle> triangles;

		Vector3 p1 = position + halfDims.xInvert().yInvert().zInvert();
		Vector3 p2 = position + halfDims.yInvert().zInvert();
		Vector3 p3 = position + halfDims.xInvert().zInvert();
		Vector3 p4 = position + halfDims.xInvert().yInvert();
		Vector3 p5 = position + halfDims.xInvert();
		Vector3 p6 = position + halfDims.yInvert();
		Vector3 p7 = position + halfDims.zInvert();
		Vector3 p8 = position + halfDims;

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

		Mesh* m = new Mesh(triangles);
		m->entity = e;
		return m;
	}

	static Mesh* CreateComplex(Entity* e, const char* fileName, bool hasTexture, Vector3 position) {
		std::ifstream f(fileName);
		if (!f.is_open()) { return 0; }
	
		std::vector<Triangle> triangles;
		std::vector<Vector3> vertices;
		std::vector<Vector3> textices;

		while (!f.eof()) {
			char line[128];
			f.getline(line, 128);
			std::stringstream s;
			s << line;
			char junk;

			if (line[0] == 'v') {
				if (line[1] == 't') {
					Vector3 t;
					s >> junk >> junk >> t.x >> t.y;
					textices.push_back(t);
				} else {
					Vector3 v;
					s >> junk >> v.x >> v.y >> v.z;
					vertices.push_back(v);
				}
			}

			if (!hasTexture) {
				if (line[0] == 'f') {
					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];

					triangles.push_back(Triangle(vertices[f[0] - (size_t)1], vertices[f[1] - (size_t)1], vertices[f[2] - (size_t)1], e));
				}
			} else {
				if (line[0] == 'f') {
					s >> junk;
					std::string tokens[6];
					int tokenCount = -1;

					while (!s.eof()) {
						char c = s.get();
						if (c == ' ' || c == '/') {
							tokenCount++;
						} else {
							tokens[tokenCount].append(1, c);
						}
					}
					tokens[tokenCount].pop_back();

					triangles.push_back(Triangle( vertices[stoi(tokens[0]) - 1], vertices[stoi(tokens[2]) - 1], vertices[stoi(tokens[4]) - 1],
						textices[stoi(tokens[1]) - 1], textices[stoi(tokens[3]) - 1], textices[stoi(tokens[5]) - 1], e));
				}
			}
		}

		Mesh* m = new Mesh(triangles);
		m->entity = e;
		return m;
	}
	
};