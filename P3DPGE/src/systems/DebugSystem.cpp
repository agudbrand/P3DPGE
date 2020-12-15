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
	Camera*				camera = admin->singletonCamera;
	Keybinds*			binds = admin->singletonKeybinds;
	uint32				moveState = admin->singletonMovementState->movementState;
	Scene*				scene = admin->singletonScene;
	ScreenSingleton*	screen = admin->singletonScreen;
	TimeSingleton*		time = admin->singletonTime;
	WorldSingleton*		world = admin->singletonWorld;
	
	//spawn box
	Entity* box;
	if(input->KeyPressed(olc::B)) {
		WorldSystem ws;
		ws.admin = admin;
		box = ws.CreateEntity({ new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE) });
		box->components.push_back(MeshSystem().BuildBoxMesh(box, Vector3(0, 0, 3), Vector3::ONE));
		SUCCESS("B Pressed");
	}

}