#include "MeshSystem.h"
#include "../math/Math.h"
#include "../EntityAdmin.h"

#include "../components/Mesh.h"
#include "../components/Transform.h"

Mesh* MeshSystem::BuildBoxMesh(Entity* e, Transform* t, Vector3 halfDims) {
	
	std::vector<Triangle> triangles;

	Vector3 p1 = t->position + halfDims.xInvert().yInvert().zInvert();
	Vector3 p2 = t->position + halfDims.yInvert().zInvert();
	Vector3 p3 = t->position + halfDims.xInvert().zInvert();
	Vector3 p4 = t->position + halfDims.xInvert().yInvert();
	Vector3 p5 = t->position + halfDims.xInvert();
	Vector3 p6 = t->position + halfDims.yInvert();
	Vector3 p7 = t->position + halfDims.zInvert();
	Vector3 p8 = t->position + halfDims;

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
	RotateMesh(m, Matrix4::RotationMatrixAroundPoint(t->position, t->rotation));
	ScaleMesh(m, Matrix3::ScaleMatrix(t->scale));
	return m;
}

Mesh* MeshSystem::BuildComplexMesh(Entity* e, Transform* t, const char* fileName, bool hasTexture) {
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

				triangles.push_back(Triangle(vertices[f[0] - 1], vertices[f[1] - 1], vertices[f[2] - 1], e));
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
	RotateMesh(m, Matrix4::RotationMatrixAroundPoint(t->position, t->rotation));
	ScaleMesh(m, Matrix3::ScaleMatrix(t->scale));
	return m;
}

void MeshSystem::TranslateMesh(Mesh* mesh, Vector3 translation) {
	for(auto& t : mesh->triangles) {
		for(auto& p : t.points) {
			p += translation;
		}
	}
}

void MeshSystem::RotateMesh(Mesh* mesh, Matrix4 rotation) {
	for(auto& t : mesh->triangles) {
		for(auto& p : t.points) {
			p *= rotation;
		}
	}
}

void MeshSystem::ScaleMesh(Mesh* mesh, Matrix3 scale) {
	for(auto& t : mesh->triangles) {
		for(auto& p : t.points) {
			p *= scale; //TODO(,delle) test 3x3 scale matrices
		}
	}
}

void MeshSystem::TransformMesh(Mesh* mesh, Matrix4 transformation) {
	for(auto& t : mesh->triangles) {
		for(auto& p : t.points) {
			p *= transformation;
		}
	}
}

void MeshSystem::Update(float deltaTime, olc::PixelGameEngine* p) {
	for(auto e : admin->entities) {
		bool hasMesh = false;
		bool hasTransform = false;
		Mesh* m = 0;
		Transform* t = 0;
		for(auto c : e.second->components) {
			if(m = dynamic_cast<Mesh*>(c)) {
				hasMesh = true;
			} else if(t = dynamic_cast<Transform*>(c)) {
				hasTransform = true;
			}
			if(hasMesh && hasTransform) {
				if(t->position != t->prevPosition) {
					TranslateMesh(m, t->position - t->prevPosition);
					t->prevPosition = t->position;
				}
				if(t->rotation != t->prevRotation) {
					RotateMesh(m, Matrix4::RotationMatrixAroundPoint(t->position, t->rotation - t->prevRotation));
					t->prevRotation = t->rotation;
				}
				if(t->scale != t->prevScale) {
					ScaleMesh(m, Matrix3::ScaleMatrix(t->scale - t->prevScale));
					t->prevScale = t->scale;
				}
				break;
			}
		}
	}
}