#pragma once
#define OLC_PGE_APPLICATION

#include "internal/olcPixelGameEngine.h"
#include "Input.h"

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

class P3DPGE : public PixelGameEngine {
public:
	Timer* timer;
	P3DPGE() { sAppName = "P3DPGE"; timer = new Timer; }

	bool OnUserCreate() override {

		Time::Init();
		Input::Init();
		Physics::Init();
		Render::Init();
		Scene::Init();

		g_cBuffer.allocate_space(100);
		Render::pDepthBuffer = new float[ScreenWidth() * ScreenHeight()];

		screenHeight = ScreenHeight();
		screenWidth = ScreenWidth();

		std::vector<Button*> debug_b;

		debug_b.push_back(new Button(([](olc::PixelGameEngine* p) {
			GLOBAL_DEBUG = !GLOBAL_DEBUG;
			}), "global_debug", ""));

		debug_b.push_back(new Button(([](olc::PixelGameEngine* p) {
			Box* box = new Box(Vector3(1, 1, 1), Vector3(0, 0, 3));
			Input::selectedEntity = box;
			Physics::AddEntity(box);
			Scene::AddEntity(box);
			}), "spawn_box", ""));

		debug_b.push_back(new Button(([](olc::PixelGameEngine* p) {
			//sub menu
			}), "spawn_complex", "", true, 0));

		std::vector<Button*> cspawn_b;

		cspawn_b.push_back(new Button(([](olc::PixelGameEngine* p) {
			Complex* complex = new Complex("objects/bmonkey.obj", Vector3(0, 0, 3));
			Input::selectedEntity = complex;
			Physics::AddEntity(complex);
			Scene::AddEntity(complex);
			}), "bmonkey", ""));

		cspawn_b.push_back(new Button(([](olc::PixelGameEngine* p) {
			Complex* complex = new Complex("objects/whale_ship.obj", Vector3(0, 0, 3));
			Input::selectedEntity = complex;
			Physics::AddEntity(complex);
			Scene::AddEntity(complex);
			}), "whale_ship", ""));

		cspawn_b.push_back(new Button(([](olc::PixelGameEngine* p) {
			Complex* complex = new Complex("objects/24k_Triangles.obj", Vector3(0, 0, 3));
			Input::selectedEntity = complex;
			Physics::AddEntity(complex);
			Scene::AddEntity(complex);
			}), "24k_Triangles", ""));

		Scene::ui_layer.push_back(new Menu(Vector2(50, 50), "debug_menu", "dm", debug_b,
			std::vector<Menu*>{new Menu(V2ZERO, "complex_spawn", "", cspawn_b)}));

		return true;
	}

	bool OnUserUpdate(float deltaTime) {
		Clear(olc::BLACK);

		Time::Update(deltaTime);
		Input::Update(this, Time::deltaTime);
		Physics::Update(Time::fixedDeltaTime);
		Scene::Update(this, Time::deltaTime);
		Render::Update(this);

		return true;
	}

	bool OnUserDestroy() {
		Time::Cleanup();
		Input::Cleanup();
		Physics::Cleanup();
		Render::Cleanup();
		Scene::CleanUp();
		return true;
	}
};

int main() {
	srand(time(0));
	P3DPGE game;
	if (game.Construct(1280, 720, 1, 1, false, false)) { game.Start(); }
}