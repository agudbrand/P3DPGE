#pragma once
#include <vector>
#include "../utils/UsefulDefines.h"

struct Entity;

struct Component {
	Entity* entity = nullptr; //reference to owning entity
	//virtual void Create(resourceHandle) = 0;
	virtual void Create() {};
	virtual ~Component() {};
};