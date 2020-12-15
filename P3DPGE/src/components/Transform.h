#pragma once
#include "Component.h"
#include "../math/Vector3.h"
//#include "../math/Quaternion.h"

struct Transform : public Component {
	Vector3 position;
	//Quaternion rotation;
	Vector3 rotation;
	Vector3 scale;

	Vector3 prevPosition;
	//Quaternion prevRotation;
	Vector3 prevRotation;
	Vector3 prevScale;

	Transform() {
		position = Vector3::ZERO;
		//rotation = Quaternion();
		rotation = Vector3::ZERO;
		scale = Vector3::ONE;
		prevPosition = Vector3::ZERO;
		//prevRotation = Quaternion();
		prevRotation = Vector3::ZERO;
		prevScale = Vector3::ONE;
	}

	/*Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale) {
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
		prevPosition = position;
		prevRotation = rotation;
		prevScale = scale;
	}*/

	Transform(const Vector3& position, const Vector3& rotation, const Vector3& scale) {
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
		prevPosition = position;
		prevRotation = rotation;
		prevScale = scale;
	}
};