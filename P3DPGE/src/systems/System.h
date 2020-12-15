#pragma once
#include "../utils/UsefulDefines.h"
#include <vector>

namespace olc {
	class PixelGameEngine;
}
struct EntityAdmin;

struct System {
	EntityAdmin* admin; //reference to owning admin
	virtual void Update(float deltaTime, olc::PixelGameEngine* p) = 0;
	//virtual void NotifyComponent(Component*) = 0;
};