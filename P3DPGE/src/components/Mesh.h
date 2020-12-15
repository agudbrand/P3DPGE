#pragma once
#include "Component.h"

#include "../geometry/Triangle.h"
#include "../animation/Armature.h"

struct Mesh : public Component {
	Armature* armature;
	std::vector<Triangle> triangles;

	Mesh() {
		armature = 0;
		triangles = std::vector<Triangle>();
	}

	Mesh(std::vector<Triangle> triangles, Armature* armature = 0) {
		this->armature = armature;
		this->triangles = triangles;
	}

	~Mesh() {
		if(armature) { //TODO(,delle) test if this format is correct
			delete armature;
		}
	}
};