#include "DebugSystem.h"
#include "../utils/Debug.h"
#include "../EntityAdmin.h"
#include "../math/Math.h"

#include "WorldSystem.h"
#include "MeshSystem.h"
#include "PhysicsSystem.h"

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

void DebugSystem::Init() {
}

void DebugSystem::Update() {
	InputSingleton*		input = admin->singletonInput;
	Camera*				camera = admin->currentCamera;
	Keybinds*			binds = admin->currentKeybinds;
	uint32				moveState = admin->tempMovementState->movementState;
	Scene*				scene = admin->currentScene;
	ScreenSingleton*	screen = admin->singletonScreen;
	TimeSingleton*		time = admin->singletonTime;
	WorldSingleton*		world = admin->singletonWorld;
	float				deltaTime = time->deltaTime;

	//add temporary debugging things here

}