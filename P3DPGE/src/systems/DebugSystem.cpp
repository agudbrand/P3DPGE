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

void DebugSystem::Update() {
	InputSingleton*		input = admin->singletonInput;
	Camera*				camera = admin->tempCamera;
	Keybinds*			binds = admin->tempKeybinds;
	uint32				moveState = admin->tempMovementState->movementState;
	Scene*				scene = admin->tempScene;
	ScreenSingleton*	screen = admin->singletonScreen;
	TimeSingleton*		time = admin->singletonTime;
	WorldSingleton*		world = admin->singletonWorld;
	float				deltaTime = time->deltaTime;
	//WorldSystem*		systemWorld;	for(auto s : admin->systems) { if(WorldSystem* w = dynamic_cast<WorldSystem*>(s)) { systemWorld = w; break; } }
	
	for(auto& pair : admin->entities) {
		for(Component* comp : pair.second->components) {
			if(Transform* t = dynamic_cast<Transform*>(comp)) {
				t->prevRotation = t->rotation;
				t->rotation += Vector3(5, 5, 5) * deltaTime;

				//rotate all entities +x
				if(input->KeyDown(olc::L)) {
					t->rotation += Vector3::RIGHT * 5 * deltaTime;
					LOG("+x");
				}
				//rotate all entities -x
				if(input->KeyDown(olc::J)) {
					t->rotation += Vector3::LEFT * 5 * deltaTime;
					LOG("-x");
				}
				//rotate all entities +y
				if(input->KeyDown(olc::O)) {
					t->rotation += Vector3::UP * 5 * deltaTime;
					LOG("+y");
				}
				//rotate all entities -y
				if(input->KeyDown(olc::U)) {
					t->rotation += Vector3::DOWN * 5 * deltaTime;
				}
				//rotate all entities +z
				if(input->KeyDown(olc::I)) {
					t->rotation += Vector3::FORWARD * 5 * deltaTime;
				}
				//rotate all entities -z
				if(input->KeyDown(olc::K)) {
					t->rotation += Vector3::BACK * 5 * deltaTime;
				}
			}
		}
	}
}