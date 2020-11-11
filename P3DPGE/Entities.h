#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"

namespace qvm = boost::qvm;

//collection of 3 points forming the basis of meshes
struct Triangle {
	Vector3 points[3];
	Vector3 projectedPoints[3];

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
		Vector3 l1 = projectedPoints[1] - projectedPoints[0];
		Vector3 l2 = projectedPoints[2] - projectedPoints[0];
		return l2.cross(l1).normalized();
	}
};

//a collection of triangles that make up the geomery of objects in space
struct Mesh {
	std::vector<Triangle> triangles;

	void Draw(olc::PixelGameEngine* p, olc::Pixel color, Vector3 pos) {

		for (auto& t : triangles) {
			//camera is currently permenantly at zero
			Vector3 camera(0, 0, 0);

			if (t.get_normal().dot(t.points[0] - camera) > 0) {
				p->DrawTriangle(
					t.projectedPoints[0].x, t.projectedPoints[0].y,
					t.projectedPoints[1].x, t.projectedPoints[1].y,
					t.projectedPoints[2].x, t.projectedPoints[2].y, color);
			}
		}
	}
};

#define EntityArgs id, position, rotation, scale
#define EntityParams Vector3 position = V3ZERO, Vector3 rotation = V3ZERO, Vector3 scale = V3ONE
//the basis of all objects drawn on screen or otherwise, includes basic information such
//as position, rotation, scale, tags, etc.
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
	//DO NOT make a pointer unless you're prepared to solve the read access violation that comes with it
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

	//these functions are virtual but aren't implemented
	//in any child yet as I see no use for differenciating
	//between them yet
	virtual void RotateX();
	virtual void RotateY();
	virtual void RotateZ();
	//TODO: this function does not work well with rotating as if they are called
	//on the same frame consistently it will begin oscillating around the axis
	//that's being rotated over. most likely have to define an order
	virtual void Translate(Vector3 translation);
	virtual void ProjectToScreen(mat<float, 4, 4> ProjMat, olc::PixelGameEngine* p);

	void SetTag(std::string newTag);
	void SetColor(olc::Pixel newColor);
};

#define PhysEntityArgs velocity, acceleration, rotVelocity, rotAcceleration, mass, bStatic
#define PhysEntityParams Vector3 velocity = V3ZERO, Vector3 acceleration = V3ZERO, Vector3 rotVelocity = V3ZERO, Vector3 rotAcceleration = V3ZERO, float mass = 1, bool bStatic = false
//the physics based implentation of Entity, anything that moves in time is this
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
	void Draw(olc::PixelGameEngine* p) override;
	bool ContainsPoint(Vector3 point) override;
};

//formed by a single dimention vector
struct Box : public PhysEntity {
	Vector3 dimensions; //full dimensions

	Box() : PhysEntity(){}
	Box(Vector3 dimensions, int id, EntityParams, PhysEntityParams) : PhysEntity(EntityArgs, PhysEntityArgs){
		this->dimensions = dimensions;

		//vertices making up the box
		Vector3 p1 = position;
		Vector3 p2 = position + dimensions.xComp();
		Vector3 p3 = position + dimensions.yComp();
		Vector3 p4 = position + dimensions.zComp();
		Vector3 p5 = position + dimensions.yComp() + dimensions.zComp();
		Vector3 p6 = position + dimensions.xComp() + dimensions.zComp();
		Vector3 p7 = position + dimensions.xComp() + dimensions.yComp();
		Vector3 p8 = position + dimensions;

		//west
		mesh.triangles.push_back(Triangle(p3, p1, p4));
		mesh.triangles.push_back(Triangle(p3, p4, p5));
		//top
		mesh.triangles.push_back(Triangle(p4, p1, p2));
		mesh.triangles.push_back(Triangle(p4, p2, p6));
		//east
		mesh.triangles.push_back(Triangle(p8, p6, p2));
		mesh.triangles.push_back(Triangle(p8, p2, p7));
		//bottom
		mesh.triangles.push_back(Triangle(p3, p5, p8));
		mesh.triangles.push_back(Triangle(p3, p8, p7));
		//south
		mesh.triangles.push_back(Triangle(p5, p4, p6));
		mesh.triangles.push_back(Triangle(p5, p6, p8));
		//north
		mesh.triangles.push_back(Triangle(p7, p2, p1));
		mesh.triangles.push_back(Triangle(p7, p1, p3));

		//TODO: convert the vertices to be offsets of the center of the prism
		//or just add a center point and make sure its position is defined there
	}
	
	void Draw(olc::PixelGameEngine* p) override;
	bool ContainsPoint(Vector3 point) override;
};

//archaic camera class for now
//in fact its nothing
struct Camera : public Entity {

};