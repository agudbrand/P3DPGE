#pragma once
#include "utils/UsefulDefines.h"
#include "utils/Debug.h"

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

	uint32 AddComponents(std::vector<Component*> components);

	~Entity();
}; //TODO(,delle) move WorldSystem entity-component functions into Entity