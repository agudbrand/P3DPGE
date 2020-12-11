#include "Collider.h"
#include "Entities.h"
#include "physics/InertiaTensors.h"

Matrix LocalToWorldInertiaTensor(PhysEntity* entity, Matrix inertiaTensor) {
	Matrix inverseTransformation = Matrix::TransformationMatrix(entity->position, entity->rotation, entity->scale).Inverse();
	return inverseTransformation * Matrix::M3x3To4x4(inertiaTensor) * inverseTransformation.Transpose();
}



//// AABB ////

AABBCollider::AABBCollider(PhysEntity* entity, Vector3 halfDimensions, int8 collisionLayer) {
	this->entity = entity;
	this->halfDims = halfDimensions;
	this->inertiaTensor = InertiaTensors::SolidCuboid(2*abs(halfDims.x), 2*abs(halfDims.y), 2*abs(halfDims.z), entity->mass);
	this->collisionLayer = collisionLayer;
	entity->SetCollider(this);
}

AABBCollider::AABBCollider(BoxCollider* boxCollider) {
	this->entity = boxCollider->entity;
	this->halfDims = boxCollider->halfDims;
	this->inertiaTensor = InertiaTensors::SolidCuboid(2*abs(halfDims.x), 2*abs(halfDims.y), 2*abs(halfDims.z), entity->mass);
	this->collisionLayer = boxCollider->collisionLayer;
	//entity->SetCollider(this);
}

//TODO(o,delle) check if the compiler optimizes this to be inline
//Returns true if the point is within the bounds of the axis-aligned box
bool AABBCollider::ContainsPoint(Vector3 point) {
	bool checkX = point.x >= entity->position.x - halfDims.x && point.x <= entity->position.x + halfDims.x;
	bool checkY = point.y >= entity->position.y - halfDims.y && point.y <= entity->position.y + halfDims.y;
	bool checkZ = point.z >= entity->position.z - halfDims.z && point.z <= entity->position.z + halfDims.z;
	return checkX && checkY && checkZ;
}

//Returns the point on this AABBs surface closest to target point
Vector3 AABBCollider::ClosestPointOnSurfaceTo(Vector3 target) {
	return Vector3(fmaxf(entity->position.x - halfDims.x, fminf(target.x, entity->position.x + halfDims.x)),
					fmaxf(entity->position.y - halfDims.y, fminf(target.y, entity->position.y + halfDims.y)),
					fmaxf(entity->position.y - halfDims.z, fminf(target.z, entity->position.z + halfDims.z)));
}

//TODO(p,delle) implement aabb-aabb collision and resolution
bool AABBAABBCollision(AABBCollider* first, AABBCollider* second, bool resolveCollision) {
	ERROR("AABB-AABB collision not implemented in Collider.cpp");
	return false;
}

//TODO(p,delle) implement sphere-aabb dynamic resolution
//TODO(p,delle) maybe abstract out closest point calculations for each collider
//Returns true if the closest point to the sphere on the AABB is within the sphere
//Reference: https://www.euclideanspace.com/physics/dynamics/collision/threed/index.htm
bool AABBSphereCollision(AABBCollider* aabb, SphereCollider* sphere, bool resolveCollision) {
	Vector3 closestAABBPoint = aabb->ClosestPointOnSurfaceTo(sphere->entity->position);
	Vector3 vectorBetween = closestAABBPoint - sphere->entity->position; //sphere towards aabb
	float distanceBetween = vectorBetween.mag();
	if (distanceBetween < sphere->radius) {
		if (resolveCollision) {
			LOG("collision happened");
			//static resolution
			if (closestAABBPoint == sphere->entity->position) { 
				//NOTE if the closest point is the same, the vector between will have no direction; this 
				//is supposed to be a remedy to that by offsetting in the direction between thier centers
				vectorBetween = aabb->entity->position - sphere->entity->position;
			}
			float overlap = .5f * (sphere->radius - distanceBetween);
			vectorBetween = vectorBetween.normalized() * overlap;
			aabb->entity->position += vectorBetween;
			sphere->entity->position -= vectorBetween;
			
			//dynamic resolution
			Matrix sphereInertiaTensorInverse = LocalToWorldInertiaTensor(sphere->entity, sphere->inertiaTensor).Inverse();
			Vector3 normal = -vectorBetween.normalized();
			Vector3 ra = sphere->entity->position + sphere->ClosestPointOnSurfaceTo(closestAABBPoint);
			Vector3 sphereAngularVelocityChange = normal.cross(ra);
			sphereAngularVelocityChange = Vector3(Matrix(sphereAngularVelocityChange, 1) * sphereInertiaTensorInverse);
			float inverseMassA = 1.f / sphere->entity->mass;
			float scalar = inverseMassA + sphereAngularVelocityChange.cross(ra).dot(normal);

			Matrix aabbInertiaTensorInverse = LocalToWorldInertiaTensor(aabb->entity, aabb->inertiaTensor).Inverse();
			Vector3 rb = aabb->entity->position + closestAABBPoint;
			Vector3 aabbAngularVelocityChange = normal.cross(rb);
			aabbAngularVelocityChange = Vector3(Matrix(aabbAngularVelocityChange, 1) * aabbInertiaTensorInverse);
			float inverseMassB = 1.f / aabb->entity->mass;
			scalar += inverseMassB + aabbAngularVelocityChange.cross(rb).dot(normal);
				
			float coefRest = 1.f; //TODO(c,delle) remove this, only for testing
			float impulseMod = (coefRest + 1) * (sphere->entity->velocity - aabb->entity->velocity).mag() / scalar;
			Vector3 impulse = normal * impulseMod;
			sphere->entity->velocity -= impulse * inverseMassA; //TODO(p,delle) test this
			aabb->entity->velocity -= impulse * inverseMassB;
			sphere->entity->rotVelocity -= sphereAngularVelocityChange;
			//aabb->entity->rotVelocity -= aabbAngularVelocityChange; //we dont do this because AABB shouldnt rotate
		}
		return true;
	}
	return false;
}

//TODO(p,delle) implement aabb-box collision and resolution
bool AABBBoxCollision(AABBCollider* aabb, BoxCollider* box, bool resolveCollision) {
	ERROR("AABB-Box collision not implemented in Collider.cpp");
	return false;
}

//TODO(p,delle) implement aabb-complex collision and resolution
bool AABBComplexCollision(AABBCollider* aabb, ComplexCollider* complex, bool resolveCollision) {
	ERROR("AABB-Complex collision not implemented in Collider.cpp");
	return false;
}

//Returns true if there was a collision between this aabb collider and the other collider
//    -If resolveCollision is true, the two entities will be resolved statically so they dont occupy the same space and
//     dynamically so they maintain their momentum.
bool AABBCollider::CheckCollision(Collider* other, bool resolveCollision) {
	if (collisionLayer == other->collisionLayer) {
		if (SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other)) {
			return AABBSphereCollision(this, otherSphere, resolveCollision);
		} else if (AABBCollider* otherAABB = dynamic_cast<AABBCollider*>(other)) {
			return AABBAABBCollision(this, otherAABB, resolveCollision);
		} else if (BoxCollider* otherBox = dynamic_cast<BoxCollider*>(other)) {
			if (otherBox->entity->rotation == V3ZERO) {
				AABBCollider* tempAABB = new AABBCollider(otherBox); //TODO(p,delle) test this
				bool value = AABBAABBCollision(this, tempAABB, resolveCollision);
				delete tempAABB;
				return value;
			} else {
				return AABBBoxCollision(this, otherBox, resolveCollision);
			}
		} else if (ComplexCollider* otherComplex = dynamic_cast<ComplexCollider*>(other)) {
			return AABBComplexCollision(this, otherComplex, resolveCollision);
		} else {
			std::stringstream thisAddress; thisAddress << (void const*)this;
			std::stringstream otherAddress; thisAddress << (void const*)other;
			ERROR("AABB collider (" + thisAddress.str() + ") tried to collide with an unhandled collider (" + otherAddress.str() + ")");
		}
	}
	return false;
}



//// Sphere ////

SphereCollider::SphereCollider(Sphere* sphere, int8 collisionLayer) {
	this->entity = sphere;
	this->radius = sphere->radius;
	this->inertiaTensor = InertiaTensors::SolidSphere(radius, entity->mass);
	this->collisionLayer = collisionLayer;
	sphere->SetCollider(this);
}

SphereCollider::SphereCollider(PhysEntity* entity, float radius, int8 collisionLayer) {
	this->entity = entity;
	this->radius = radius;
	this->inertiaTensor = InertiaTensors::SolidSphere(radius, entity->mass);
	this->collisionLayer = collisionLayer;
	entity->SetCollider(this);
}

//Returns true if the distance between the point and the sphere's position is less than or equal to the sphere's radius
bool SphereCollider::ContainsPoint(Vector3 point) {
	return point.distanceTo(entity->position) <= radius;
}

//Returns the point on this spheres surface closest to target point
Vector3 SphereCollider::ClosestPointOnSurfaceTo(Vector3 target) {
	return (target - entity->position).normalized() * radius;
}

//TODO(p,delle) implement rotational collision resolution
//Returns true if the distance between the two spheres is less than thier combined radius
bool SphereSphereCollision(SphereCollider* first, SphereCollider* second, bool resolveCollision) {
	Vector3 vectorBetween = first->entity->position - second->entity->position; //other towards sphere
	float distanceBetween = vectorBetween.mag();
	if (distanceBetween <= (first->radius + second->radius)) {
		if (resolveCollision) {
			//static resolution
			float overlap = .5f * (distanceBetween - first->radius - second->radius);
			vectorBetween = vectorBetween.normalized() * overlap;
			first->entity->position -= vectorBetween;
			second->entity->position += vectorBetween;

			//dynamic resolution
			
		}
		return true;
	}
	return false;
}

//TODO(p,delle) implement sphere-box collision and resolution
bool SphereBoxCollision(SphereCollider* sphere, BoxCollider* box, bool resolveCollision) {
	ERROR("Sphere-Box collision not implemented in Collider.cpp");
	return false;
}

//TODO(p,delle) implement sphere-complex collision and resolution
bool SphereComplexCollision(SphereCollider* sphere, ComplexCollider* box, bool resolveCollision) {
	ERROR("Sphere-Complex collision not implemented in Collider.cpp");
	return false;
}

//Returns true if there was a collision between this sphere collider and the other collider
//    -If resolveCollision is true, the two entities will be resolved statically so they dont occupy the same space and
//     dynamically so they maintain their momentum.
bool SphereCollider::CheckCollision(Collider* other, bool resolveCollision) {
	if (collisionLayer == other->collisionLayer) {
		if (SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other)) {
			return SphereSphereCollision(this, otherSphere, resolveCollision);
		} else if (AABBCollider* otherAABB = dynamic_cast<AABBCollider*>(other)) {
			return AABBSphereCollision(otherAABB, this, resolveCollision);
		} else if (BoxCollider* otherBox = dynamic_cast<BoxCollider*>(other)) {
			if (otherBox->entity->rotation == V3ZERO) {
				AABBCollider* tempAABB = new AABBCollider(otherBox);
				bool value = AABBSphereCollision(tempAABB, this, resolveCollision);
				delete tempAABB;
				return value;
			} else {
				return SphereBoxCollision(this, otherBox, resolveCollision);
			}
		} else if (ComplexCollider* otherComplex = dynamic_cast<ComplexCollider*>(other)) {
			return SphereComplexCollision(this, otherComplex, resolveCollision);
		} else {
			std::stringstream thisAddress; thisAddress << (void const*)this;
			std::stringstream otherAddress; thisAddress << (void const*)other;
			ERROR("Sphere collider (" + thisAddress.str() + ") tried to collide with an unhandled collider (" + otherAddress.str() + ")");
		}
	}
	return false;
}



//// Box ////

BoxCollider::BoxCollider(Box* box, int8 collisionLayer) {
	this->entity = box;
	this->halfDims = box->halfDims;
	this->inertiaTensor = InertiaTensors::SolidCuboid(2*abs(halfDims.x), 2*abs(halfDims.y), 2*abs(halfDims.z), entity->mass);
	this->collisionLayer = collisionLayer;
}

BoxCollider::BoxCollider(PhysEntity* entity, Vector3 halfDimensions, int8 collisionLayer) {
	this->entity = entity;
	this->halfDims = halfDimensions;
	this->inertiaTensor = InertiaTensors::SolidCuboid(2*abs(halfDims.x), 2*abs(halfDims.y), 2*abs(halfDims.z), entity->mass);
	this->collisionLayer = collisionLayer;
}

//TODO(p,delle) implement 
bool BoxCollider::ContainsPoint(Vector3 point) {
	ERROR("BoxCollider-ContainsPoint not implemented in Collider.cpp");
	return false;
}

//TODO(p,delle) implement box-box collision and resolution
bool BoxBoxCollision(BoxCollider* first, BoxCollider* second, bool resolveCollision) {
	ERROR("Box-Box collision not implemented in Collider.cpp");
	return false;
}

//TODO(p,delle) implement box-complex collision and resolution
bool BoxComplexCollision(BoxCollider* box, ComplexCollider* complex, bool resolveCollision) {
	ERROR("Box-Complex collision not implemented in Collider.cpp");
	return false;
}

//Returns true if there was a collision between this box collider and the other collider
//    -If resolveCollision is true, the two entities will be resolved statically so they dont occupy the same space and
//     dynamically so they maintain their momentum.
bool BoxCollider::CheckCollision(Collider* other, bool resolveCollision) {
	if (collisionLayer == other->collisionLayer) {
		if (this->entity->rotation == V3ZERO) { //if no rotation, treat as AABB
			AABBCollider* tempThis = new AABBCollider(this);
			bool value = tempThis->CheckCollision(other, resolveCollision);
			delete tempThis;
			return value;
		} else { //has rotation, dont treat as AABB
			if (SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other)) {
				return SphereBoxCollision(otherSphere, this, resolveCollision);
			} else if (AABBCollider* otherAABB = dynamic_cast<AABBCollider*>(other)) {
				return AABBBoxCollision(otherAABB, this, resolveCollision);
			} else if (BoxCollider* otherBox = dynamic_cast<BoxCollider*>(other)) {
				if (otherBox->entity->rotation == V3ZERO) {
					AABBCollider* tempAABB = new AABBCollider(otherBox);
					bool value = AABBBoxCollision(tempAABB, this, resolveCollision);
					delete tempAABB;
					return value;
				} else {
					return BoxBoxCollision(this, otherBox, resolveCollision);
				}
			} else if (ComplexCollider* otherComplex = dynamic_cast<ComplexCollider*>(other)) {
				return BoxComplexCollision(this, otherComplex, resolveCollision);
			} else {
				std::stringstream thisAddress; thisAddress << (void const*)this;
				std::stringstream otherAddress; thisAddress << (void const*)other;
				ERROR("Box collider (" + thisAddress.str() + ") tried to collide with an unhandled collider (" + otherAddress.str() + ")");
			}
		}
	}
	return false;
}



//// Complex ////

//TODO(p,delle) implement complex collider interia tensor
ComplexCollider::ComplexCollider(PhysEntity* entity, std::vector<AABBCollider> colliders, int8 collisionLayer) {
	this->entity = entity;
	this->colliders = colliders;
	this->collisionLayer = collisionLayer;
	//this->inertiaTensor = ? ;
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
bool ComplexComplexCollision(ComplexCollider* first, ComplexCollider* second, bool resolveCollision) {
	ERROR("Complex-Complex collision not implemented in Collider.cpp");
	return false;
}

//Returns true if there was a collision between this complex collider and the other collider
//    -If resolveCollision is true, the two entities will be resolved statically so they dont occupy the same space and
//     dynamically so they maintain their momentum.
bool ComplexCollider::CheckCollision(Collider* other, bool resolveCollision) {
	if (collisionLayer == other->collisionLayer) {
		if (SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other)) {
			return SphereComplexCollision(otherSphere, this, resolveCollision);
		} else if (AABBCollider* otherAABB = dynamic_cast<AABBCollider*>(other)) {
			return AABBComplexCollision(otherAABB, this, resolveCollision);
		} else if (BoxCollider* otherBox = dynamic_cast<BoxCollider*>(other)) {
			if (otherBox->entity->rotation == V3ZERO) {
				AABBCollider* tempAABB = new AABBCollider(otherBox);
				bool value = AABBComplexCollision(tempAABB, this, resolveCollision);
				delete tempAABB;
				return value;
			} else {
				return BoxComplexCollision(otherBox, this, resolveCollision);
			}
		} else if (ComplexCollider* otherComplex = dynamic_cast<ComplexCollider*>(other)) {
			return ComplexComplexCollision(this, otherComplex, resolveCollision);
		} else {
			std::stringstream thisAddress; thisAddress << (void const*)this;
			std::stringstream otherAddress; thisAddress << (void const*)other;
			ERROR("Complex collider (" + thisAddress.str() + ") tried to collide with an unhandled collider (" + otherAddress.str() + ")");
		}
	}
	return false;
}