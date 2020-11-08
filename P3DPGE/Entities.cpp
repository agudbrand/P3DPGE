#include "Entities.h"

//// Entity	////

void Entity::SetTag(std::string newTag){
	tag = newTag;
}

//// Physics Entity ////

void PhysEntity::Update(float deltaTime){
	if(!bStatic) {
		velocity += acceleration * deltaTime;
		position += velocity * deltaTime;

		rotVelocity += rotAcceleration * deltaTime;
		rotation += rotVelocity * deltaTime;
	}
}

void PhysEntity::AddForce(PhysEntity* creator, Vector3 force, bool bIgnoreMass){
	this->acceleration += bIgnoreMass ? force : force / mass;
	if (creator) { creator->acceleration -= bIgnoreMass ? force : force / creator->mass; }
}

void PhysEntity::GenerateRadialForce(Vector3 position, float radius, float strength, float falloff, bool bIgnoreMass){

}

////	Sphere	////

void Sphere::Draw(olc::PixelGameEngine* p, olc::Pixel color) {
	p->FillCircle(position.x, position.y, r, color);
}

////	Box		////

void Box::Draw(olc::PixelGameEngine* p, olc::Pixel color) {
	p->FillRect(position.x - dimensions.x/2, position.y - dimensions.y/2, dimensions.x, dimensions.y);
}