#include "PhysicsSystem.h"
#include "../utils/PhysicsWorld.h"
#include "../math/Math.h"

#include "../components/Transform.h"
#include "../components/Physics.h"

#include "../utils/Command.h"
#include "../components/InputSingleton.h"
#include "../components/TimeSingleton.h"
#include "../components/Camera.h"
#include "../components/ScreenSingleton.h"

inline void AddSelectedEntityCommands(EntityAdmin* admin) {
	//TODO(ip,delle) update entity movement commands to be based on EntityID
	admin->commands["translate_right"] = new Command([](EntityAdmin* admin) {
		if(admin->singletonInput->selectedEntity) {
			if(Physics* p = admin->singletonInput->selectedEntity->GetComponent<Physics>()) {
				PhysicsSystem::AddInput(p, Vector3::RIGHT);
				LOG("+x");
			}
		}
	}, "translate_right", "translate_right <EntityID> <amount> [speed]");

	admin->commands["translate_left"] = new Command([](EntityAdmin* admin) {
		if(admin->singletonInput->selectedEntity) {
			if(Physics* p = admin->singletonInput->selectedEntity->GetComponent<Physics>()) {
				PhysicsSystem::AddInput(p, Vector3::LEFT);
				LOG("-x");
			}
		}
	}, "translate_left", "translate_left <EntityID> <amount> [speed]");

	admin->commands["translate_up"] = new Command([](EntityAdmin* admin) {
		if(admin->singletonInput->selectedEntity) {
			if(Physics* p = admin->singletonInput->selectedEntity->GetComponent<Physics>()) {
				PhysicsSystem::AddInput(p, Vector3::UP);
				LOG("+y");
			}
		}
	}, "translate_up", "translate_up <EntityID> <amount> [speed]");

	admin->commands["translate_down"] = new Command([](EntityAdmin* admin) {
		if(admin->singletonInput->selectedEntity) {
			if(Physics* p = admin->singletonInput->selectedEntity->GetComponent<Physics>()) {
				PhysicsSystem::AddInput(p, Vector3::DOWN);
				LOG("-y");
			}
		}
	}, "translate_down", "translate_down <EntityID> <amount> [speed]");

	admin->commands["translate_forward"] = new Command([](EntityAdmin* admin) {
		if(admin->singletonInput->selectedEntity) {
			if(Physics* p = admin->singletonInput->selectedEntity->GetComponent<Physics>()) {
				PhysicsSystem::AddInput(p, Vector3::FORWARD);
				LOG("+z");
			}
		}
	}, "translate_forward", "translate_forward <EntityID> <amount> [speed]");

	admin->commands["translate_backward"] = new Command([](EntityAdmin* admin) {
		if(admin->singletonInput->selectedEntity) {
			if(Physics* p = admin->singletonInput->selectedEntity->GetComponent<Physics>()) {
				PhysicsSystem::AddInput(p, Vector3::BACK);
				LOG("-z");
			}
		}
	}, "translate_backward", "translate_backward <EntityID> <amount> [speed]");

	admin->commands["add_force"] = new Command([](EntityAdmin* admin) {
		if(admin->singletonInput->selectedEntity) {
			if(Physics* p = admin->singletonInput->selectedEntity->GetComponent<Physics>()) {
				Vector3 pos = Math::ScreenToWorld(admin->singletonInput->mousePos, admin->currentCamera->projectionMatrix, 
													admin->currentCamera->viewMatrix, admin->singletonScreen->dimensions);
				Vector3 clickPos = Math::ScreenToWorld(admin->singletonInput->mouseClickPos, admin->currentCamera->projectionMatrix, 
														admin->currentCamera->viewMatrix, admin->singletonScreen->dimensions);
				//TODO(pi,delle) test that you can add force to a selected entity
				PhysicsSystem::AddForce(nullptr, p, (pos - clickPos).normalized() * 5);
			}
		}
	}, "add_force", "add_force <EntityID> <force_vector> [constant_force?]");
}

void PhysicsSystem::Init() {
	AddSelectedEntityCommands(admin);
}

struct PhysicsTuple { 
	Transform* transform; 
	Physics* physics; 
	PhysicsTuple(Transform* transform, Physics* physics) : transform(transform), physics(physics) {}
};

inline std::vector<PhysicsTuple> GetPhysicsTuples(EntityAdmin* admin) {
	std::vector<PhysicsTuple> out;
	for(auto e : admin->entities) {
		Transform* transform = 0;
		Physics* physics = 0;
		for(Component* c : e.second->components) {
			if(Transform* t = dynamic_cast<Transform*>(c)) { transform = t; }
			if(Physics* p = dynamic_cast<Physics*>(c)) { physics = p; }
		}
		if(transform && physics) {
			out.push_back(PhysicsTuple(transform, physics));
		}
	}
	return out;
}

//TODO(p,delle) look into bettering this physics tick
//https://gafferongames.com/post/physics_in_3d/
inline void PhysicsTick(PhysicsTuple& t, PhysicsWorld* pw, TimeSingleton* time) {
	//add input forces
	t.physics->inputVector.normalize();
	PhysicsSystem::AddForce(nullptr, t.physics, t.physics->inputVector);
	t.physics->inputVector = Vector3::ZERO;

	//add temp air friction force
	PhysicsSystem::AddFrictionForce(nullptr, t.physics, pw->frictionAir);

	//sum up forces to calculate acceleration
	Vector3 netForce;
	for(auto& f : t.physics->forces) {
		netForce += f;
	}
	t.physics->acceleration = netForce / t.physics->mass * 50; //TODO(p,sushi) explain to delle, why multiply by 50?
	t.physics->forces.clear();

	//update linear movement and clamp it to min/max velocity
	t.physics->velocity += t.physics->acceleration * time->physicsDeltaTime;
	float velMag = t.physics->velocity.mag();
	if(velMag > pw->maxVelocity) {
		t.physics->velocity.normalize();
		t.physics->velocity *= pw->maxVelocity;
	} else if(velMag < pw->minVelocity) {
		t.physics->velocity = Vector3::ZERO;
		t.physics->acceleration = Vector3::ZERO;
	}
	t.physics->position += t.physics->velocity * time->physicsDeltaTime;

	//update rotational movement
	t.physics->rotVelocity += t.physics->rotAcceleration * time->physicsDeltaTime;
	t.physics->rotation += t.physics->rotVelocity * time->physicsDeltaTime * 10; //TODO(p,sushi) explain to delle, why multiply by 10?
}

void PhysicsSystem::Update() {
	TimeSingleton* time = admin->singletonTime;
	PhysicsWorld* pw = admin->physicsWorld;

	std::vector<PhysicsTuple> tuples = GetPhysicsTuples(admin);

	//update physics extra times per frame if frame time delta is larger than physics time delta
	while(time->physicsAccumulator >= time->physicsDeltaTime) {
		for(auto& t : tuples) {
			PhysicsTick(t, pw, time);
		}
		time->physicsAccumulator -= time->physicsDeltaTime;
		time->physicsTotalTime += time->physicsDeltaTime;
	}

	//interpolate between new physics position and old transform position by the leftover time
	float alpha = time->physicsAccumulator / time->physicsDeltaTime;
	for(auto& t : tuples) {
		t.transform->prevPosition = t.transform->position;
		t.transform->prevRotation = t.transform->rotation;
		t.transform->position = t.transform->position*(1.f - alpha) + t.physics->position*alpha;
		t.transform->rotation = t.transform->rotation*(1.f - alpha) + t.physics->rotation*alpha;
		//t.transform->rotation *= Matrix4::RotationMatrixAroundPoint(t.transform->position, t.transform->rotation*(1.f - alpha) + t.physics->rotation*alpha);
		//TODO(p,delle) look into better rotational interpolation once we switch to quaternions
	}
}

//adds a force to this entity, and this entity applies that force back on the sending object
//simply, changes acceleration by force
inline void PhysicsSystem::AddForce(Physics* creator, Physics* target, Vector3 force) {
	//this->acceleration += bIgnoreMass ? force : force / mass;
	//if (creator) { creator->acceleration -= bIgnoreMass ? force : force / creator->mass; }
	target->forces.push_back(force);
	if(creator) { creator->forces.push_back(-force); }
}

inline void PhysicsSystem::AddInput(Physics* target, Vector3 input) {
	target->inputVector += input;
}

//if no creator, assume air friction
//if creator, assume sliding friction
//TODO(up,delle,11/13/20) change air friction to calculate for shape of object
inline void PhysicsSystem::AddFrictionForce(Physics* creator, Physics* target, float frictionCoef, float gravity) {
	target->forces.push_back(-target->velocity.normalized() * frictionCoef * target->mass * gravity);
	if (creator) {
		//TODO(p,delle,12/21/20) implement sliding friction between two objects 
	}
}

//adds an impulse to this entity, and this entity applies that impulse back on the sending object
//simply, changes velocity by impulse force
inline void PhysicsSystem::AddImpulse(Physics* creator, Physics* target, Vector3 impulse, bool ignoreMass) {
	target->velocity += ignoreMass ? impulse : impulse / target->mass;
	if (creator) { creator->velocity -= ignoreMass ? impulse : impulse / creator->mass; }
}