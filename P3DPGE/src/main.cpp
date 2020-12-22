#pragma once
#define OLC_PGE_APPLICATION
#define KEYBOARD_LAYOUT_US_UK
#define DEBUG_P3DPGE

#include "internal/olcPixelGameEngine.h"
#include "EntityAdmin.h"

using namespace olc;

//// Requires boost library 1.74.0 ////

/* General TODOs and NOTEs board
   TODOs should be ordered about NOTEs and TODOs should be listed in order of
   severity.

Tags: + GitIssue, s Severe, u Unimportant, p Physics, r Render, e Entity, i Input,
	  m Math, o Optimization, g General, c Clean Up Code

TODO(g, sushi) create a WorldMatrix that takes in several matrix operations and
	  gets there product in the order specified then apply that final matrix
	  to the object. this would probably eliminate the need to constantly
	  throw the object between local and world space everytime we do an
	  operation on it

TODO(p,delle) add physics based collision resolution for all entities

TODO(i,delle) convert inputs to new format, method to add and remove inputs

*/

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