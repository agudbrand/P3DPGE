#pragma once
#include "Math.h"
#include <stdint.h>
#include <vector>

struct PhysEntity;
struct Sphere;
struct Box;
struct Complex;

struct Collider{
	uint8_t collisionLayer;

	virtual bool ContainsPoint(Vector3 point) = 0;
	virtual bool CheckCollision(Collider* other, bool resolveCollision = true) = 0;
};

//Rotatable box based on BoxEntity's transform
struct BoxCollider : public Collider {
	Box* box;

	BoxCollider(Box* box);

	bool ContainsPoint(Vector3 point) override;
	bool CheckCollision(Collider* other, bool resolveCollision = true) override;
};

//Axis-aligned bounding box (no rotation)
struct AABBCollider : public Collider {
	PhysEntity* entity;
	Vector3 halfDims; //half dimensions, entity's position to the bounding box's locally positive corner

	AABBCollider(PhysEntity* entity, Vector3 halfDimensions);
	AABBCollider(BoxCollider* boxCollider);
	~AABBCollider() {}

	bool ContainsPoint(Vector3 point) override;
	bool CheckCollision(Collider* other, bool resolveCollision = true) override;
};

struct SphereCollider : public Collider {
	Sphere* sphere;

	SphereCollider(Sphere* sphere);

	bool ContainsPoint(Vector3 point) override;
	bool CheckCollision(Collider* other, bool resolveCollision = true) override;
};

//NOTE this could instead be any kind of collider and just check contains point 
//on all those colliders, but this seems the best/simplest solution to me -delle
//A group of AABB colliders
struct ComplexCollider : public Collider {
	Complex* complex;
	std::vector<AABBCollider> colliders;

	ComplexCollider(Complex* complex, std::vector<AABBCollider> colliders);

	bool ContainsPoint(Vector3 point) override;
	bool CheckCollision(Collider* other, bool resolveCollision = true) override;
};

//struct ConvexPolyCollider : public Collider {
//
//
//};
