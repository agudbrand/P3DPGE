#include "DebugSystem.h"
#include "../utils/Debug.h"
#include "../EntityAdmin.h"
#include "../math/Math.h"

#include "WorldSystem.h"
#include "MeshSystem.h"
#include "PhysicsSystem.h"

#include "../components/Input.h"
#include "../components/Camera.h"
#include "../components/Keybinds.h"
#include "../components/MovementState.h"
#include "../components/Scene.h"
#include "../components/Screen.h"
#include "../components/Time.h"
#include "../components/World.h"
#include "../components/Light.h"
#include "../components/Mesh.h"
#include "../components/Transform.h"
#include "../components/Physics.h"

void DebugSystem::Init() {
}

void DebugSystem::Update() {
	Input*		input = admin->singletonInput;
	Camera*				camera = admin->currentCamera;
	Keybinds*			binds = admin->currentKeybinds;
	uint32				moveState = admin->tempMovementState->movementState;
	Scene*				scene = admin->currentScene;
	Screen*	screen = admin->singletonScreen;
	Time*		time = admin->singletonTime;
	World*		world = admin->singletonWorld;
	float				deltaTime = time->deltaTime;

	//add temporary debugging things here

}