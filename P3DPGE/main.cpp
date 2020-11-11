#pragma once
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Input.h"
#include "Render.h"
using namespace olc;

/* General TODOs and NOTEs board

TODO: find a reason to either keep or remove Render::Init()

NOTE: it may be benefitial to have an objects triangles always be defined in
	  local space so we don't have to keep translating between world and local
	  everytime we do something to it. but this may cause other issues.

*/

class P3DPGE : public PixelGameEngine {
private:
	float time;

public:
	P3DPGE() { sAppName = "P3DPGE"; }

	bool OnUserCreate() override {

		time = 0;
		Physics::Init();
		Render::Init();
		
		return true;
	}
	
	bool OnUserUpdate(float deltaTime) {
		Clear(olc::BLACK);

		//time
		time += deltaTime;

		//input
		Input::Update(this, deltaTime);

		//physics
		Physics::Update(deltaTime);

		//rendering
		Render::Update(this);

		return true;
	}

	bool OnUserDestroy() {
		Input::Cleanup();
		Physics::Cleanup();
		Render::Cleanup();

		return true;
	}
	
};

int main() {
	P3DPGE game;
	if (game.Construct(600, 600, 1, 1)) { game.Start(); }
}