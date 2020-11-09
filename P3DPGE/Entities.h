#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"

#define EntityArgs id, position, rotation, scale
#define EntityParams Vector3 position = V3ZERO, Vector3 rotation = V3ZERO, Vector3 scale = V3ONE
struct Entity {
		//transform
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;

		//info
		int id;
		std::string tag;

		//mesh
		olc::Pixel color = olc::WHITE;

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

		void SetTag(std::string newTag);
		void SetColor(olc::Pixel newColor);
};

#define PhysEntityArgs velocity, acceleration, rotVelocity, rotAcceleration, mass, bStatic
#define PhysEntityParams Vector3 velocity = V3ZERO, Vector3 acceleration = V3ZERO, Vector3 rotVelocity = V3ZERO, Vector3 rotAcceleration = V3ZERO, float mass = 1, bool bStatic = false
struct PhysEntity : public Entity{
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

struct Box : public PhysEntity {
		Vector3 dimensions; //full dimensions

		Box() : PhysEntity(){}
		Box(Vector3 dimensions, int id, EntityParams, PhysEntityParams) : PhysEntity(EntityArgs, PhysEntityArgs){
			this->dimensions = dimensions;
		}
	
		void Draw(olc::PixelGameEngine* p) override;
		bool ContainsPoint(Vector3 point) override;
};