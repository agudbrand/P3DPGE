#pragma once
#include "math/Math.h"
#include <vector>

class PhysEntity;
struct Sphere;
struct Box;
struct Complex;

struct Collider{
	PhysEntity* entity = nullptr;
	Matrix3 inertiaTensor;
	int8 collisionLayer;

	virtual Vector3 ClosestPointOnSurfaceTo(Vector3 target) = 0;
	virtual bool ContainsPoint(Vector3 point) = 0;
	virtual bool CheckCollision(Collider* other, bool resolveCollision = true) = 0;
};

//Rotatable box based on BoxEntity's transform
struct BoxCollider : public Collider {
	Vector3 halfDims; //half dimensions, entity's position to the bounding box's locally positive corner

	BoxCollider(Box* box, int8 collisionLayer = 0);
	BoxCollider(PhysEntity* entity, Vector3 halfDimensions, int8 collisionLayer = 0);

	bool ContainsPoint(Vector3 point) override;
	bool CheckCollision(Collider* other, bool resolveCollision = true) override;
};

//Axis-aligned bounding box (no rotation)
struct AABBCollider : public Collider {
	Vector3 halfDims; //half dimensions, entity's position to the bounding box's locally positive corner
	
	AABBCollider(PhysEntity* entity, Vector3 halfDimensions, int8 collisionLayer = 0);
	AABBCollider(BoxCollider* boxCollider);

	bool ContainsPoint(Vector3 point) override;
	Vector3 ClosestPointOnSurfaceTo(Vector3 target) override;
	bool CheckCollision(Collider* other, bool resolveCollision = true) override;
};

struct SphereCollider : public Collider {
	float radius;

	SphereCollider(Sphere* sphere, int8 collisionLayer = 0);
	SphereCollider(PhysEntity* entity, float radius, int8 collisionLayer = 0);

	bool ContainsPoint(Vector3 point) override;
	Vector3 ClosestPointOnSurfaceTo(Vector3 target) override;
	bool CheckCollision(Collider* other, bool resolveCollision = true) override;
};

//NOTE this could instead be a group of any kind of collider and just check contains
//point  on all those colliders, but this seems the best/simplest solution to me -delle
//A group of AABB colliders
struct ComplexCollider : public Collider {
	std::vector<AABBCollider> colliders;

	ComplexCollider(PhysEntity* entity, std::vector<AABBCollider> colliders, int8 collisionLayer = 0);

	bool ContainsPoint(Vector3 point) override;
	bool CheckCollision(Collider* other, bool resolveCollision = true) override;
};

//struct ConvexPolyCollider : public Collider {
//
//
//};

//struct CapsuleCollider : public Collider {
//
//};

