#pragma once
#include "math/Math.h"
#include <vector>

class PhysEntity;
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
	Vector3 ClosestPointOnSurface(Vector3 target);
	bool CheckCollision(Collider* other, bool resolveCollision = true) override;
};

struct SphereCollider : public Collider {
	Sphere* sphere;

	SphereCollider(Sphere* sphere);

	bool ContainsPoint(Vector3 point) override;
	bool CheckCollision(Collider* other, bool resolveCollision = true) override;

	static Vector3 ClosestPointOnSurface(Sphere* sphere, Vector3 target);
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

//struct CapsuleCollider : public Collider {
//
//};

namespace InertiaTensors {
	static float twoThirds = (2.f / 3.f);
	static float oneTwelfth = (1.f / 12.f);

	static mat<float, 3, 3> SolidSphere(float radius, float mass) {
		float value = .4f * mass * radius * radius;
		return mat<float, 3, 3>{
			value, 0, 0,
				0, value, 0,
				0, 0, value
		};
	}

	static mat<float, 3, 3> HollowSphere(float radius, float mass) {
		float value = twoThirds * mass * radius * radius;
		return mat<float, 3, 3>{
			value, 0, 0,
				0, value, 0,
				0, 0, value
		};
	}

	static mat<float, 3, 3> SolidEllipsoid(Vector3 halfDimensions, float mass) {
		float oneFifthMass = .2f * mass;
		float aSqrd = halfDimensions.x * halfDimensions.x;
		float bSqrd = halfDimensions.y * halfDimensions.y;
		float cSqrd = halfDimensions.z * halfDimensions.z;
		return mat<float, 3, 3>{
			oneFifthMass* (bSqrd + cSqrd), 0, 0,
				0, oneFifthMass* (bSqrd + cSqrd), 0,
				0, 0, oneFifthMass* (bSqrd + cSqrd)
		};
	}

	static mat<float, 3, 3> SolidCuboid(float width, float height, float depth, float mass) {
		float oneTwelfthMass = oneTwelfth * mass;
		float wSqrd = width * width;
		float hSqrd = height * height;
		float dSqrd = depth * depth;
		return mat<float, 3, 3>{
			oneTwelfthMass* (hSqrd + dSqrd), 0, 0,
				0, oneTwelfthMass* (wSqrd + dSqrd), 0,
				0, 0, oneTwelfthMass* (wSqrd + hSqrd)
		};
	}

	static mat<float, 3, 3> SolidCylinder(float radius, float height, float mass) {
		float rSqrd = radius * radius;
		float value = oneTwelfth * mass * (3 * rSqrd + height * height);
		return mat<float, 3, 3>{
			value, 0, 0,
				0, value, 0,
				0, 0, .5f * mass * rSqrd
		};
	}
};