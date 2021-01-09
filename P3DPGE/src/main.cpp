#pragma once
#define OLC_PGE_APPLICATION
#define KEYBOARD_LAYOUT_US_UK
#define DEBUG_P3DPGE

#include "internal/olcPixelGameEngine.h"
#include "EntityAdmin.h"

using namespace olc;

//// Requires boost library 1.74.0 ////

/* Changes to olcPixelGameEngine.h, see line 1000
1. Changed PixelGameEngine.pKeyboardState to public
2. Changed PixelGameEngine.pMouseState to public
*/

class P3DPGE : public PixelGameEngine {
public:
	P3DPGE() { sAppName = "P3DPGE"; }

	EntityAdmin entityAdmin;

	bool OnUserCreate() override {
		entityAdmin.Create(this);
		return true;
	}

	bool OnUserUpdate(float deltaTime) {
		entityAdmin.Update();
		return true;
	}

	bool OnUserDestroy() {
		entityAdmin.Cleanup();
		return true;
	}
};

int main() {
	srand(time(0));
	P3DPGE game;
	if (game.Construct(1280, 720, 1, 1, false, false)) { game.Start(); }
}