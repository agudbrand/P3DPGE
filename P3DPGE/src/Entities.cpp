#include "Entities.h"

//// Entity	////

void Entity::SetTag(std::string newTag) {
	tag = newTag;
}

void Entity::SetColor(olc::Pixel newColor) {
	for (Triangle& t : mesh->triangles) { t.color = newColor; }
}

void Entity::Draw(olc::PixelGameEngine* p, bool wireframe) {
	mesh->Draw(p, position, wireframe);
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

//TODO(up,delle,11/13/20) create the GenerateRadialForce function
void PhysEntity::GenerateRadialForce(Vector3 position, float radius, float strength, float falloff, bool bIgnoreMass) {
}

//// Sphere	////

bool Sphere::ContainsPoint(Vector3 point) {
	return point.distanceTo(position) <= radius;
}


bool Sphere::ContainsScreenPoint(Vector3 point) {
	point.z = 0;
	Vector3 pos = position;
	pos.z = 0;
	return point.distanceTo(pos) <= radius;
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
//TODO(oup,delle,11/17/20) can optimize this by recieving info from CheckCollision
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
	bool checkZ = point.z >= position.z - dimensions.z / 2 && point.z <= position.z + dimensions.z / 2;
	return  checkX && checkY && checkZ;
}

bool Box::ContainsScreenPoint(Vector3 point) {
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
	for (Triangle& t : mesh->triangles) {
		if (t.selected) { t.selected = false; }
		if (t.contains_point(point)) { return true; }
	}
	return false;
}

bool Complex::ContainsScreenPoint(Vector3 point){
	return false;
}

bool Complex::CheckCollision(Entity* entity) {
	return false;
}

void Complex::ResolveCollision(PhysEntity* entity) {
}

//// Line2 and Line3 ////

void Line2::Draw(olc::PixelGameEngine* p, bool wireframe) {
	p->DrawLine(position.Vector3Tovd2d(), endPosition.Vector3Tovd2d(), color);
}

void Line2::SetColor(olc::Pixel newColor) { color = newColor; }
bool Line2::ContainsPoint(Vector3 point) { return false; }
bool Line2::ContainsScreenPoint(Vector3 point) { return false; }
void Line2::Update(float deltaTime) {}

//TODO(re, sushi, 11/18/2020) for some reason this seems to have the same projecting problem as meshes did but idk y yet so fix it
void Line3::Draw(olc::PixelGameEngine* p, bool wireframe) {
	//these operations are very gross and could probably be abstracted in Math
	//TODO(m, sushi) abstract the following functions in Math.h in order to make this not look retarded
	Vector3 posView = position.GetM1x4ToVector3(position.proj_mult(position.ConvertToM1x4(), mesh->view));
	Vector3 endView = endPosition.GetM1x4ToVector3(endPosition.proj_mult(endPosition.ConvertToM1x4(), mesh->view));

	posView.ProjToScreen(mesh->ProjMat, p);
	endView.ProjToScreen(mesh->ProjMat, p);

	p->DrawLine(posView.Vector3Tovd2d(), endView.Vector3Tovd2d(), color);
}

void Line3::SetColor(olc::Pixel newColor) { color = newColor; }
bool Line3::ContainsPoint(Vector3 point) { return false; }
bool Line3::ContainsScreenPoint(Vector3 point) { return false; }
void Line3::Update(float deltaTime) {}

//// Debug Triangle ////

bool DebugTriangle::ContainsPoint(Vector3 point) {
	return false;
}
bool DebugTriangle::ContainsScreenPoint(Vector3 point) { return false; }

void DebugTriangle::Update(float deltaTime) {
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
bool Camera::ContainsScreenPoint(Vector3 point) { return false; }

void Camera::Update(float deltaTime) {
}