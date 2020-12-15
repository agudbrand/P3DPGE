#pragma once
#include "System.h"

struct RenderCanvasSystem : public System {
	//function description goes here
	void Update(float deltaTime, olc::PixelGameEngine* p) override;
};