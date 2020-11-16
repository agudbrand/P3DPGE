#include "Entities.h"

//// Entity	////

void Entity::SetTag(std::string newTag) {
	tag = newTag;
}

void Entity::SetColor(olc::Pixel newColor) {
	color = newColor;
}

void Entity::RotateX(Vector3 offset) {
	for (auto& m : mesh.triangles) {
		for (auto& n : m.points) {
			n.rotateV3_X(rotation.x, position, offset);
		}
	}
}

void Entity::RotateY(Vector3 offset) {
	for (auto& m : mesh.triangles) {
		for (auto& n : m.points) {
			n.rotateV3_Y(rotation.y, position, offset);
		}
	}
}

void Entity::RotateZ(Vector3 offset) {
	for (auto& m : mesh.triangles) {
		for (auto& n : m.points) {
			n.rotateV3_Z(rotation.z, position, offset);
		}
	}
}

void Entity::Translate(Vector3 translation) {
	for (auto& m : mesh.triangles) {
		for (auto& n : m.points) {
			n.translateV3(translation);
		}
	}
	position += translation;
}

void Entity::ProjectToScreen(mat<float, 4, 4> ProjMat, olc::PixelGameEngine* p, mat<float, 4, 4> view) {
	//copy triangles actual points to projected points for projecting
	for (auto& t : mesh.triangles) {
		t.copy_points();
	}

	//convert mesh to viewed mesh
	for (auto& m : mesh.triangles) {
		for (auto& n : m.projectedPoints) { n = n * view; }
	}

	for (auto& m : mesh.triangles) {
		//get clipped triangles
		int clippedTriangles = 0;
		Triangle clipped[2];
		clippedTriangles = Math::ClipTriangles(Vector3(0, 0, 0.1), Vector3(0, 0, 1), m, clipped[0], clipped[1]);
		for (auto& t : clipped){
			for (auto& n : t.projectedPoints) {
				n.ProjToScreen(ProjMat, p, position);
			}
		}
		for (auto& n : m.projectedPoints) {
			n.ProjToScreen(ProjMat, p, position);
		}
		
	}
	
}

//// Physics Entity ////

void PhysEntity::Update(float deltaTime) {
	if (!bStatic) {
		if (acceleration.mag() < .01f) { acceleration = V3ZERO; }
		velocity += acceleration * deltaTime;
		if (velocity.mag() < .1f) { velocity = V3ZERO; }
		position += velocity * deltaTime;

		rotVelocity += rotAcceleration * deltaTime;
		rotation += rotVelocity * deltaTime;
	}
}

//adds a force to this entity, and this entity applies that force back on the sending object
//simply, changes acceleration by force
//NOTE for some reason, this breaks things like friction
void PhysEntity::AddForce(PhysEntity* creator, Vector3 force, bool bIgnoreMass) {
	this->acceleration += bIgnoreMass ? force : force / mass;
	if (creator) { creator->acceleration -= bIgnoreMass ? force : force / creator->mass; }
}

//if no creator, assume air friction and temporarily treat object as sphere with C=.5
//if creator, assume sliding friction
//TODO(up,delle,11/13/20) change air friction to calculate for shape of object
void PhysEntity::AddFrictionForce(PhysEntity* creator, float frictionCoef, bool bIngoreMass) {
	if (creator) {
	}
	else {
		acceleration = -velocity * frictionCoef;
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

void Sphere::Draw(olc::PixelGameEngine* p, bool wireframe) {
	p->FillCircle(position.x, position.y, radius, color);
}

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
			//TEMP manual static resolution in here
			float overlap = .5f * (distanceBetween - radius - sphere->radius);
			vectorBetween = vectorBetween.normalized() * overlap;
			position -= vectorBetween;
			sphere->position += vectorBetween;
			return true;
		}
	}
	return false;
}

//TODO(sp,delle,11/9/20) expand this to a general entity check, but right now it just checks circles
void Sphere::ResolveCollision(Entity* entity) {
}

//// Box ////

void Box::Draw(olc::PixelGameEngine* p, bool wireframe) { mesh.Draw(p, color, position, wireframe); }

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
void Box::ResolveCollision(Entity* entity) {
}

//// Complex ////

void Complex::Draw(olc::PixelGameEngine* p, bool wireframe) { mesh.Draw(p, color, position, wireframe); }

bool Complex::ContainsPoint(Vector3 point) {
	//this will probably be hard too implement good luck
	return false;
}

bool Complex::CheckCollision(Entity* entity) {
	return false;
}

void Complex::ResolveCollision(Entity* entity) {
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

//i wish i could not write these
void Camera::Draw(olc::PixelGameEngine* p, bool wireframe) {
	//camera is not drawn
}

bool Camera::ContainsPoint(Vector3 point) {
	return false;
}

void Camera::Update(float deltaTime) {
}