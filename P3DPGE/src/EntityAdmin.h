#pragma once
#include "utils/UsefulDefines.h"
#include "utils/Debug.h"

/* TODOs
1.  create templated component tuple iterator that loops thru a vector and returns an iterator of components of templated type
3.  store Light and other components on entities
5.  cut away alot of triangle
6.  store all components in an object pool so that i can loop over that instead of entities [combine with 1]
7.  Add TODO of fix rotation moving object up
8.  add looking up/down
9.  cleanup all warnings
10. figure out which rotation is correct: transform vs mesh
11. rename singletons to normal

12. add drawPos and drawVertices
13. add entityRotation binds/commands
14. cut down physics to be better
15. add collision
16. update tick order and .cpp includes
*/

/*
---Systems Tick Order---||--------Read/Write Components---------||------------Read Only Components-------------------
  olcPixelGameEngine	|| InputSingleton						|| N/A
  TimeSystem			|| TimeSingleton, Command				|| N/A
  ScreenSystem			|| ScreenSingleton						|| N/A
  CommandSystem			|| Command, InputSingleton, Canvas		|| N/A 
  SimpleMovementSystem	|| Camera								|| InputSingleton, Keybinds, MovementState
						||										||	TimeSingleton
  PhysicsSystem			|| TimeSingleton, Transform, Physics	|| N/A
  CollisionSystem		|| 
  CameraSystem			|| Camera								|| ScreenSingleton
  MeshSystem			|| Mesh									|| Transform
  RenderSceneSystem		|| Scene								|| Mesh, Camera, InputSingleton, Keybinds, 
						||										||	ScreenSingleton, TimeSingleton, Transform
  RenderCanvasSystem	|| Canvas								|| ScreenSingleton
  WorldSystem			|| WorldSingleton, EntityAdmin,	Entity	|| N/A
  TriggeredCommandSystem|| Command								|| N/A
  DebugSystem			|| ALL									|| ALL
*/

typedef uint32 EntityID;
struct Entity;
struct System;
struct Component;
struct Command;

struct PhysicsWorld;
struct InputSingleton;
struct ScreenSingleton;
struct TimeSingleton;
struct WorldSingleton;

struct Camera;
struct Keybinds;
struct Scene;
struct MovementState;
struct Canvas;

struct EntityAdmin {
	olc::PixelGameEngine* p;
	std::vector<System*> systems;
	std::map<EntityID, Entity*> entities;
	//object_pool<Component>* componentsPtr;
	std::vector<Component*> components;
	std::map<std::string, Command*> commands;
	PhysicsWorld* physicsWorld;

	//singletons
	InputSingleton* singletonInput;
	ScreenSingleton* singletonScreen;
	TimeSingleton* singletonTime;
	WorldSingleton* singletonWorld;

	Camera* currentCamera;
	Keybinds* currentKeybinds;
	Scene* currentScene;
	MovementState* tempMovementState;
	Canvas* tempCanvas;

	void Create(olc::PixelGameEngine* p);
	void Cleanup();

	void Update();

	void AddSystem(System* system);
	void RemoveSystem(System* system);

	void AddComponent(Component* component);
	void RemoveComponent(Component* component);

	Command* GetCommand(std::string command);
	bool ExecCommand(std::string command);
	bool TriggerCommand(std::string command);
};

struct Entity {
	EntityAdmin* admin; //reference to owning admin
	EntityID id;
	std::vector<Component*> components;

	//returns a component pointer from the entity of provided type, nullptr otherwise
	template<class T>
		T* GetComponent(){
		T* t = nullptr;
		for(Component* c : components) { if(T* temp = dynamic_cast<T*>(c)) { t = temp; break; } }
		return t;
	}

	//adds a component to the end of the components vector
	//returns the position in the vector
	uint32 AddComponent(Component* component);

	~Entity();
}; //TODO(,delle) move WorldSystem entity-component functions into Entity