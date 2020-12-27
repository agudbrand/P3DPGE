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


*/

/* ECS conversion TODOs

16. update tick order and .cpp includes

1.  create templated component tuple iterator that loops thru a vector and returns an iterator of components of templated type
3.  store Light and other components on entities
5.  cut away alot of triangle
6.  store all components in an object pool so that i can loop over that instead of entities [combine with 1]
8.  add looking up/down
9.  cleanup all warnings
11. rename singletons to normal
14. cut down physics to be better
18. figure out why rotation degenerates in collision
19. rotation around y is local but every other axis is global? (might be because of perspective matrix)

*/

/*
---Systems Tick Order---||--------Read/Write Components---------||------------Read Only Components-------------------
  olcPixelGameEngine	|| InputSingleton						|| N/A
  TimeSystem			|| TimeSingleton, Command				|| N/A
  ScreenSystem			|| ScreenSingleton						|| N/A
  CommandSystem			|| Command, InputSingleton, Canvas		|| N/A 
  SimpleMovementSystem	|| Camera								|| InputSingleton, Keybinds, MovementState
						||										||	TimeSingleton
  PhysicsSystem			|| TimeSingleton, Transform, Physics	|| Camera, Screen
  CameraSystem			|| Camera								|| ScreenSingleton
  MeshSystem			|| Mesh									|| Transform
  RenderSceneSystem		|| Scene								|| Mesh, Camera, InputSingleton, Keybinds, 
						||										||	ScreenSingleton, TimeSingleton, Transform
  RenderCanvasSystem	|| Canvas								|| ScreenSingleton
  WorldSystem			|| WorldSingleton, EntityAdmin,	Entity	|| N/A
  TriggeredCommandSystem|| Command								|| N/A
  DebugSystem			|| ALL									|| ALL
*/

#include "EntityAdmin.h"						//UsefulDefines.h, Debug.h
#include "utils/PhysicsWorld.h"					//
#include "utils/Command.h"						//Debug.h
//#include "utils/UsefulDefines.h"				//olcPixelGameEngine.h
//#include "math/Math.h"						//UsefulDefines.h, Vector3.h, Vector4.h, Matrix3.h, Matrix4.h, MatrixN.h,
												//	<math.h>, <algorithm>, <numeric>
//#include "geometry/Edge.h"					//Math.h
//#include "geometry/Triangle.h"				//Math.h, Edge.h


//component includes
#include "components/Component.h"				//UsefulDefines.h, <vector>
#include "components/InputSingleton.h"			//Component.h, Vector3.h
#include "components/ScreenSingleton.h"			//Component.h, Vector3.h
#include "components/TimeSingleton.h"			//Component.h, <time.h>
#include "components/WorldSingleton.h"			//Component.h
#include "components/Camera.h"					//Component.h, Vector3.h, Matrix4.h
#include "components/Keybinds.h"				//Component.h
#include "components/MovementState.h"			//Component.h
#include "components/Scene.h"					//Component.h
#include "components/Canvas.h"					//Component.h, UI.h
//#include "components/Mesh.h"					//Component.h, Vector3.h, Triangle.h, Armature.h
//#include "components/Light.h"					//Component.h, Vector3.h 
//#include "components/Physics.h"				//Component.h, Vector3.h
//#include "components/Transform.h"				//Component.h, Vector3.h, Matrix4.h

//system includes
#include "systems/System.h"						//EntityAdmin.h
#include "systems/TimeSystem.h"					//System.h |cpp->| TimeSingleton.h, Command.h
#include "systems/ScreenSystem.h"				//System.h |cpp->| ScreenSingleton.h
#include "systems/CommandSystem.h"				//System.h |cpp->| Command.h, InputSingleton.h, Canvas.h
#include "systems/SimpleMovementSystem.h"		//System.h |cpp->| InputSingleton.h, Keybinds.h, Camera.h, MovementState.h, TimeSingleton.h
#include "systems/PhysicsSystem.h"				//System.h |cpp->| PhysicsWorld.h, Math.h, Transform.h, Physics.h, InputSingleton.h,
												//		   |	 |	Command.h, InputSingleton.h, TimeSingleton.h, Camera.h, ScreenSingleton.h
#include "systems/CameraSystem.h"				//System.h |cpp->| Camera.h, ScreenSingleton.h, Command.h
#include "systems/MeshSystem.h"					//System.h |cpp->| Mesh.h, Transform.h, Physics.h, 
												//		   |	 |	Command.h, InputSingleton.h, Camera.h, Scene.h, ScreenSingleton.h
#include "systems/RenderSceneSystem.h"			//System.h |cpp->| Math.h, Scene.h, Mesh.h, Camera.h, Light.h, ScreenSingleton.h, Transform.h, Command.h
#include "systems/RenderCanvasSystem.h"			//System.h |cpp->| Canvas.h, ScreenSingleton.h
#include "systems/WorldSystem.h"				//System.h |cpp->| WorldSingleton.h, Transform.h, Mesh.h, Command.h, InputSingleton.h
#include "systems/TriggeredCommandSystem.h"		//System.h |cpp->| Command.h

//debug includes
#ifdef DEBUG_P3DPGE
#include "systems/DebugSystem.h"
#endif

//// EntityAdmin ////

void EntityAdmin::Create(olc::PixelGameEngine* p) {
	this->p = p;
	systems = std::vector<System*>();
	entities = std::map<EntityID, Entity*>();
	components = std::vector<Component*>();
	commands = std::map<std::string, Command*>();
	physicsWorld = new PhysicsWorld();

	//singleton initialization
	singletonInput = new InputSingleton(p);
	singletonScreen = new ScreenSingleton(p);
	singletonTime = new TimeSingleton();
	singletonWorld = new WorldSingleton();

	//current admin components
	currentCamera = new Camera();
	currentScene = new Scene(p);
	currentKeybinds = new Keybinds();

	//temporary singletons
	tempMovementState = new MovementState();
	tempCanvas = new Canvas();

	//systems initialization
	AddSystem(new TimeSystem());
	AddSystem(new ScreenSystem());
	AddSystem(new CommandSystem());
	AddSystem(new SimpleMovementSystem());
	switch(physicsWorld->integrationMode) {
		default: /* Semi-Implicit Euler */ {
			AddSystem(new PhysicsSystem());
		}
	}
	AddSystem(new CameraSystem());
	AddSystem(new MeshSystem());
	AddSystem(new RenderSceneSystem());
	AddSystem(new RenderCanvasSystem());
	AddSystem(new WorldSystem());
	AddSystem(new TriggeredCommandSystem());
#ifdef DEBUG_P3DPGE
	AddSystem(new DebugSystem());
#endif	
}

void EntityAdmin::Cleanup() {
	//cleanup collections
	for(System* s : systems)		{ delete s; }			systems.clear();
	for(auto pair : entities)		{ delete pair.second; }	entities.clear();
	for(Component* c : components)	{ delete c; }			components.clear();
	for(auto pair : commands)		{ delete pair.second; }	commands.clear();
	delete physicsWorld;

	//clean up singletons
	delete singletonInput;
	delete singletonScreen;
	delete singletonTime;
	delete singletonWorld;
	delete currentCamera;
	delete currentKeybinds;
	delete tempMovementState;
	delete currentScene;
	delete tempCanvas;
}

void EntityAdmin::Update() {
	for(System* s : systems) {
		s->Update();
	}
}

void EntityAdmin::AddSystem(System* system) {
	systems.push_back(system);
	system->admin = this;
	system->Init();
}

void EntityAdmin::RemoveSystem(System* system) {
	for(int i = 0; i < systems.size(); ++i) {
		if(systems[i] == system) {
			systems.erase(systems.begin() + i);
		}
	}
}

void EntityAdmin::AddComponent(Component* component) {
	components.push_back(component);
	component->entity = 0;
}

void EntityAdmin::RemoveComponent(Component* component) {
	for(int i = 0; i < components.size(); ++i) {
		if(components[i] == component) {
			components.erase(components.begin() + i);
		}
	}
}

Command* EntityAdmin::GetCommand(std::string command) {
	try {
		return commands.at(command);
	} catch(std::exception e) {
		ERROR("Command \"", command, "\" does not exist");
		return 0;
	}
}

bool EntityAdmin::ExecCommand(std::string command) {
	try {
		commands.at(command)->Exec(this);
		return true;
	} catch(std::exception e) {
		ERROR("Command \"", command, "\" does not exist");
		return false;
	}
}

bool EntityAdmin::TriggerCommand(std::string command) {
	try {
		commands.at(command)->triggered = true;
		return true;
	} catch(std::exception e) {
		ERROR("Command \"", command, "\" does not exist");
		return false;
	}
}



//// Entity ////

uint32 Entity::AddComponent(Component* component) {
	components.push_back(component);
	component->entity = this;
	return components.size()-1;
}

uint32 Entity::AddComponents(std::vector<Component*> comps) {
	uint32 value = this->components.size();
	for(auto& c : comps) {
		this->components.push_back(c);
		c->entity = this;
	}
	return value;
}

Entity::~Entity() {
	for(Component* c : components) delete c;
}