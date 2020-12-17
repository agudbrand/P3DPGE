#pragma once
#include "System.h"

struct SimpleMovementSystem : public System {
	void Update(float deltaTime, olc::PixelGameEngine* p) override;
};