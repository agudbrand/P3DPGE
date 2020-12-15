#pragma once
#include "System.h"

struct TimeSystem : public System {
	void Update(float deltaTime, olc::PixelGameEngine* p) override;
};