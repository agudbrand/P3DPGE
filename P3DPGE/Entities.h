#pragma once
#include "Physics.h"

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

		Entity() {}
		Entity(int id, EntityParams) {
			this->id = id;
			this->position = position;
			this->rotation = rotation;
			this->scale = scale;
		}

		virtual void Draw(olc::PixelGameEngine* p, olc::Pixel color = olc::WHITE) = 0;
		virtual void Update(float deltaTime) = 0;

		void SetTag(std::string newTag);
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

class Sphere : public PhysEntity {
	public:
		float r;

		Sphere() : PhysEntity(){}
		Sphere(float r, int id, EntityParams, PhysEntityParams) : PhysEntity(EntityArgs, PhysEntityArgs) {
			this->r = r;
		}

		void Draw(olc::PixelGameEngine* p, olc::Pixel color = olc::WHITE) override;
};

class Box : public PhysEntity {
	public:
		Vector3 dimensions;

		Box() : PhysEntity(){}
		Box(Vector3 dimensions, int id, EntityParams, PhysEntityParams) : PhysEntity(EntityArgs, PhysEntityArgs){
			this->dimensions = dimensions;
		}
	
		void Draw(olc::PixelGameEngine* p, olc::Pixel color = olc::WHITE) override;

};