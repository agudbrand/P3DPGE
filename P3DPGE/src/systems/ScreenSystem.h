#pragma once
#include "System.h"

struct ScreenSystem : public System {
	//updates the singleton screen component's dimensions if there was a change
	void Update(float deltaTime, olc::PixelGameEngine* p) override;
};