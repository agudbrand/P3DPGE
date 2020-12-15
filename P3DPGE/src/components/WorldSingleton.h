#pragma once
#include "Component.h"

struct Entity;

struct WorldSingleton : public Component {
	std::vector<Entity*> creationBuffer;
	std::vector<Entity*> deletionBuffer;

	WorldSingleton() {
		creationBuffer = std::vector<Entity*>();
		deletionBuffer = std::vector<Entity*>();
	}

	~WorldSingleton() {
		for(Entity* e : creationBuffer) delete e;
		creationBuffer.clear();
		for(Entity* e : deletionBuffer) delete e;
		deletionBuffer.clear();
	}
};