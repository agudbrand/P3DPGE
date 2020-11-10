#include "Entities.h"

//// Entity	////

void Entity::SetTag(std::string newTag){
	tag = newTag;
}

void Entity::SetColor(olc::Pixel newColor) {
	color = newColor;
}

//// Physics Entity ////

void PhysEntity::Update(float deltaTime){
	if(!bStatic) {
		velocity += acceleration * deltaTime;
		if (velocity.x < 0.1f) { velocity.x = 0; }
		if (velocity.y < 0.1f) { velocity.y = 0; }
		if (velocity.z < 0.1f) { velocity.z = 0; }
		position += velocity * deltaTime;

		rotVelocity += rotAcceleration * deltaTime;
		rotation += rotVelocity * deltaTime;
	}
}

void PhysEntity::AddForce(PhysEntity* creator, Vector3 force, bool bIgnoreMass){
	this->acceleration += bIgnoreMass ? force : force / mass;
	if (acceleration.x < 0.1f) { acceleration.x = 0; }
	if (acceleration.y < 0.1f) { acceleration.y = 0; }
	if (acceleration.z < 0.1f) { acceleration.z = 0; }
	if (creator) { creator->acceleration -= bIgnoreMass ? force : force / creator->mass; }
}

void PhysEntity::GenerateRadialForce(Vector3 position, float radius, float strength, float falloff, bool bIgnoreMass){

}

//// Sphere	////

void Sphere::Draw(olc::PixelGameEngine* p) {
	p->FillCircle(position.x, position.y, radius, color);
}

void Sphere::RotateX() {
	//do when 3d spheres are implemented
}

bool Sphere::ContainsPoint(Vector3 point) {
	return point.distanceTo(position) <= radius;
}

//// Box ////

void Box::Draw(olc::PixelGameEngine* p) { mesh.Draw(p, color); }

void Box::RotateX() {
	for (auto& m : mesh.triangles) {
		for (auto& n : m.points) {
			n.rotateV3_X(rotation.x);
		}
	}
}

//not sure if this still works or not, when I was trying to select boxes
//it wouldn't do anything but i feel it should still work
bool Box::ContainsPoint(Vector3 point) {
	bool checkX = point.x >= position.x - dimensions.x / 2 && point.x <= position.x + dimensions.x / 2;
	bool checkY = point.y >= position.y - dimensions.y / 2 && point.y <= position.y + dimensions.y / 2;
	bool checkZ = point.z >= position.z - dimensions.z / 2 && point.z <= position.z + dimensions.z / 2;
	return  checkX && checkY && checkZ;
}