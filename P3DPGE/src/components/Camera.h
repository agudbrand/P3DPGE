#pragma once
#include "Component.h"
#include "../math/Vector3.h"
#include "../math/Matrix4.h"

struct Camera : public Component {
	Vector3 position;
	Vector3 rotation;
	Vector3 lookDir;

	float nearZ; //the distance from the camera's position to screen plane
	float farZ; //the maximum render distance
	float fieldOfView;

	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;

	Camera() {
		nearZ = .1f;
		farZ = 1000.1f;
		fieldOfView = 90.f;
	}

	Camera(float fov, float nearZ = .1f, float farZ = 1000.f) {
		this->nearZ = nearZ;
		this->farZ = farZ;
		this->fieldOfView = fov;
	}
};