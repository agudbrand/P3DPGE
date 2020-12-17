#include "DebugSystem.h"
#include "../utils/Debug.h"
#include "../EntityAdmin.h"

#include "WorldSystem.h"
#include "MeshSystem.h"

#include "../components/InputSingleton.h"
#include "../components/Camera.h"
#include "../components/Keybinds.h"
#include "../components/MovementState.h"
#include "../components/Scene.h"
#include "../components/ScreenSingleton.h"
#include "../components/TimeSingleton.h"
#include "../components/WorldSingleton.h"

#include "../components/Light.h"
#include "../components/Mesh.h"
#include "../components/Transform.h"
#include "../components/Physics.h"

void DebugSystem::Update(float deltaTime, olc::PixelGameEngine* p) {
	InputSingleton*		input = admin->singletonInput;
	Camera*				camera = admin->tempCamera;
	Keybinds*			binds = admin->tempKeybinds;
	uint32				moveState = admin->tempMovementState->movementState;
	Scene*				scene = admin->tempScene;
	ScreenSingleton*	screen = admin->singletonScreen;
	TimeSingleton*		time = admin->singletonTime;
	WorldSingleton*		world = admin->singletonWorld;
	//WorldSystem*		systemWorld;	for(auto s : admin->systems) { if(WorldSystem* w = dynamic_cast<WorldSystem*>(s)) { systemWorld = w; break; } }
	
	//spawn box
	if(input->KeyPressed(olc::B)) {
		Entity* box;
		WorldSystem ws;
		ws.admin = admin;
		box = ws.CreateEntity();

		Transform* t = new Transform(Vector3(0, 0, 3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = MeshSystem().BuildBoxMesh(box, t, Vector3::ONE);
		ws.AddComponentsToEntity(box, {t, m});
		SUCCESS("B Pressed");
	}
	
	for(auto& pair : admin->entities) {
		for(Component* comp : pair.second->components) {
			if(Transform* t = dynamic_cast<Transform*>(comp)) {
				t->prevRotation = t->rotation;
				t->rotation += Vector3(5, 5, 5) * deltaTime;

				//rotate all entities +x
				if(input->KeyPressed(olc::L) || input->KeyHeld(olc::L)) {
					t->rotation += Vector3::RIGHT * 5 * deltaTime;
					LOG("+x");
				}
				//rotate all entities -x
				if(input->KeyPressed(olc::J) || input->KeyHeld(olc::J)) {
					t->rotation += Vector3::LEFT * 5 * deltaTime;
					LOG("-x");
				}
				//rotate all entities +y
				if(input->KeyPressed(olc::O) || input->KeyHeld(olc::O)) {
					t->rotation += Vector3::UP * 5 * deltaTime;
					LOG("+y");
				}
				//rotate all entities -y
				if(input->KeyPressed(olc::U) || input->KeyHeld(olc::U)) {
					t->rotation += Vector3::DOWN * 5 * deltaTime;
				}
				//rotate all entities +z
				if(input->KeyPressed(olc::I) || input->KeyHeld(olc::I)) {
					t->rotation += Vector3::FORWARD * 5 * deltaTime;
				}
				//rotate all entities -z
				if(input->KeyPressed(olc::K) || input->KeyHeld(olc::K)) {
					t->rotation += Vector3::BACK * 5 * deltaTime;
				}
			}
		}
	}

	////temporary ui vectors
	//std::vector<Button*> debug_b = std::vector<Button*>();
	//debug_b.push_back(new Button(([](olc::PixelGameEngine* p) {
	//	GLOBAL_DEBUG = !GLOBAL_DEBUG;
	//	}), "global_debug", ""));

	//debug_b.push_back(new Button(([](olc::PixelGameEngine* p) {
	//	Box* box = new Box(Vector3(1, 1, 1), Vector3(0, 0, 3));
	//	Input::selectedEntity = box;
	//	Physics::AddEntity(box);
	//	Scene::AddEntity(box);
	//	}), "spawn_box", ""));

	//debug_b.push_back(new Button(([](olc::PixelGameEngine* p) {
	//	//sub menu
	//	}), "spawn_complex", "", true, 0));

	//std::vector<Button*> cspawn_b;
	//cspawn_b.push_back(new Button(([](olc::PixelGameEngine* p) {
	//	Complex* complex = new Complex("objects/bmonkey.obj", Vector3(0, 0, 3));
	//	Input::selectedEntity = complex;
	//	Physics::AddEntity(complex);
	//	Scene::AddEntity(complex);
	//	}), "bmonkey", ""));

	//cspawn_b.push_back(new Button(([](olc::PixelGameEngine* p) {
	//	Complex* complex = new Complex("objects/whale_ship.obj", Vector3(0, 0, 3));
	//	Input::selectedEntity = complex;
	//	Physics::AddEntity(complex);
	//	Scene::AddEntity(complex);
	//	}), "whale_ship", ""));

	//cspawn_b.push_back(new Button(([](olc::PixelGameEngine* p) {
	//	Complex* complex = new Complex("objects/24k_Triangles.obj", Vector3(0, 0, 3));
	//	Input::selectedEntity = complex;
	//	Physics::AddEntity(complex);
	//	Scene::AddEntity(complex);
	//	}), "24k_Triangles", ""));


}