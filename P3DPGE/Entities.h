#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"

namespace qvm = boost::qvm;

struct Triangle {
	Vector3 points[3];

	Triangle(Vector3 p1, Vector3 p2, Vector3 p3) {
		points[0] = p1;
		points[1] = p2;
		points[2] = p3;
	}
};

struct Mesh {
	std::vector<Triangle> triangles;

	void Draw(olc::PixelGameEngine* p, olc::Pixel color) {
		for (auto& t : triangles) {
			p->DrawTriangle(
				t.points[0].x, t.points[0].y, 
				t.points[1].x, t.points[1].y, 
				t.points[2].x, t.points[2].y, color);
		}
	}
};

#define EntityArgs id, position, rotation, scale
#define EntityParams Vector3 position = V3ZERO, Vector3 rotation = V3ZERO, Vector3 scale = V3ONE
class Entity {
public:
	//transform
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	//info
	int id;
	std::string tag;

	//mesh
	olc::Pixel color = olc::WHITE;
	//DO NOT make a pointer unless you're 
	//prepared to solve the read access violation
	//that comes with it
	Mesh mesh;
	 
	Entity() {}
	Entity(int id, EntityParams) {
		this->id = id;
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
	}
	virtual ~Entity() {}

	// User must override these functions as required. I have not made
	// them abstract because I do need a default behaviour to occur if
	// they are not overwritten
	//TODO: this ^
	virtual void Draw(olc::PixelGameEngine* p) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual bool ContainsPoint(Vector3 point) = 0;

	virtual void RotateX() = 0;

	void SetTag(std::string newTag);
	void SetColor(olc::Pixel newColor);
};

#define PhysEntityArgs velocity, acceleration, rotVelocity, rotAcceleration, mass, bStatic
#define PhysEntityParams Vector3 velocity = V3ZERO, Vector3 acceleration = V3ZERO, Vector3 rotVelocity = V3ZERO, Vector3 rotAcceleration = V3ZERO, float mass = 1, bool bStatic = false
class PhysEntity : public Entity{
public:
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 rotVelocity;
	Vector3 rotAcceleration;
	float mass;
	bool bStatic;

	PhysEntity() : Entity() {}
	PhysEntity(int id, EntityParams, PhysEntityParams) : Entity(EntityArgs) {
		this->velocity = velocity;
		this->acceleration = acceleration;
		this->rotVelocity = rotVelocity;
		this->rotAcceleration = rotAcceleration;
		this->mass = mass;
		this->bStatic = bStatic;
	};

	void Update(float deltaTime) override;

	void AddForce(PhysEntity* creator, Vector3 force, bool bIgnoreMass = false);
	void GenerateRadialForce(Vector3 position, float radius, float strength, float falloff, bool bIgnoreMass);
		
};

struct Sphere : public PhysEntity {
	float radius;

	Sphere() : PhysEntity(){}
	Sphere(float r, int id, EntityParams, PhysEntityParams) : PhysEntity(EntityArgs, PhysEntityArgs) {
		this->radius = r;
	}

	void RotateX() override;
	void Draw(olc::PixelGameEngine* p) override;
	bool ContainsPoint(Vector3 point) override;
};

struct Box : public PhysEntity {
	Vector3 dimensions; //full dimensions

	Box() : PhysEntity(){}
	Box(Vector3 dimensions, int id, EntityParams, PhysEntityParams) : PhysEntity(EntityArgs, PhysEntityArgs){
		this->dimensions = dimensions;

		//vertices making up the prism

		Vector3 p1 = position;
		Vector3 p2 = position + dimensions.xComp();
		Vector3 p3 = position + dimensions.yComp();
		Vector3 p4 = position + dimensions.zComp();
		Vector3 p5 = position + dimensions.yComp() + dimensions.zComp();
		Vector3 p6 = position + dimensions.xComp() + dimensions.zComp();
		Vector3 p7 = position + dimensions.xComp() + dimensions.yComp();
		Vector3 p8 = position + dimensions;

		//generate face triangles
		//west
		mesh.triangles.push_back(Triangle(p3, p4, p1));
		mesh.triangles.push_back(Triangle(p3, p5, p4));
		//top
		mesh.triangles.push_back(Triangle(p4, p1, p2));
		mesh.triangles.push_back(Triangle(p4, p2, p6));
		//east
		mesh.triangles.push_back(Triangle(p7, p2, p6));
		mesh.triangles.push_back(Triangle(p7, p6, p8));
		//bottom
		mesh.triangles.push_back(Triangle(p5, p3, p7));
		mesh.triangles.push_back(Triangle(p5, p7, p8));
		//south
		mesh.triangles.push_back(Triangle(p5, p4, p6));
		mesh.triangles.push_back(Triangle(p5, p6, p8));
		//north
		mesh.triangles.push_back(Triangle(p3, p1, p2));
		mesh.triangles.push_back(Triangle(p3, p2, p7));

		std::cout << mesh.triangles.size() << std::endl;
	}
	
	void RotateX() override;

	void Draw(olc::PixelGameEngine* p) override;
	bool ContainsPoint(Vector3 point) override;
};