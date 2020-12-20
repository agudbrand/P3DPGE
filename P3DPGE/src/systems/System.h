#pragma once
#include "../utils/UsefulDefines.h"
#include <vector>

struct EntityAdmin;

struct System {
	EntityAdmin* admin; //reference to owning admin
	virtual void Init() {}
	virtual void Update() = 0;
	//virtual void NotifyComponent(Component*) = 0;
};