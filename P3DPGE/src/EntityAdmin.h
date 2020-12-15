#pragma once
#include "utils/UsefulDefines.h"
#include "systems/System.h"
#include "components/Component.h"

//singleton includes
#include "components/InputSingleton.h"
#include "components/ScreenSingleton.h"
#include "components/TimeSingleton.h"
#include "components/WorldSingleton.h"
#include "components/Camera.h"
#include "components/Keybinds.h"
#include "components/MovementState.h"
#include "components/Scene.h"
#include "components/Canvas.h"

//system includes
#include "systems/TimeSystem.h"
#include "systems/SimpleMovementSystem.h"
#include "systems/ScreenSystem.h"
#include "systems/CameraSystem.h"
#include "systems/MeshSystem.h"
#include "systems/RenderSceneSystem.h"
#include "systems/RenderCanvasSystem.h"
#include "systems/WorldSystem.h"

//debug includes
#include "systems/DebugSystem.h"

/* TODOs
* 
1. create templated component iterator that loops thru a vector and returns an iterator of components of templated type
2. create UI
3. store Light and other components on entities
4. think about assigning entities ids based on thier components
5. cut away alot of triangle
*/

/*
---Systems Tick Order---||----Read/Write Components-----||-----------Read Only Components--------------
  olcPixelGameEngine	||								||
  TimeSystem			|| TimeSingleton				||
  SimpleMovementSystem	|| Camera						|| InputSingleton, Keybinds, MovementState
  ScreenSystem			|| ScreenSingleton				||
  CameraSystem			|| Camera						|| ScreenSingleton
  MeshSystem			|| Mesh
  RenderSceneSystem		|| Scene						|| Mesh, Camera, InputSingleton, Keybinds,  
						||								||	ScreenSingleton, TimeSingleton
  RenderCanvasSystem	|| Canvas						|| ScreenSingleton
  WorldSystem			|| WorldSingleton, EntityAdmin, ||
						||	Entity						||
  DebugSystem			|| N/A							|| N/A
*/

typedef uint32 EntityID;
struct Entity {
	EntityAdmin* admin; //reference to owning admin
	EntityID id;
	std::vector<Component*> components;
	//resourceHandle

	~Entity() {
		for(Component* c : components) delete c;
	}
};

struct EntityAdmin {
	olc::PixelGameEngine* p;
	std::vector<System*> systems;
	std::map<EntityID, Entity*> entities;
	//object_pool<Component>* componentsPtr;
	std::vector<Component*> components;

	//singletons
	InputSingleton* singletonInput;
	ScreenSingleton* singletonScreen;
	TimeSingleton* singletonTime;
	WorldSingleton* singletonWorld;

	Camera* singletonCamera;
	Keybinds* singletonKeybinds;
	MovementState* singletonMovementState;
	Scene* singletonScene;
	Canvas* singletonCanvas;

	void AddSystem(System* system) {
		systems.push_back(system);
		system->admin = this;
	}

	void RemoveSystem(System* system) {
		for(int i = 0; i < systems.size(); ++i) {
			if(systems[i] == system) {
				systems.erase(systems.begin() + i);
			}
		}
	}

	void AddComponent(Component* component) {
		components.push_back(component);
		component->entity = 0;
	}

	void RemoveComponent(Component* component) {
		for(int i = 0; i < components.size(); ++i) {
			if(components[i] == component) {
				components.erase(components.begin() + i);
			}
		}
	}

	void Create(olc::PixelGameEngine* p) {
		this->p = p;
		systems = std::vector<System*>();
		entities = std::map<EntityID, Entity*>();
		components = std::vector<Component*>();

		//singleton initialization
		singletonInput = new InputSingleton(p);
		singletonScreen = new ScreenSingleton(p);
		singletonTime = new TimeSingleton();
		singletonWorld = new WorldSingleton();
		//temp (maybe) singletons
		singletonCamera = new Camera();
		singletonKeybinds = new Keybinds();
		singletonMovementState = new MovementState();
		singletonScene = new Scene(p);

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

		singletonCanvas = new Canvas();

		//systems initialization
		AddSystem(new TimeSystem());
		AddSystem(new SimpleMovementSystem());
		AddSystem(new ScreenSystem());
		AddSystem(new CameraSystem());
		AddSystem(new MeshSystem());
		AddSystem(new RenderSceneSystem());
		AddSystem(new RenderCanvasSystem());
		AddSystem(new WorldSystem());
		AddSystem(new DebugSystem());
	}

	void Update(float deltaTime) {
		for(System* s : systems) {
			s->Update(deltaTime, p);
		}
	}

	void Cleanup() {
		//cleanup collections
		for(System* s : systems)		{ delete s; }			systems.clear();
		for(auto it : entities)			{ delete it.second; }	entities.clear();
		for(Component* c : components)	{ delete c; }			components.clear();

		//clean up singletons
		delete singletonInput;
		delete singletonScreen;
		delete singletonTime;
		delete singletonCamera;
		delete singletonKeybinds;
		delete singletonMovementState;
		delete singletonScene;
	}

};