#include "Entities.h"

//// Entity	////

void Entity::SetTag(std::string newTag) {
	tag = newTag;
}

void Entity::SetColor(olc::Pixel newColor) {
	color = newColor;
}

void Entity::RotateX(Vector3 offset) {
	for (auto& m : mesh->triangles) {
		for (auto& n : m.points) {
			n.rotateV3_X(rotation.x, position, offset);
		}
	}
}

void Entity::RotateY(Vector3 offset) {
	for (auto& m : mesh->triangles) {
		for (auto& n : m.points) {
			n.rotateV3_Y(rotation.y, position, offset);
		}
	}
}

void Entity::RotateZ(Vector3 offset) {
	for (auto& m : mesh->triangles) {
		for (auto& n : m.points) {
			n.rotateV3_Z(rotation.z, position, offset);
		}
	}
}

void Entity::Translate(Vector3 translation) {
	for (auto& m : mesh->triangles) {
		for (auto& n : m.points) {
			n.translateV3(translation);
		}
	}
	position += translation;
}

//// Physics Entity ////

void PhysEntity::Update(float deltaTime) {
	if (!bStatic) {
		//Vector3 velLast = velocity;
		//if (acceleration.mag() < .01f) { acceleration = V3ZERO; }
		acceleration = V3ZERO;
		for (auto& f : forces) {
			acceleration += f / mass;
		}
		forces.clear();
		velocity += acceleration * deltaTime;
		//if (velLast.normalized() == -velocity.normalized()) { velocity = V3ZERO; acceleration = V3ZERO; }
		if (velocity.mag() < .01f) { velocity = V3ZERO; acceleration = V3ZERO; }
		position += velocity * deltaTime * 10;

		rotVelocity += rotAcceleration * deltaTime;
		rotation += rotVelocity * deltaTime;
	}
}

//adds a force to this entity, and this entity applies that force back on the sending object
//simply, changes acceleration by force
//NOTE for some reason, this breaks things like friction
void PhysEntity::AddForce(PhysEntity* creator, Vector3 force, bool bIgnoreMass) {
	//this->acceleration += bIgnoreMass ? force : force / mass;
	//if (creator) { creator->acceleration -= bIgnoreMass ? force : force / creator->mass; }
	forces.push_back(force);
}

//if no creator, assume air friction and temporarily treat object as sphere with C=.5
//if creator, assume sliding friction
//TODO(up,delle,11/13/20) change air friction to calculate for shape of object
void PhysEntity::AddFrictionForce(PhysEntity* creator, float frictionCoef, float deltaTime, bool bIngoreMass) {
	if (creator) {

	}
	else {
		forces.push_back(-velocity.normalized() * frictionCoef * mass * GRAVITY);
	}
}

//adds an impulse to this entity, and this entity applies that impulse back on the sending object
//simply, changes velocity by impulse force
void PhysEntity::AddImpulse(PhysEntity* creator, Vector3 impulse, bool bIgnoreMass) {
	this->velocity += bIgnoreMass ? impulse : impulse / mass;
	if (creator) { creator->acceleration -= bIgnoreMass ? impulse : impulse / creator->mass; }
}

//TODO(up,delle,11/13/20) this
void PhysEntity::GenerateRadialForce(Vector3 position, float radius, float strength, float falloff, bool bIgnoreMass) {
}

//// Sphere	////

bool Sphere::ContainsPoint(Vector3 point) {
	return point.distanceTo(position) <= radius;
}

//NOTE can instead return a Collision object with all info needed
//TODO(sp,delle,11/9/20) expand this to a general entity check, but right now it just checks circles
//TODO(oup,delle,11/9/20) if other object is sphere, can optimize the equation to not use sqrt
bool Sphere::CheckCollision(Entity* entity) {
	if (Sphere* sphere = dynamic_cast<Sphere*>(entity)) {
		Vector3 vectorBetween = position - sphere->position;
		float distanceBetween = vectorBetween.mag();
		if (distanceBetween <= (radius + sphere->radius)) {
			return true;
		}
	}
	return false;
}

//TODO(sp,delle,11/9/20) expand this to a general entity check, but right now it just checks circles
void Sphere::ResolveCollision(PhysEntity* other) {
	if (Sphere* sphere = dynamic_cast<Sphere*>(other)) {
		//static resolution
		Vector3 vectorBetween = position - sphere->position;
		float distanceBetween = vectorBetween.mag();
		float overlap = .5f * (distanceBetween - radius - sphere->radius);
		vectorBetween = vectorBetween.normalized() * overlap;
		position -= vectorBetween;
		sphere->position += vectorBetween;

		//dynamic resolution
		//From wikipedia without rotation
		//https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional_collision_with_two_moving_objects
		Vector3 normal = (other->position - position) / distanceBetween;
		float p = 2.f * (normal.dot(velocity - other->velocity)) / (mass + other->mass);
		velocity -= normal * p * other->mass;
		other->velocity += normal * p * mass;
	}
}

//// Box ////

//not sure if this still works or not, when I was trying to select boxes
//it wouldn't do anything but i feel it should still work
bool Box::ContainsPoint(Vector3 point) {
	bool checkX = point.x >= position.x - dimensions.x / 2 && point.x <= position.x + dimensions.x / 2;
	bool checkY = point.y >= position.y - dimensions.y / 2 && point.y <= position.y + dimensions.y / 2;
	//bool checkZ = point.z >= position.z - dimensions.z / 2 && point.z <= position.z + dimensions.z / 2;
	return  checkX && checkY;//&& checkZ;
}

//TODO(sp,delle,11/9/20) expand this to a general entity check
bool Box::CheckCollision(Entity* entity) {
	return false;
}

//TODO(sp,delle,11/9/20) expand this to a general entity check
void Box::ResolveCollision(PhysEntity* entity) {
}

//// Complex ////

bool Complex::ContainsPoint(Vector3 point) {
	//this will probably be hard too implement good luck
	return false;
}

bool Complex::CheckCollision(Entity* entity) {
	return false;
}

void Complex::ResolveCollision(PhysEntity* entity) {
}

//// Camera ////

mat<float, 4, 4> Camera::MakeViewMatrix(float yaw) {
	Vector3 target(0, 0, 1);
	Vector3 up(0, 1, 0);

	lookDir = target * Math::GetRotateV3_Y(yaw);
	target = position + lookDir;

	mat<float, 4, 4> view = inverse(Math::PointAt(position, target, up));

	return view;
}

bool Camera::ContainsPoint(Vector3 point) {
	return false;
}

void Camera::Update(float deltaTime) {
}