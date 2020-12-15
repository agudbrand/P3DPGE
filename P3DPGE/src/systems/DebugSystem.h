#pragma once
#include "System.h"

struct DebugSystem : public System {
	void Update(float deltaTime, olc::PixelGameEngine* p) override;
};
