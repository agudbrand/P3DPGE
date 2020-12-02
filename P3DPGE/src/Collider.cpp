#include "Collider.h"

#include "Entities.h"

//// AABB ////

AABBCollider::AABBCollider(PhysEntity* entity, Vector3 halfDimenstions) {
	this->entity = entity;
	this->halfDims = halfDimenstions;
}

AABBCollider::AABBCollider(BoxCollider* boxCollider) {
	this->entity = (PhysEntity*)boxCollider;
	this->halfDims = boxCollider->box->dimensions;
}

//TODO(o,delle) check if the compiler optimizes this to be inline
//Returns true if the point is within the bounds of the axis-aligned box
bool AABBCollider::ContainsPoint(Vector3 point) {
	bool checkX = point.x >= entity->position.x - halfDims.x && point.x <= entity->position.x + halfDims.x;
	bool checkY = point.y >= entity->position.y - halfDims.y && point.y <= entity->position.y + halfDims.y;
	bool checkZ = point.z >= entity->position.z - halfDims.z && point.z <= entity->position.z + halfDims.z;
	return checkX && checkY && checkZ;
}

//TODO(p,delle) implement aabb-aabb collision and resolution
bool AABBAABBCollision(AABBCollider* first, AABBCollider* second, bool resolveCollision) {
	Debug::Error("AABB-AABB collision not implemented in Collider.cpp");
	return false;
}

//TODO(p,delle) implement sphere-aabb dynamic resolution
//TODO(p,delle) maybe abstract out closest point calculations for each collider
//Returns true if the closest point to the sphere on the AABB is within the sphere
bool AABBSphereCollision(AABBCollider* aabb, Sphere* sphere, bool resolveCollision) {
	Debug::Error("AABB-Sphere collision resolution not implemented in Collider.cpp");
	Vector3 closestAABBPoint(fmaxf(-aabb->halfDims.x, fminf(sphere->position.x, aabb->halfDims.x)),
							 fmaxf(-aabb->halfDims.y, fminf(sphere->position.y, aabb->halfDims.y)),
							 fmaxf(-aabb->halfDims.z, fminf(sphere->position.z, aabb->halfDims.z)));
	Vector3 vectorBetween = closestAABBPoint - sphere->position; //sphere towards aabb
	float distanceBetween = vectorBetween.mag();
	if (distanceBetween < sphere->radius) {
		if (resolveCollision) {
			//static resolution
			if (closestAABBPoint == sphere->position) { 
				//NOTE if the closest point is the same, the vector between will have no direction; this 
				//is supposed to be a remedy to that by offsetting in the direction between thier centers
				vectorBetween = aabb->entity->position - sphere->position;
			}
			float overlap = .5f * (sphere->radius - distanceBetween);
			vectorBetween = vectorBetween.normalized() * overlap;
			aabb->entity->position += vectorBetween; //TODO(p,delle) test this
			sphere->position -= vectorBetween;

			//dynamic resolution
			mat<float, 3, 3> sphereInertiaTensorInverse = boost::qvm::inverse(InertiaTensors::LocalToWorld(sphere, InertiaTensors::SolidSphere(sphere->radius, sphere->mass)));
			Vector3 normal = vectorBetween.normalized();

		}
		return true;
	}
	return false;
}

//TODO(p,delle) implement aabb-box collision and resolution
bool AABBBoxCollision(AABBCollider* aabbCollider, Box* box, bool resolveCollision) {
	Debug::Error("AABB-Box collision not implemented in Collider.cpp");
	return false;
}

//TODO(p,delle) implement aabb-complex collision and resolution
bool AABBComplexCollision(AABBCollider* aabbCollider, Complex* complex, bool resolveCollision) {
	Debug::Error("AABB-Complex collision not implemented in Collider.cpp");
	return false;
}

//Returns true if there was a collision between this box collider and the other collider
//    -If resolveCollision is true, the two entities will be resolved statically so they dont occupy the same space and
//     dynamically so they maintain their momentum.
bool AABBCollider::CheckCollision(Collider* other, bool resolveCollision) {
	if (collisionLayer == other->collisionLayer) {
		if (SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other)) {
			return AABBSphereCollision(this, otherSphere->sphere, resolveCollision);
		} else if (AABBCollider* otherAABB = dynamic_cast<AABBCollider*>(other)) {
			return AABBAABBCollision(this, otherAABB, resolveCollision);
		} else if (BoxCollider* otherBox = dynamic_cast<BoxCollider*>(other)) {
			if (otherBox->box->rotation == V3ZERO) {
				AABBCollider* tempAABB = new AABBCollider(otherBox); //TODO(p,delle) test this
				bool value = AABBAABBCollision(this, tempAABB, resolveCollision);
				delete tempAABB;
				return value;
			} else {
				return AABBBoxCollision(this, otherBox->box, resolveCollision);
			}
		} else if (ComplexCollider* otherComplex = dynamic_cast<ComplexCollider*>(other)) {
			return AABBComplexCollision(this, otherComplex->complex, resolveCollision);
		} else {
			std::stringstream thisAddress; thisAddress << (void const*)this;
			std::stringstream otherAddress; thisAddress << (void const*)other;
			Debug::Error("AABB collider (" + thisAddress.str() + ") tried to collide with an unhandled collider (" + otherAddress.str() + ")");
		}
	}
	return false;
}

//// Sphere ////

SphereCollider::SphereCollider(Sphere* sphere) {
	this->sphere = sphere;
}

//Returns true if the distance between the point and the sphere's position is less than or equal to the sphere's radius
bool SphereCollider::ContainsPoint(Vector3 point) {
	return point.distanceTo(sphere->position) <= sphere->radius;
}

//TODO(p,delle) implement rotational collision resolution
//Returns true if the distance between the two spheres is less than thier combined radius
bool SphereSphereCollision(Sphere* first, Sphere* second, bool resolveCollision) {
	Vector3 vectorBetween = first->position - second->position; //other towards sphere
	float distanceBetween = vectorBetween.mag();
	if (distanceBetween <= (first->radius + second->radius)) {
		if (resolveCollision) {
			//static resolution
			float overlap = .5f * (distanceBetween - first->radius - second->radius);
			vectorBetween = vectorBetween.normalized() * overlap;
			first->position -= vectorBetween;
			second->position += vectorBetween;

			//dynamic resolution without rotation, From wikipedia
			//https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional_collision_with_two_moving_objects
			Vector3 normal = (second->position - first->position) / distanceBetween;
			float p = 2.f * (normal.dot(first->velocity - second->velocity)) / (first->mass + second->mass);
			first->velocity -= normal * p * second->mass;
			second->velocity += normal * p * first->mass; //TODO(p,delle) test this in 3d
		}
		return true;
	}
	return false;
}

//TODO(p,delle) implement sphere-box collision and resolution
bool SphereBoxCollision(Sphere* sphere, Box* box, bool resolveCollision) {
	Debug::Error("Sphere-Box collision not implemented in Collider.cpp");
	return false;
}

//TODO(p,delle) implement sphere-complex collision and resolution
bool SphereComplexCollision(Sphere* sphere, Complex* box, bool resolveCollision) {
	Debug::Error("Sphere-Complex collision not implemented in Collider.cpp");
	return false;
}

//Returns true if there was a collision between this sphere collider and the other collider
//    -If resolveCollision is true, the two entities will be resolved statically so they dont occupy the same space and
//     dynamically so they maintain their momentum.
bool SphereCollider::CheckCollision(Collider* other, bool resolveCollision) {
	if (collisionLayer == other->collisionLayer) {
		if (SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other)) {
			return SphereSphereCollision(this->sphere, otherSphere->sphere, resolveCollision);
		} else if (AABBCollider* otherAABB = dynamic_cast<AABBCollider*>(other)) {
			return AABBSphereCollision(otherAABB, this->sphere, resolveCollision);
		} else if (BoxCollider* otherBox = dynamic_cast<BoxCollider*>(other)) {
			if (otherBox->box->rotation == V3ZERO) {
				AABBCollider* tempAABB = new AABBCollider(otherBox);
				bool value = AABBSphereCollision(tempAABB, this->sphere, resolveCollision);
				delete tempAABB;
				return value;
			} else {
				return SphereBoxCollision(this->sphere, otherBox->box, resolveCollision);
			}
		} else if (ComplexCollider* otherComplex = dynamic_cast<ComplexCollider*>(other)) {
			return SphereComplexCollision(this->sphere, otherComplex->complex, resolveCollision);
		} else {
			std::stringstream thisAddress; thisAddress << (void const*)this;
			std::stringstream otherAddress; thisAddress << (void const*)other;
			Debug::Error("Sphere collider (" + thisAddress.str() + ") tried to collide with an unhandled collider (" + otherAddress.str() + ")");
		}
	}
	return false;
}

//// Box ////

BoxCollider::BoxCollider(Box* box) {
	this->box = box;
}

//TODO(p,delle) implement 
bool BoxCollider::ContainsPoint(Vector3 point) {
	Debug::Error("BoxCollider-ContainsPoint not implemented in Collider.cpp");
	return false;
}

//TODO(p,delle) implement box-box collision and resolution
bool BoxBoxCollision(Box* first, Box* second, bool resolveCollision) {
	Debug::Error("Box-Box collision not implemented in Collider.cpp");
	return false;
}

//TODO(p,delle) implement box-complex collision and resolution
bool BoxComplexCollision(Box* box, Complex* complex, bool resolveCollision) {
	Debug::Error("Box-Complex collision not implemented in Collider.cpp");
	return false;
}

//Returns true if there was a collision between this box collider and the other collider
//    -If resolveCollision is true, the two entities will be resolved statically so they dont occupy the same space and
//     dynamically so they maintain their momentum.
bool BoxCollider::CheckCollision(Collider* other, bool resolveCollision) {
	if (collisionLayer == other->collisionLayer) {
		if (this->box->rotation == V3ZERO) { //if no rotation, treat as AABB
			AABBCollider* tempThis = new AABBCollider(this);
			if (SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other)) {
				bool value = AABBSphereCollision(tempThis, otherSphere->sphere, resolveCollision);
				delete tempThis;
				return value;
			} else if (AABBCollider* otherAABB = dynamic_cast<AABBCollider*>(other)) {
				bool value = AABBAABBCollision(tempThis, otherAABB, resolveCollision);
				delete tempThis;
				return value;
			} else if (BoxCollider* otherBox = dynamic_cast<BoxCollider*>(other)) {
				if (otherBox->box->rotation == V3ZERO) {
					AABBCollider* tempAABB = new AABBCollider(otherBox);
					bool value = AABBAABBCollision(tempThis, tempAABB, resolveCollision);
					delete tempThis;
					return value;
				} else {
					bool value = AABBBoxCollision(tempThis, otherBox->box, resolveCollision);
					delete tempThis;
					return value;
				}
			} else if (ComplexCollider* otherComplex = dynamic_cast<ComplexCollider*>(other)) {
				bool value = AABBComplexCollision(tempThis, otherComplex->complex, resolveCollision);
				delete tempThis;
				return value;
			} else {
				std::stringstream thisAddress; thisAddress << (void const*)this;
				std::stringstream otherAddress; thisAddress << (void const*)other;
				Debug::Error("Box collider (" + thisAddress.str() + ") tried to collide with an unhandled collider (" + otherAddress.str() + ")");
			}
		} else { //has rotation, dont treat as AABB
			if (SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other)) {
				return SphereBoxCollision(otherSphere->sphere, this->box, resolveCollision);
			} else if (AABBCollider* otherAABB = dynamic_cast<AABBCollider*>(other)) {
				return AABBBoxCollision(otherAABB, box, resolveCollision);
			} else if (BoxCollider* otherBox = dynamic_cast<BoxCollider*>(other)) {
				if (otherBox->box->rotation == V3ZERO) {
					AABBCollider* tempAABB = new AABBCollider(otherBox);
					bool value = AABBBoxCollision(tempAABB, this->box, resolveCollision);
					delete tempAABB;
					return value;
				} else {
					return BoxBoxCollision(this->box, otherBox->box, resolveCollision);
				}
			} else if (ComplexCollider* otherComplex = dynamic_cast<ComplexCollider*>(other)) {
				return BoxComplexCollision(this->box, otherComplex->complex, resolveCollision);
			} else {
				std::stringstream thisAddress; thisAddress << (void const*)this;
				std::stringstream otherAddress; thisAddress << (void const*)other;
				Debug::Error("Box collider (" + thisAddress.str() + ") tried to collide with an unhandled collider (" + otherAddress.str() + ")");
			}
		}
	}
	return false;
}

//// Complex ////

ComplexCollider::ComplexCollider(Complex* complex, std::vector<AABBCollider> colliders) {
	this->complex = complex;
	this->colliders = colliders;
}

//Returns true if the point is within the bounds of one of the box colliders
bool ComplexCollider::ContainsPoint(Vector3 point) {
	for (AABBCollider aabb : colliders) {
		if (aabb.ContainsPoint(point)) {
			return true;
		}
	}
	return false;
}

//TODO(p,delle) implement complex-complex collision and resolution
bool ComplexComplexCollision(Complex* box, Complex* complex, bool resolveCollision) {
	Debug::Error("Complex-Complex collision not implemented in Collider.cpp");
	return false;
}

//Returns true if there was a collision between this complex collider and the other collider
//    -If resolveCollision is true, the two entities will be resolved statically so they dont occupy the same space and
//     dynamically so they maintain their momentum.
bool ComplexCollider::CheckCollision(Collider* other, bool resolveCollision) {
	if (collisionLayer == other->collisionLayer) {
		if (SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other)) {
			return SphereComplexCollision(otherSphere->sphere, this->complex, resolveCollision);
		} else if (AABBCollider* otherAABB = dynamic_cast<AABBCollider*>(other)) {
			return AABBComplexCollision(otherAABB, this->complex, resolveCollision);
		} else if (BoxCollider* otherBox = dynamic_cast<BoxCollider*>(other)) {
			if (otherBox->box->rotation == V3ZERO) {
				AABBCollider* tempAABB = new AABBCollider(otherBox);
				bool value = AABBComplexCollision(tempAABB, this->complex, resolveCollision);
				delete tempAABB;
				return value;
			} else {
				return BoxComplexCollision(otherBox->box, this->complex, resolveCollision);
			}
		} else if (ComplexCollider* otherComplex = dynamic_cast<ComplexCollider*>(other)) {
			return ComplexComplexCollision(this->complex, otherComplex->complex, resolveCollision);
		} else {
			std::stringstream thisAddress; thisAddress << (void const*)this;
			std::stringstream otherAddress; thisAddress << (void const*)other;
			Debug::Error("Complex collider (" + thisAddress.str() + ") tried to collide with an unhandled collider (" + otherAddress.str() + ")");
		}
	}
	return false;
}