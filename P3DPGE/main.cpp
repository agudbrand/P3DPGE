#pragma once
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Input.h"
#include "Render.h"
using namespace olc;

/* General TODOs and NOTEs board

TODO: create system for handling multiple translations and rotations
TODO: get translation and rotation to work with inputs
TODO: define an object's local space so that we can rotate an object about its center
TODO: find a reason to either keep or remove Render::Init()
TODO: define a world origin in 3D space, how should we do this? For now its just 0, 0 in screen space.
TODO: write a function that takes in an angle and interpolates to that angle over a set time
TODO: write rotations to work with quaternions

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
	if (game.Construct(800, 400, 2, 2)) { game.Start(); }
}