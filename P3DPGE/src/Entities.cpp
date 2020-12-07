#include "Entities.h"
#include "Mesh.h"
#include "Collider.h"

//// Entity	////

Entity::Entity(int id, EntityParams) {
	this->id = id;
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
}

Entity::~Entity() {
	if (mesh) delete mesh;
	if (sprite) delete sprite;
	if (decal) delete decal;
}

void Entity::SetTag(std::string newTag) {
	tag = newTag;
}

void Entity::SetColor(olc::Pixel newColor) {
	for (Triangle& t : mesh->triangles) { t.color = newColor; }
}

//override this later please
bool Entity::LineIntersect(Edge3* e) {
	for (Triangle& t : mesh->triangles) {
		if (t.line_intersect(e)) { return true; }
	}
	return false;
}

void Entity::Draw(olc::PixelGameEngine* p, mat<float, 4, 4> ProjMat, mat<float, 4, 4> view) {
	//do nothing if not SpecialDraw
}

bool Entity::SpecialDraw() { return false; }

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

std::string Entity::str() {
	std::string s =
		"tag         " + tag					+ "\n" +
		"id          " + std::to_string(id)		+ "\n" +
		"position    " + position.str2f()		+ "\n" +
		"rotation    " + rotation.str2f()		+ "\n" +
		"scale       " + scale.str2f()			+ "\n" +
		"entity_type: base_entity";
		return s;
 
}

//// Physics Entity ////

PhysEntity::PhysEntity(int id, EntityParams, PhysEntityParams) : Entity(EntityArgs) {
	this->velocity = velocity;
	this->acceleration = acceleration;
	this->rotVelocity = rotVelocity;
	this->rotAcceleration = rotAcceleration;
	this->mass = mass;
	this->elasticity = elasticity;
	this->bStatic = bStatic;
};

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

Sphere::Sphere(float r, int id, EntityParams, PhysEntityParams) : PhysEntity(EntityArgs, PhysEntityArgs) {
	this->radius = r;
}

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

std::string Sphere::str() {
	std::string s =
		"tag             " + tag						+ "\n" +
		"id              " + std::to_string(id)			+ "\n" +
		"position        " + position.str2f()			+ "\n" +
		"rotation        " + rotation.str2f()			+ "\n" +
		"scale           " + scale.str2f()				+ "\n" +
		"mass            " + std::to_string(mass)		+ "\n" +
		"velocity        " + velocity.str2f()			+ "\n" +
		"acceleration    " + acceleration.str2f()		+ "\n" +
		"rotVelocity     " + rotVelocity.str2f()		+ "\n" +
		"rotAcceleration " + rotAcceleration.str2f()	+ "\n" +
		"entity_type: sphere";
	return s;

}

//// Box ////

Box::Box(Vector3 dimensions, int id, EntityParams, PhysEntityParams) : PhysEntity(EntityArgs, PhysEntityArgs) {
	this->dimensions = dimensions;
	mesh = new BoxMesh(dimensions, position);
}

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

std::string Box::str() {
	std::string s =
		"tag             " + tag						+ "\n" +
		"id              " + std::to_string(id)			+ "\n" +
		"position        " + position.str2f()			+ "\n" +
		"rotation        " + rotation.str2f()			+ "\n" +
		"scale           " + scale.str2f()				+ "\n" +
		"mass            " + std::to_string(mass)		+ "\n" +
		"velocity        " + velocity.str2f()			+ "\n" +
		"acceleration    " + acceleration.str2f()		+ "\n" +
		"rotVelocity     " + rotVelocity.str2f()		+ "\n" +
		"rotAcceleration " + rotAcceleration.str2f()	+ "\n" +
		"entity_type: box";
	return s;

}

//// Complex ////

Complex::Complex(std::string file_name, int id, EntityParams, PhysEntityParams) : PhysEntity(EntityArgs, PhysEntityArgs) {
	mesh = new Mesh();
	if (!LoadFromObjectFile(file_name)) {
		std::cout << "OBJ LOAD ERROR" << std::endl;
	}
	model_name = Math::append_decimal(file_name);

	for (Triangle& t : mesh->triangles) {
		t.set_color(olc::Pixel(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1));
	}
}

bool Complex::LoadFromObjectFile(std::string file_name) {
	std::ifstream f(file_name);
	if (!f.is_open()) { return false; }

	std::vector<Vector3> vertices;

	while (!f.eof()) {
		char line[128];
		f.getline(line, 128);

		std::strstream s;

		s << line;

		char junk;

		if (line[0] == 'v') {
			Vector3 v;

			s >> junk >> v.x >> v.y >> v.z;
			vertices.push_back(v);
		}

		if (line[0] == 'f') {
			int f[3];
			s >> junk >> f[0] >> f[1] >> f[2];

			mesh->triangles.push_back(Triangle(vertices[f[0] - 1], vertices[f[1] - 1], vertices[f[2] - 1]));
		}
	}

	return true;
}

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

std::string Complex::str() {
	std::string s =
		"tag             " + tag						+ "\n" +
		"id              " + std::to_string(id)			+ "\n" +
		"position        " + position.str2f()			+ "\n" +
		"rotation        " + rotation.str2f()			+ "\n" +
		"scale           " + scale.str2f()				+ "\n" +
		"mass            " + std::to_string(mass)		+ "\n" +
		"velocity        " + velocity.str2f()			+ "\n" +
		"acceleration    " + acceleration.str2f()		+ "\n" +
		"rotVelocity     " + rotVelocity.str2f()		+ "\n" +
		"rotAcceleration " + rotAcceleration.str2f()	+ "\n" +
		"entity_type: complex";
	return s;

}

//// Line2 and Line3 ////

Line2::Line2(Vector3 endPosition, int id, EntityParams) : Entity(EntityArgs) {
	//just so no RAV
	mesh = new Mesh();

	endPosition.z = 0; this->endPosition = endPosition;
	this->id = id;

}

void Line2::Draw(olc::PixelGameEngine* p, mat<float, 4, 4> ProjMat, mat<float, 4, 4> view) {
	p->DrawLine(position.toVector2(), endPosition.toVector2(), color);
}
bool Line2::SpecialDraw() { return true; }


void Line2::SetColor(olc::Pixel newColor) { color = newColor; }
bool Line2::ContainsPoint(Vector3 point) { return false; }
bool Line2::ContainsScreenPoint(Vector3 point) { return false; }
void Line2::Update(float deltaTime) {}

std::string Line2::str() {
	std::string s =
		"tag         " + tag + "\n" +
		"id          " + std::to_string(id) + "\n" +
		"position    " + position.str2f() + "\n" +
		"rotation    " + rotation.str2f() + "\n" +
		"scale       " + scale.str2f() + "\n" +
		"entity_type: line_2";
	return s;

}

Line3::Line3(Vector3 endPosition, int id, EntityParams) : Entity(EntityArgs) {
	mesh = new Mesh();
	this->endPosition = endPosition;
	this->id = id;

	edge = Edge3(position, endPosition);
}

//TODO(r,delle) test that this works
void Line3::Draw(olc::PixelGameEngine* p, mat<float, 4, 4> ProjMat, mat<float, 4, 4> view) {
	Vector2 screenDimensions = Vector2(p->ScreenWidth(), p->ScreenHeight());
	//convert vertexes from world to camera/view space
	Vector3 startVertex = Render::WorldSpaceToCameraSpace(position, view);
	Vector3 endVertex = Render::WorldSpaceToCameraSpace(endPosition, view);
	
	//clip vertexes to the near and far z planes in camera/view space
	if (!Render::ClipLineToZPlanes(startVertex, endVertex, .1f, 1000.1f)) { return; } //TODO(r,delle) change the third parameter to the camera variable
	
	//convert vertexes from camera/view space to clip space
	startVertex = Render::CameraSpaceToScreenSpace(startVertex, ProjMat, screenDimensions);
	endVertex = Render::CameraSpaceToScreenSpace(endVertex, ProjMat, screenDimensions);

	//clip vertexes to border planes in clip space
	if (!Render::ClipLineToBorderPlanes(startVertex, endVertex, screenDimensions)) { return; }

	//draw the lines after all clipping and space conversion
	p->DrawLine(startVertex.toVector2(), endVertex.toVector2(), color);
}
bool Line3::SpecialDraw() { return true; }

void Line3::SetColor(olc::Pixel newColor) { color = newColor; }
bool Line3::ContainsPoint(Vector3 point) { return false; }
bool Line3::ContainsScreenPoint(Vector3 point) { return false; }
void Line3::Update(float deltaTime) {}

std::string Line3::str() {
	std::string s =
		"tag         " + tag + "\n" +
		"id          " + std::to_string(id) + "\n" +
		"position    " + position.str2f() + "\n" +
		"rotation    " + rotation.str2f() + "\n" +
		"scale       " + scale.str2f() + "\n" +
		"entity_type: line_3";
	return s;

}

//// Debug Triangle ////

DebugTriangle::DebugTriangle(Triangle triangle, int id, EntityParams) : Entity(EntityArgs) {
	mesh = new Mesh(triangle);
}

bool DebugTriangle::ContainsPoint(Vector3 point) {
	return false;
}
bool DebugTriangle::ContainsScreenPoint(Vector3 point) { return false; }

void DebugTriangle::Update(float deltaTime) {
}

std::string DebugTriangle::str() {
	std::string s =
		"tag         " + tag + "\n" +
		"id          " + std::to_string(id) + "\n" +
		"position    " + position.str2f() + "\n" +
		"rotation    " + rotation.str2f() + "\n" +
		"scale       " + scale.str2f() + "\n" +
		"entity_type: debug_triangle";
	return s;

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

//this matrix seems to only work well with 1:1 aspect ratios I think its cause FOV is set to 90
mat<float, 4, 4> Camera::ProjectionMatrix(olc::PixelGameEngine* p) {
	float renderToView = farZ - nearZ;
	float aspectRatio = (float)p->ScreenHeight() / (float)p->ScreenWidth();
	float fovRad = 1.f / tanf(fieldOfView * .5f / 180.f * M_PI);

	mat<float, 4, 4> proj{
		aspectRatio * fovRad, 0, 0,	0,
		0, fovRad, 0, 0,
		0, 0, farZ / renderToView, 1,
		0, 0, (-farZ * nearZ) / renderToView, 0
	};

	return proj;
}

bool Camera::ContainsPoint(Vector3 point) {
	return false;
}
bool Camera::ContainsScreenPoint(Vector3 point) { return false; }

void Camera::Update(float deltaTime) {
}

std::string Camera::str() {
	std::string s =
		"tag         " + tag + "\n" +
		"id          " + std::to_string(id) + "\n" +
		"position    " + position.str2f() + "\n" +
		"rotation    " + rotation.str2f() + "\n" +
		"scale       " + scale.str2f() + "\n" +
		"entity_type: camera";
	return s;

}