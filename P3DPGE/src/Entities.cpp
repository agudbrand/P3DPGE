#include "Entities.h"
#include "Mesh.h"
#include "Collider.h"

//// Entity	////

Vector3 g_campos;

Entity::Entity(int id, EntityParams) {
	this->id = id;
	this->position = position;
	this->rotation = rotation;
	prev_position = position;
	this->scale = scale;
	timer = new Timer;
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
	//do nothing if not SpecialDraw unless debug is enabled
	DEBUGE DrawPosition(p, ProjMat, view);
	DEBUGE DrawVertices(p, ProjMat, view);

	

}
bool Entity::SpecialDraw() { return false; }

void Entity::DrawPosition(olc::PixelGameEngine* p, mat<float, 4, 4> ProjMat, mat<float, 4, 4> view) {
	
	Vector3 nuposition = Math::ProjMult(position.ConvertToM1x4(), view);
			nuposition.ProjToScreen(ProjMat, p);
	
	std::vector<Vector3> points;
	for (Triangle t : mesh->triangles) {
		for (Vector3 po : t.points) {
			Vector3 newp = Math::ProjMult(po.ConvertToM1x4(), view);
			newp.ProjToScreen(ProjMat, p);
			points.push_back(newp);
		}
	}

	Vector3 rightmost;
	Vector3 leftmost;
	Vector3 highest;
	Vector3 lowest;

	std::sort(points.begin(), points.end(), ([](Vector3& v1, Vector3& v2) {return v1.x > v2.x; }));
	leftmost = points[points.size() - 1];
	rightmost  = points[0];

	std::sort(points.begin(), points.end(), ([](Vector3& v1, Vector3& v2) {return v1.y > v2.y; }));
	highest  = points[points.size() - 1];
	lowest = points[0];

	Vector3 tr(rightmost.x, highest.y);
	Vector3 tl(leftmost.x, highest.y);
	Vector3 br(rightmost.x, lowest.y);
	Vector3 bl(leftmost.x, lowest.y);

	Line2 top = Line2(tr, -1, tl);
	Line2 left = Line2(bl, -1, tl);
	Line2 right = Line2(tr, -1, br);
	Line2 bottom = Line2(br, -1, bl);
	
	top.Draw(p, ProjMat, view);
	left.Draw(p, ProjMat, view);
	right.Draw(p, ProjMat, view);
	bottom.Draw(p, ProjMat, view);
	p->DrawString(tr.toVector2(), position.str2f());

}

void Entity::DrawVertices(olc::PixelGameEngine* p, mat<float, 4, 4> ProjMat, mat<float, 4, 4> view) {
	for (Triangle t : mesh->triangles) {
		if (t.get_normal().dot(t.midpoint() - g_campos) < 0) {
			for (Vector3 v : t.points) {
				Vector3 nuv = Math::ProjMult(v.ConvertToM1x4(), view);
						nuv.ProjToScreen(ProjMat, p);
				p->DrawString(nuv.toVector2(), v.str2f());
			}
		}
	}
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

void Entity::Rotate(Vector3 offset) {
	if (rotation != prev_rotation) {
		for (auto& m : mesh->triangles) {
			for (auto& n : m.points) {
				

				mat<float, 4, 4> rot_mat =
					Math::GetRotateV3_X(rotation.x - prev_rotation.x) *
					Math::GetRotateV3_Y(rotation.y - prev_rotation.y) *
					Math::GetRotateV3_Z(rotation.z - prev_rotation.z);

				Vector3 n_local = Math::ProjMult(n.ConvertToM1x4(), Math::GetWorldToLocal(position));
				n_local = Math::ProjMult(n_local.ConvertToM1x4(), rot_mat);
				n = Math::ProjMult(n_local.ConvertToM1x4(), Math::GetLocalToWorld(position));
			}
		}
	}
	prev_rotation = rotation;
}

void Entity::Translate() {	
	if (position != prev_position) {
		for (auto& m : mesh->triangles) {
			for (auto& n : m.points) {
				//n = n * Math::GetTranslate(position - prev_position);

				Vector3 n_local = Math::ProjMult(n.ConvertToM1x4(), Math::GetWorldToLocal(position));
				n_local = Math::ProjMult(n_local.ConvertToM1x4(), Math::GetTranslate(position - prev_position));
				n = Math::ProjMult(n_local.ConvertToM1x4(), Math::GetLocalToWorld(position));
			}
		}
	}
	prev_position = position;
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



void Entity::Update(float deltaTime) {

	Translate(); Rotate();
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

void PhysEntity::Draw(olc::PixelGameEngine* p, mat<float, 4, 4> ProjMat, mat<float, 4, 4> view) {
	//do nothing if not SpecialDraw unless debug is enabled

	Line3 v_vector = Line3((position + velocity * 100).clampMag(100), -1, position);
	Line3 a_vector = Line3((position + acceleration * 100).clampMag(100), -1, position);

	v_vector.Draw(p, ProjMat, view);
	a_vector.Draw(p, ProjMat, view);

	DrawPosition(p, ProjMat, view);


}

void PhysEntity::Update(float deltaTime) {
	Entity::Update(deltaTime);
}

void PhysEntity::PhysUpdate(float deltaTime) {

	BUFFERLOG(12, inputs);

	//for debug
	Vector3 tf = V3ZERO;
	BUFFERLOGI(8, 20, deltaTime, " ", g_fixedDeltaTime);
	if (deltaTime > g_fixedDeltaTime) {
		//psuedo input
		AddForce(nullptr, inputs);
		AddFrictionForce(nullptr, 0.01f, deltaTime);
		if (!bStatic) {
			Vector3 netForce;
			acceleration = V3ZERO;
			for (auto& f : forces) {
				netForce += f;
				tf += f;
			}
			BUFFERLOG(3, "Forces:       ", netForce);
			acceleration = netForce / mass * 100;

			
			forces.clear();
			velocity += acceleration * g_fixedDeltaTime;
			rotVelocity += rotAcceleration * g_fixedDeltaTime;
			rotation += rotVelocity * g_fixedDeltaTime * 10;
			Vector3 last_vel = V_STORE(velocity, 1);
			if (velocity.mag() < .1f) { velocity = V3ZERO; acceleration = V3ZERO; }

			//NOTE: this may hinder some collision checking so be aware of that
			if (Math::round2v(velocity.normalized()) == Math::round2v(-last_vel.normalized())) {
				velocity = V3ZERO; acceleration = V3ZERO;
			}
			pos_lerp_from = position;
			pos_lerp_to = position + velocity * g_fixedDeltaTime;
			rotAcceleration = V3ZERO;
		}
	}
	else {
		Interpolate(deltaTime / g_fixedDeltaTime);
	}

	BUFFERLOG(0,  "lerp_from:       ", pos_lerp_from);
	BUFFERLOG(1,  "lerp_to:         ", pos_lerp_to);
	BUFFERLOG(5,  "Velocity:        ", velocity);
	BUFFERLOG(6,  "Acceleration:    ", acceleration);
	BUFFERLOG(17, "rotAcceleration: ", rotAcceleration);
	BUFFERLOG(18, "rotVelocity:     ", rotVelocity);

	inputs = V3ZERO;
	
}

void PhysEntity::Interpolate(float t) {
	
	position.x = Math::lerpf(pos_lerp_from.x, pos_lerp_to.x, t);
	position.y = Math::lerpf(pos_lerp_from.y, pos_lerp_to.y, t);
	position.z = Math::lerpf(pos_lerp_from.z, pos_lerp_to.z, t);

}

//adds a force to this entity, and this entity applies that force back on the sending object
//simply, changes acceleration by force
void PhysEntity::AddForce(PhysEntity* creator, Vector3 force, bool bIgnoreMass) {
	//this->acceleration += bIgnoreMass ? force : force / mass;
	//if (creator) { creator->acceleration -= bIgnoreMass ? force : force / creator->mass; }
	forces.push_back(force);
}

void PhysEntity::AddInput(Vector3 input) {
	inputs += input;
	inputs.normalize();
}

//if no creator, assume air friction and temporarily treat object as sphere with C=.5
//if creator, assume sliding friction
//TODO(up,delle,11/13/20) change air friction to calculate for shape of object
void PhysEntity::AddFrictionForce(PhysEntity* creator, float frictionCoef, float deltaTime, bool bIngoreMass) {
	//if (creator) {
	//}
	//else {
	forces.push_back(-velocity.normalized() * frictionCoef * mass * GRAVITY);
	BUFFERLOG(13, -velocity.normalized() * frictionCoef * mass * GRAVITY);
	//}
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
	tag = "box";
	mesh = new BoxMesh(dimensions, position, this);
	sprite = new olc::Sprite("sprites/UV_Grid_Sm.jpg");
	pos_lerp_from = position;
	pos_lerp_to = position;
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
	ASSERT(LoadFromObjectFile(file_name), "OBJ_LOAD_ERROR");
	model_name = Math::append_decimal(file_name);
	//temp fix i guess
	prev_position = V3ZERO;
	Translate();

}

bool Complex::LoadFromObjectFile(std::string file_name) {
	std::ifstream f(file_name);
	if (!f.is_open()) { return false; }

	std::vector<Vector3> vertices;

	while (!f.eof()) {
		char line[128];
		f.getline(line, 128);

		std::stringstream s;

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

			mesh->triangles.push_back(Triangle(vertices[f[0] - 1], vertices[f[1] - 1], vertices[f[2] - 1], this));
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
		"model_name      " + model_name                 + "\n" +
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

void Line3::Draw(olc::PixelGameEngine* p, mat<float, 4, 4> ProjMat, mat<float, 4, 4> view) {
	Vector2 screenDimensions = Vector2(p->ScreenWidth(), p->ScreenHeight());
	//convert vertexes from world to camera/view space
	Vector3 startVertex = Math::WorldToCamera(position, view);
	Vector3 endVertex = Math::WorldToCamera(endPosition, view);
	
	//clip vertexes to the near and far z planes in camera/view space
	if (!Math::ClipLineToZPlanes(startVertex, endVertex, .1f, 1000.1f)) { return; } //TODO(r,delle) change the third parameter to the camera variable
	
	//convert vertexes from camera/view space to clip space
	startVertex = Math::CameraToScreen(startVertex, ProjMat, screenDimensions);
	endVertex = Math::CameraToScreen(endVertex, ProjMat, screenDimensions);

	//clip vertexes to border planes in clip space
	if (!Math::ClipLineToBorderPlanes(startVertex, endVertex, screenDimensions)) { return; }

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

mat<float, 4, 4> Camera::MakeViewMatrix(float yaw, bool force_target) {
	target = V3FORWARD;

	lookDir = target * Math::GetRotateV3_X(yaw);
	target = position + lookDir;
	
	BUFFERLOG(16, lookDir);
	mat<float, 4, 4> view = inverse(Math::PointAt(position, target, up));
	
	return view;
}

mat<float, 4, 4> Camera::ProjectionMatrix(olc::PixelGameEngine* p) {
	float renderToView = farZ - nearZ;
	float aspectRatio = (float)p->ScreenHeight() / (float)p->ScreenWidth();
	float fovRad = 1.f / tanf(fieldOfView * .5f / 180.f * M_PI);

	mat<float, 4, 4> proj{
		aspectRatio * fovRad, 0,	  0,								0,
		0,					  fovRad, 0,								0,
		0,					  0,	  farZ / renderToView,				1,
		0,					  0,	  (-farZ * nearZ) / renderToView,	0
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