#pragma once
#include "Component.h"
#include "../math/Vector3.h"

struct Physics : public Component {
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 rotVeloctiy;
	Vector3 rotAcceleration;
	float mass;
	
	//std::vector<Vector3> forces;

	Physics(){
		velocity = Vector3::ZERO;
		acceleration = Vector3::ZERO;
		rotVeloctiy = Vector3::ZERO;
		rotAcceleration = Vector3::ZERO;
		mass = 0.f;
	}

	Physics(Vector3 velocity, Vector3 acceleration = Vector3::ZERO, Vector3 rotVeloctiy = Vector3::ZERO, 
						Vector3 rotAcceleration = Vector3::ZERO, float mass = 0.f){
		this->velocity = velocity;
		this->acceleration = acceleration;
		this->rotVeloctiy = rotVeloctiy;
		this->rotAcceleration = rotAcceleration;
		this->mass = mass;
	}
};