#pragma once
#define OLC_PGE_APPLICATION
#include "internal/olcPixelGameEngine.h"

#define KEYBOARD_LAYOUT_US_UK
#define DEBUG_P3DPGE
#include "EntityAdmin.h"

using namespace olc;

//// Requires boost library 1.74.0 ////

/* Changes to olcPixelGameEngine.h, see line 1000
1. Changed PixelGameEngine.pKeyboardState to public
2. Changed PixelGameEngine.pMouseState to public
*/

class P3DPGE : public PixelGameEngine {
public:
	EntityAdmin entityAdmin;
	int gameLayer;

	P3DPGE() { sAppName = "P3DPGE"; }

	bool OnUserCreate() override {
		//Create a new Layer which will be used for the game
		gameLayer = CreateLayer();
		//The layer is not enabled by default,  so we need to enable it
		EnableLayer(gameLayer, true);

		entityAdmin.Create(this);
		return true;
	}

	bool OnUserUpdate(float deltaTime) {
		//Change the Draw Target to not be Layer 0
		SetDrawTarget((uint8_t)gameLayer);

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