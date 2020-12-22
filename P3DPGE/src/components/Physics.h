#pragma once
#include "Component.h"
#include "../math/Vector3.h"

struct Physics : public Component {
	Vector3 position;
	Vector3 rotation;

	Vector3 velocity;
	Vector3 acceleration;
	Vector3 rotVelocity;
	Vector3 rotAcceleration;
	float mass;

	std::vector<Vector3> forces;
	Vector3 inputVector;

	Physics(Vector3 position, Vector3 rotation, Vector3 velocity = Vector3::ZERO, Vector3 acceleration = Vector3::ZERO, Vector3 rotVeloctiy = Vector3::ZERO, 
						Vector3 rotAcceleration = Vector3::ZERO, float mass = 1.f){
		this->position = position;
		this->rotation = rotation;
		this->velocity = velocity;
		this->acceleration = acceleration;
		this->rotVelocity = rotVeloctiy;
		this->rotAcceleration = rotAcceleration;
		this->mass = mass;
	}
};