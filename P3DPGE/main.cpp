#pragma once
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Input.h"
#include "Render.h"
using namespace olc;


//// Requires boost library 1.74.0 ////


/* General TODOs and NOTEs board
   TODOs should be ordered about NOTEs and TODOs should be listed in order of
   severity.

TODO: create a WorldMatrix that takes in several matrix operations and
	  gets there product in the order specified then apply that final matrix
	  to the object. this would probably eliminate the need to constantly
	  throw the object between local and world space everytime we do an 
	  operation on it. //sushi

TODO: put mesh and triangle somewhere better than entities
	  since these are the only two things that aren't entities
	  and often I find myself wanting to call functions outside
	  of entity, it would be best to put these somewhere else
	  maybe in Render itself //sushi

TODO: find a reason to either keep or remove Render::Init() //sushi

TODO: attempt to implement the depth buffer explained in Javid's 
	  3D engine part 2 video. this will probably be very complex 
	  as we'll need a way to tell PGE whether or not it draws
	  a single pixel based on the depth buffer. so maybe do this much later.
	  either do this or just try to implement a better algorithm. //sushi

NOTE: it may be benefitial to have an objects triangles always be defined in
	  local space so we don't have to keep translating between world and local
	  everytime we do something to it. but this may cause other issues. //sushi

NOTE: should we use Blender to import objects or make our own tool?
	  maybe use Blender then make a custom one later? dunno, it'd 
	  be a fun project to build our own tool. //sushi

NOTE: currently, generating an object relative to mouse position
	  does not work in 3D. //sushi

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
	if (game.Construct(600, 600, 2, 2)) { game.Start(); }
}