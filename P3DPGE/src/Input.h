#pragma once
#include "Render.h"
#include "Physics.h"
#include "internal/olcPixelGameEngine.h"

//#include "Collider.h" //TODO(i,delle) remove this

//TODO(io,delle,11/17/20) look into heap vs stack memory allocation for the func pointer
typedef void (*Action)(olc::PixelGameEngine* p);

//NOTE update InputAction to support ALT when olcPGE does
struct InputAction {
	olc::Key key;
	int mouseButton;
	int inputState; //0 = bPressed, 1 = bHeld, 2 = bReleased
	bool bShiftHeld;
	bool bCtrlHeld;
	//bool bAltHeld;
	std::string name;
	std::string description;
	Action action;

	//function, name, key, mouseButton, inputState, shift, ctrl, desc
	InputAction(Action action, std::string name, olc::Key key = olc::Key::NONE, int mouseButton = -1, int inputState = 0,
		bool bShiftHeld = false, bool bCtrlHeld = false, /*bool bAltHeld = false,*/ std::string description = "") {
		this->key = key;
		this->mouseButton = mouseButton;
		this->inputState = inputState;
		this->bShiftHeld = bShiftHeld;
		this->bCtrlHeld = bCtrlHeld;
		//this->bAltHeld = bAltHeld;
		this->name = name;
		this->description = description;
		this->action = *action;
	}

	void CheckKeyboard(olc::PixelGameEngine* p) {
		if (inputState == 1 && p->GetKey(key).bHeld) {
			action(p);
		}
		else if (inputState == 2 && p->GetKey(key).bReleased) {
			action(p);
		}
		else if (p->GetKey(key).bPressed) {
			action(p);
		}
	}

	void CheckMouse(olc::PixelGameEngine* p) {
		if (inputState == 1 && p->GetMouse(mouseButton).bHeld) {
			action(p);
		}
		else if (inputState == 2 && p->GetMouse(mouseButton).bReleased) {
			action(p);
		}
		else if (p->GetMouse(mouseButton).bPressed) {
			action(p);
		}
	}

	void Update(olc::PixelGameEngine* p) {
		if (key != olc::NONE) {
			if (bShiftHeld) {
				if (p->GetKey(olc::SHIFT).bHeld) {
					CheckKeyboard(p);
				}
			}
			else if (bCtrlHeld) {
				if (p->GetKey(olc::CTRL).bHeld) {
					CheckKeyboard(p);
				}
				//} else if (bAltHeld) {
				//	if (p->GetKey(olc::ALT).bHeld) {
				//		CheckKeyboard(p);
				//	}
			}
			else if (!(p->GetKey(olc::SHIFT).bHeld || p->GetKey(olc::CTRL).bHeld)) {
				CheckKeyboard(p);
			}
		}
		else if (mouseButton != -1) {
			if (bShiftHeld) {
				if (p->GetKey(olc::SHIFT).bHeld) {
					CheckMouse(p);
				}
			}
			else if (bCtrlHeld) {
				if (p->GetKey(olc::CTRL).bHeld) {
					CheckMouse(p);
				}
				//} else if (bAltHeld) {
				//	if (p->GetKey(olc::ALT).bHeld) {
				//		CheckMouse(p);
				//	}
			}
			else if (!(p->GetKey(olc::SHIFT).bHeld || p->GetKey(olc::CTRL).bHeld)) {
				CheckMouse(p);
			}
		}
	};
};

namespace Input {
	global_variable std::vector<InputAction> inputActions;

	internal Entity* selectedEntity;
	internal Triangle* selectedTriangle;
	internal Vector3 leftClickPos = V3NULL;
	internal bool DEBUG_INPUT = true;

	Timer* timer;

#define DEBUGI DEBUG if(DEBUG_INPUT)

	internal Vector3 GetMousePos(olc::PixelGameEngine* p) {
		return Vector3(p->GetMouseX(), p->GetMouseY());
	}

	internal Vector3 GetMousePosNormalized(olc::PixelGameEngine* p) {
		return Vector3((float)p->GetMouseX() / (float)p->ScreenWidth(), (float)p->GetMouseY() / (float)p->ScreenHeight());
	}

	internal Camera* c;

	static void Init() {
		c = &Render::camera;

		timer = new Timer;

		inputActions = std::vector<InputAction>();
		//NOTE InputAction: function, name, key, mouseButton, inputState, shift, ctrl, description

	//// time control ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Time::deltaTime = 0;
			//TODO(i, sushi) set up pausing to also pause moving/rotating objects manually
			if (DEBUG_INPUT) std::cout << "Pausing the engine" << std::endl;
			}, "pause_game_held", olc::P, -1, 1, 0, 0,
			"Pauses the game while button is held."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::paused = !Render::paused;
			Physics::paused = !Physics::paused;
			DEBUGI{
				std::string output = (Render::paused) ? "true" : "false";
				std::cout << "Toggling paused to " + output << std::endl;
			}
			}, "pause_game", olc::SPACE, -1, 0, 0, 0,
			"Pauses the game on press."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::frame = !Render::frame;
			Physics::frame = !Physics::frame;
			DEBUGI std::cout << "Advancing one frame forward" << std::endl;
			}, "next_frame", olc::F, -1, 0, 0, 0,
			"Advances to the next frame if paused."));

		//// object spawning ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = GetMousePos(p);
			Sphere* sphere = new Sphere(10, 0, pos);
			sphere->mass = 10;
			sphere->mesh = new CircleMesh(10);
			Physics::AddEntity(sphere);
			Render::AddEntity(sphere);
			DEBUGI std::cout << "Creating Sphere at: " + pos.str() << std::endl;
			}, "spawn_sphere", olc::N, -1, 0, 0, 0,
			"Spawns a sphere of radius/mass 10 at the mouse."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = GetMousePos(p);
			Sphere* sphere = new Sphere(100, 0, pos);
			sphere->mass = 100;
			sphere->mesh = new CircleMesh(100);
			Physics::AddEntity(sphere);
			Render::AddEntity(sphere);
			DEBUGI std::cout << "Creating Large Sphere at: " + pos.str() << std::endl;
			}, "spawn_sphere_large", olc::N, -1, 0, 1, 0,
			"Spawns a large sphere of radius/mass 100 at the mouse."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Complex* complex = new Complex("objects/24k_Triangles.obj", 0, Vector3(0, 0, 3));
			selectedEntity = complex;
			Physics::AddEntity(complex);
			Render::AddEntity(complex);
			DEBUGI std::cout << "Creating " + complex->model_name + " at: " + V3ZERO.str() << std::endl;
			}, "spawn_complex", olc::T, -1, 0, 0, 0,
			"Spawns a complex object at (0,0,3)"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = Vector3(0, 0, 3);
			Box* box = new Box(Vector3(1, 1, 1), 0, pos);
			selectedEntity = box;
			Physics::AddEntity(box);
			Render::AddEntity(box);
			DEBUGI std::cout << "Creating Box at: " + pos.str() << std::endl;
			}, "spawn_box", olc::B, -1, 0, 0, 0,
			"Spawns a box at (0,0,3)"));

		//// object movement ////

		//inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
		//	for (auto& e : Render::entities) {
		//		e->rotation.x += 30 * Time::deltaTime;
		//	}
		//	DEBUGI std::cout << "Rotating everything in the positive x" << std::endl;
		//	}, "rotate_+x", olc::J, -1, 1, 0, 0,
		//	"Rotates all objects around the global x-axis in the positive direction"));
		//
		//inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
		//	for (auto& e : Render::entities) {
		//		e->rotation.y += 30 * Time::deltaTime;
		//	}
		//	DEBUGI std::cout << "Rotating everything in the positive y" << std::endl;
		//	}, "rotate_+y", olc::K, -1, 1, 0, 0,
		//	"Rotates all objects around the global y-axis in the positive direction"));
		//
		//inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
		//	for (auto& e : Render::entities) {
		//		e->rotation.z += 30 * Time::deltaTime;
		//	}
		//	DEBUGI std::cout << "Rotating everything in the positive z" << std::endl;
		//	}, "rotate_+z", olc::L, -1, 1, 0, 0,
		//	"Rotates all objects around the global z-axis in the positive direction"));
		//
		//inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
		//	for (auto& e : Render::entities) {
		//		e->rotation.x -= 30 * Time::deltaTime;
		//	}
		//	DEBUGI std::cout << "Rotating everything in the negative x" << std::endl;
		//	}, "rotate_-x", olc::M, -1, 1, 0, 0,
		//	"Rotates all objects around the global x-axis in the negative direction"));
		//
		//inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
		//	for (auto& e : Render::entities) {
		//		e->rotation.y -= 30 * Time::deltaTime;
		//	}
		//	DEBUGI std::cout << "Rotating everything in the negative y" << std::endl;
		//	}, "rotate_-y", olc::COMMA, -1, 1, 0, 0,
		//	"Rotates all objects around the global y-axis in the negative direction"));
		//
		//inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
		//	for (auto& e : Render::entities) {
		//		e->rotation.z -= 30 * Time::deltaTime;
		//	}
		//	DEBUGI std::cout << "Rotating everything in the negative z" << std::endl;
		//	}, "rotate_-z", olc::PERIOD, -1, 1, 0, 0,
		//	"Rotates all objects around the global z-axis in the negative direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->AddForce(nullptr, V3FORWARD);
				DEBUGI LOG("Adding force ", V3FORWARD, " to selected object.");
			}
			}, "phys_force_add_+z", olc::I, -1, 1, 0, 0,
			"Add force on selected object in z direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->AddForce(nullptr, V3BACKWARD);
				DEBUGI LOG("Adding force ", V3BACKWARD, " to selected object.");
			}
			}, "phys_force_add_-z", olc::K, -1, 1, 0, 0,
			"Add force on selected object in negative z direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->AddForce(nullptr, V3RIGHT);
				DEBUGI LOG("Adding force ", V3RIGHT, " to selected object.");
			}
			}, "phys_force_add_+x", olc::L, -1, 1, 0, 0,
			"Add force on selected object in x direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->AddForce(nullptr, V3LEFT);
				DEBUGI LOG("Adding force ", V3LEFT, " to selected object.");
			}
			}, "phys_force_add_-x", olc::J, -1, 1, 0, 0,
			"Add force on selected object in negative x direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->AddForce(nullptr, V3UP);
				DEBUGI LOG("Adding force ", V3UP, " to selected object.");
			}
			}, "phys_force_add_+y", olc::O, -1, 1, 0, 0,
			"Add force on selected object in y direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			if (PhysEntity* ptr = dynamic_cast<PhysEntity*>(selectedEntity)) {
				ptr->AddForce(nullptr, V3DOWN);
				DEBUGI LOG("Adding force ", V3DOWN, " to selected object.");
			}
			}, "phys_force_add_-y", olc::U, -1, 1, 0, 0,
			"Add force on selected object in negative y direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::entities.clear();
			Render::sentities.clear();
			}, "clear_entities", olc::C, -1, 1, 1, 0,
			"Delete all entities"));

		//// object selection ////

			//TODO(o, sushi) write this to skip objects who aren't close to the line
		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = GetMousePos(p);
			if (selectedEntity) { selectedEntity->ENTITY_DEBUG = false; selectedEntity = nullptr; }

			pos.ScreenToWorld(c->ProjectionMatrix(p), c->MakeViewMatrix(Render::yaw), p);
			pos.WorldToLocal(c->position);
			pos.normalize();
			pos *= 1000;
			pos.LocalToWorld(c->position);

			//make function somehere that returns a ray cast for ease of use later maybe
			Line3* ray = new Line3(pos, -1, c->position);

			//draw ray if debugging
			DEBUGI Render::entities.push_back(ray);

			for (Entity* e : Render::entities) {
				if (e->LineIntersect(&ray->edge) && e->id != -1) {
					selectedEntity = e;
					selectedEntity->ENTITY_DEBUG = true;
					break;
				}
			}

			if (selectedEntity == nullptr) { ERROR("No object selected"); }
			}, "select_entity", olc::NONE, 0, 0, 0, 0,
			"Selects an entity"));

		//// camera movement ////

			//TODO(i,,) change camera movement to W-forward, S-backward, Q-down, E-up
		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position.y -= 8 * Time::deltaTime;
			DEBUGI std::cout << "Translating the camera in the positive y" << std::endl;
			}, "camera_translate_+y", olc::E, -1, 1, 0, 0,
			"Translates the camera along the positive global y-axis"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position.y += 8 * Time::deltaTime;
			DEBUGI std::cout << "Translating the camera in the negative y" << std::endl;
			}, "camera_translate_-y", olc::Q, -1, 1, 0, 0,
			"Translates the camera along the negative global y-axis"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position += c->lookDir.cross(c->up) * 8 * Time::deltaTime;
			DEBUGI std::cout << "Translating the camera in the negative x" << std::endl;
			}, "camera_translate_left", olc::A, -1, 1, 0, 0,
			"Translates the camera left"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position -= c->lookDir.cross(c->up) * 8 * Time::deltaTime;
			DEBUGI std::cout << "Translating the camera in the positive x" << std::endl;
			}, "camera_translate_right", olc::D, -1, 1, 0, 0,
			"Translates the camera right"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position += c->lookDir * 8 * Time::deltaTime;
			DEBUGI std::cout << "Translating the camera forward" << std::endl;
			}, "camera_translate_forward", olc::W, -1, 1, 0, 0,
			"Translates the camera forwards"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position -= c->lookDir * 8 * Time::deltaTime;
			DEBUGI std::cout << "Translating the camera backward" << std::endl;
			}, "camera_translate_backward", olc::S, -1, 1, 0, 0,
			"Translates the camera backwards"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::yaw -= 50 * Time::deltaTime;
			DEBUGI std::cout << "Turning the camera right" << std::endl;
			}, "camera_turn_right", olc::RIGHT, -1, 1, 0, 0,
			"Rotates the camera along its local y-axis (yaw) in the positive direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::yaw += 50 * Time::deltaTime;
			DEBUGI std::cout << "Turning the camera left" << std::endl;
			}, "camera_turn_left", olc::LEFT, -1, 1, 0, 0,
			"Rotates the camera along its local y-axis (yaw) in the negative direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			c->position = V3ZERO;
			Render::yaw = 0;
			DEBUGI std::cout << "Resetting camera to pos: (0,0,0) and yaw: 0" << std::endl;
			}, "camera_reset", olc::Z, -1, 0, 0, 0,
			"Resets the camera to position: (0,0,0) and yaw: 0"));

		//// render options ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::WIRE_FRAME = !Render::WIRE_FRAME;
			std::string output = (Render::WIRE_FRAME) ? "true" : "false";
			DEBUGI std::cout << "Toggling wireframe to: " + output << std::endl;
			}, "TOGGLE_WIRE_FRAME", olc::C, -1, 0, 0, 0,
			"Toggles whether the wireframe of objects should be rendered"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::DISP_EDGES = !Render::DISP_EDGES;
			std::string output = (Render::DISP_EDGES) ? "true" : "false";
			DEBUGI std::cout << "Toggling display edge numbers to: " + output << std::endl;
			}, "TOGGLE_DISP_EDGES", olc::V, -1, 0, 0, 0,
			"Toggles whether the edges of objects are numbered"));

		//// input management ////
			//loop through all the input actions and unbind duplicate binds
		for (InputAction& action : inputActions) {
			for (InputAction& action2 : inputActions) {
				if (&action != &action2 && action.key != olc::NONE) {
					if (action.key == action2.key && action.bCtrlHeld == action2.bCtrlHeld && action.bShiftHeld == action2.bShiftHeld) {
						ERROR("Actions " + action.name + " and " + action2.name + " were both bound to " + std::to_string(action.key) + "\n\tBoth have been unbound because of this");
						action.key = olc::NONE; action2.key = olc::NONE;
					}
				}
			}
		}
	}

	static void Update(olc::PixelGameEngine* p, float& deltaTimePtr) {
		for (InputAction action : inputActions) {
			action.Update(p);
		}

		////    Mouse Input    /////

		//LMB press = set click position
		if (p->GetMouse(0).bPressed) {
			leftClickPos = GetMousePos(p);
		}

		//LMB hold = draw line between click and mouse position
		if (p->GetMouse(0).bHeld) {
			p->DrawLine(leftClickPos.x, leftClickPos.y, p->GetMouseX(), p->GetMouseY(), olc::WHITE);
		}

		//LMB release = add  drawn force to selected entity
		if (p->GetMouse(0).bReleased) {
			if (PhysEntity* entity = dynamic_cast<PhysEntity*>(selectedEntity)) {
				entity->AddForce(nullptr, (GetMousePos(p) - leftClickPos) * 100, true);
			}

			leftClickPos = V3NULL;
		}

		//RMB hold = set the position of selected entity to mouse
		if (selectedEntity && p->GetMouse(1).bHeld) {
			//do nothing for now

			//Vector3 pos = GetMousePos(p);
			//pos.ScreenToWorld(c->ProjectionMatrix(p), c->MakeViewMatrix(Render::yaw), p);
			//pos.WorldToLocal(c->position);
			//pos.normalize();
			//pos *= 1000;
			//pos.LocalToWorld(c->position);
			//
			//selectedEntity->position.x = pos.x;
			//selectedEntity->position.y = pos.y;
		}

		if (selectedEntity) {
			DEBUGI p->DrawStringDecal(Vector2(0, 0), selectedEntity->str());
		}
	}

	//NOTE: selected entity should never point to a NEW object, since Input shouldnt own that object
	static void Cleanup() {
		//delete selectedEntity;
		selectedEntity = nullptr;
	}
};