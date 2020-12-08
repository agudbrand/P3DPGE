#pragma once
#define OLC_PGE_APPLICATION
#include "internal/olcPixelGameEngine.h"
#include "Input.h"
#include "Matrix.h"
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

TODO(g,delle) create a debug header for easy printing and such

NOTE sushi: it may be benefitial to have an objects triangles always be defined in
	  local space so we don't have to keep translating between world and local
	  everytime we do something to it. but this may cause other issues

NOTE sushi: should we use Blender to import objects or make our own tool?
	  maybe use Blender then make a custom one later? dunno, it'd
	  be a fun project to build our own tool

NOTE sushi: currently, generating an object relative to mouse position
	  does not work in 3D

*/

class P3DPGE : public PixelGameEngine {
public:
	Timer* timer;
	P3DPGE() { sAppName = "P3DPGE"; timer = new Timer; }

	bool OnUserCreate() override {
		Time::Init();
		Input::Init();
		Physics::Init();
		Render::Init();

		//Matrix test1(1, 4, { 2,1,2,3 });
		//Matrix test2(1, 4, { 1,2,3,4 });

		return true;
	}

	bool OnUserUpdate(float deltaTime) {

		Clear(olc::BLACK);

		//time
		Time::Update(deltaTime);

		//input
		Input::Update(this, Time::deltaTime);

		//physics
		//Physics::Update(Time::fixedDeltaTime);

		//rendering
		Render::Update(this);

		//Debug::Print("string: %s, decimal: %d, integer: %i", V3ZERO.str(), 1, 2);
		//Debug::Print("float: %f, .2 float: %.2f, double: %f", 5.1234f, 5.1234f, 2.0);

		return true;
	}

	bool OnUserDestroy() {
		Time::Cleanup();
		Input::Cleanup();
		Physics::Cleanup();
		Render::Cleanup();

		return true;
	}
};

int main() {
	P3DPGE game;
	if (game.Construct(1280, 720, 1, 1, false, false)) { game.Start(); }
}