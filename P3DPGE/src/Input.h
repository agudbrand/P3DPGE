#pragma once
#include "Render.h"
#include "Physics.h"
#include "olcPixelGameEngine.h"

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
	internal bool debugInput = false;
	
	internal Vector3 GetMousePos(olc::PixelGameEngine* p) {
		return Vector3(p->GetMouseX(), p->GetMouseY(), 0);
	}
	
	static void Init() {
		inputActions = std::vector<InputAction>();
		//NOTE InputAction: function, name, key, mouseButton, inputState, shift, ctrl, description
	
		//// time control ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Time::deltaTime = 0;
			//TODO(i, sushi) set up pausing to also pause moving/rotating objects manually
			if(debugInput) std::cout << "Pausing the engine" << std::endl;
		}, "pause_game_held", olc::P, -1, 1, 0, 0,
		"Pauses the game while button is held."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::paused = !Render::paused;
			Physics::paused = !Physics::paused;
			if (debugInput) {
				std::string output = (Render::paused) ? "true" : "false";
				std::cout << "Toggling paused to " + output << std::endl;
			}
		}, "pause_game", olc::SPACE, -1, 0, 0, 0,
		"Pauses the game on press."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::frame = !Render::frame;
			Physics::frame = !Physics::frame;
			if (debugInput) std::cout << "Advancing one frame forward" << std::endl;
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
			if (debugInput) std::cout << "Creating Sphere at: " + pos.str() << std::endl;
		}, "spawn_sphere", olc::Q, -1, 0, 0, 0,
		"Spawns a sphere of radius/mass 10 at the mouse."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = GetMousePos(p);
			Sphere* sphere = new Sphere(100, 0, pos);
			sphere->mass = 100;
			sphere->mesh = new CircleMesh(100);
			Physics::AddEntity(sphere);
			Render::AddEntity(sphere);
			if (debugInput) std::cout << "Creating Large Sphere at: " + pos.str() << std::endl;
		}, "spawn_sphere_large", olc::Q, -1, 0, 1, 0,
		"Spawns a large sphere of radius/mass 100 at the mouse."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Complex* complex = new Complex("objects/24k_Triangles.obj", 0, V3ZERO);
			selectedEntity = complex;
			Physics::AddEntity(complex);
			Render::AddEntity(complex);
			if (debugInput) std::cout << "Creating " + complex->model_name + " at: " + V3ZERO.str() << std::endl;
		}, "spawn_complex", olc::T, -1, 0, 0, 0,
		"Spawns a complex object at (0,0,0)"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = Vector3(0, 0, 3);
			Box* box = new Box(Vector3(1, 1, 1), 0, pos);
			selectedEntity = box;
			Physics::AddEntity(box);
			Render::AddEntity(box);
			if (debugInput) std::cout << "Creating Box at: " + pos.str() << std::endl;
		}, "spawn_box", olc::E, -1, 0, 0, 0,
		"Spawns a box at (0,0,3)"));

	//// object movement ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			for (auto& e : Render::entities) {
				e->rotation.x = 40 * Time::deltaTime;
				e->RotateX();
			}
			if (debugInput) std::cout << "Rotating everything in the positive x" << std::endl;
			}, "rotate_+x", olc::J, -1, 1, 0, 0,
		"Rotates all objects around the global x-axis in the positive direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			for (auto& e : Render::entities) {
				e->rotation.y = 40 * Time::deltaTime;
				e->RotateY();
			}
			if (debugInput) std::cout << "Rotating everything in the positive y" << std::endl;
			}, "rotate_+y", olc::K, -1, 1, 0, 0,
		"Rotates all objects around the global y-axis in the positive direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			for (auto& e : Render::entities) {
				e->rotation.z = 40 * Time::deltaTime;
				e->RotateZ();
			}
			if (debugInput) std::cout << "Rotating everything in the positive z" << std::endl;
			}, "rotate_+z", olc::L, -1, 1, 0, 0,
		"Rotates all objects around the global z-axis in the positive direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			for (auto& e : Render::entities) {
				e->rotation.x = -40 * Time::deltaTime;
				e->RotateX();
			}
			if (debugInput) std::cout << "Rotating everything in the negative x" << std::endl;
			}, "rotate_-x", olc::M, -1, 1, 0, 0,
		"Rotates all objects around the global x-axis in the negative direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			for (auto& e : Render::entities) {
				e->rotation.y = -40 * Time::deltaTime;
				e->RotateY();
			}
			if (debugInput) std::cout << "Rotating everything in the negative y" << std::endl;
			}, "rotate_-y", olc::COMMA, -1, 1, 0, 0,
		"Rotates all objects around the global y-axis in the negative direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			for (auto& e : Render::entities) {
				e->rotation.z = -40 * Time::deltaTime;
				e->RotateZ();
			}
			if (debugInput) std::cout << "Rotating everything in the negative z" << std::endl;
			}, "rotate_-z", olc::PERIOD, -1, 1, 0, 0,
		"Rotates all objects around the global z-axis in the negative direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			for (auto& e : Render::entities) {
				e->Translate(Vector3(0, 0, 10 * Time::deltaTime));
			}
			if (debugInput) std::cout << "Translate everything in the positive z" << std::endl;
			}, "translate_+z", olc::U, -1, 1, 0, 0,
		"Translates all objects along the positive global z-axis"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			for (auto& e : Render::entities) {
				e->Translate(Vector3(0, 0, -10 * Time::deltaTime));
			}
			if (debugInput) std::cout << "Translate everything in the negative z" << std::endl;
			}, "translate_-z", olc::I, -1, 1, 0, 0,
		"Translates all objects along the negative global z-axis"));

	//// object selection ////
		
		//TODO(o, sushi) write this to skip objects who aren't close to the line
		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = GetMousePos(p);
			pos.unProjToScreen(Render::ProjectionMatrix(p), p);
			Vector3 unview = Math::M1x4ToVector3(Math::Vector3ToM1x4(pos) * inverse(Render::view));
			
			if (selectedEntity) { selectedEntity = nullptr; }

			
			Vector3 ctox = unview - Render::camera.position;

			Line3* ray = new Line3(ctox * 10, -1, Render::camera.position);
			//Box* b = new Box(Vector3(0.3, 0.3, 0.3), -1, ctox * 10);
			Box* b2 = new Box(Vector3(0.3, 0.3, 0.3), -1, ctox * 10);

			Debug::Message(pos.str());
			Debug::Message(unview.str());

			for (Entity* e : Render::entities) {
				if (e->LineIntersect(&ray->edge) && e->id != -1) {
					selectedEntity = e;
					break;
				}
			}

			Render::sentities.push_back(ray);
			Render::sentities.push_back(b2);

			if (selectedEntity == nullptr) { Debug::Error("No object selected"); }

			}, "select_entity", olc::NONE, 0, 0, 0, 0,
			"Selects an entity"));

	//// camera movement ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::camera.position.y -= 8 * Time::deltaTime;
			if (debugInput) std::cout << "Translating the camera in the positive y" << std::endl;
			}, "camera_translate_+y", olc::W, -1, 1, 0, 0,
		"Translates the camera along the positive global y-axis"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::camera.position.y += 8 * Time::deltaTime;
			if (debugInput) std::cout << "Translating the camera in the negative y" << std::endl;
			}, "camera_translate_-y", olc::S, -1, 1, 0, 0,
		"Translates the camera along the negative global y-axis"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::camera.position.x -= 8 * Time::deltaTime;
			if (debugInput) std::cout << "Translating the camera in the negative x" << std::endl;
			}, "camera_translate_-x", olc::A, -1, 1, 0, 0,
		"Translates the camera along the negative global x-axis"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::camera.position.x += 8 * Time::deltaTime;
			if (debugInput) std::cout << "Translating the camera in the positive x" << std::endl;
			}, "camera_translate_+x", olc::D, -1, 1, 0, 0,
		"Translates the camera along the positive global y-axis"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::camera.position += Render::camera.lookDir * 8 * Time::deltaTime;
			if (debugInput) std::cout << "Translating the camera forward" << std::endl;
			}, "camera_translate_forward", olc::UP, -1, 1, 0, 0,
		"Translates the camera along the positive local z-axis"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::camera.position -= Render::camera.lookDir * 8 * Time::deltaTime;
			if (debugInput) std::cout << "Translating the camera backward" << std::endl;
			}, "camera_translate_backward", olc::DOWN, -1, 1, 0, 0,
		"Translates the camera along the negative local z-axis"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::yaw -= 50 * Time::deltaTime;
			if (debugInput) std::cout << "Turning the camera right" << std::endl;
			}, "camera_turn_right", olc::RIGHT, -1, 1, 0, 0,
		"Rotates the camera along its local y-axis (yaw) in the positive direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::yaw += 50 * Time::deltaTime;
			if (debugInput) std::cout << "Turning the camera left" << std::endl;
			}, "camera_turn_left", olc::LEFT, -1, 1, 0, 0,
		"Rotates the camera along its local y-axis (yaw) in the negative direction"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::camera.position = V3ZERO; 
			Render::yaw = 0;
			if (debugInput) std::cout << "Resetting camera to pos: (0,0,0) and yaw: 0" << std::endl;
			}, "camera_reset", olc::Z, -1, 0, 0, 0,
		"Resets the camera to position: (0,0,0) and yaw: 0"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {

			Vector3 pos1 = V3ZERO; pos1.ScreenToWorld(Render::ProjectionMatrix(p), Render::view, p);
			Vector3 pos2 = Vector3(0, p->ScreenHeight()); pos2.ScreenToWorld(Render::ProjectionMatrix(p), Render::view, p);
			Vector3 pos3 = Vector3(p->ScreenWidth(), 0); pos3.ScreenToWorld(Render::ProjectionMatrix(p), Render::view, p);
			Vector3 pos4 = Vector3(p->ScreenWidth(), p->ScreenHeight()); pos4.ScreenToWorld(Render::ProjectionMatrix(p), Render::view, p);

			Vector3 ctox1 = (pos1 - Render::camera.position).normalized();
			Vector3 ctox2 = (pos2 - Render::camera.position).normalized();
			Vector3 ctox3 = (pos3 - Render::camera.position).normalized();
			Vector3 ctox4 = (pos4 - Render::camera.position).normalized();

			Line3* r1 = new Line3(ctox1, -1, Render::camera.position);
			Line3* r2 = new Line3(ctox2, -1, Render::camera.position);
			Line3* r3 = new Line3(ctox3, -1, Render::camera.position);
			Line3* r4 = new Line3(ctox4, -1, Render::camera.position);
			Line3* r5 = new Line3(ctox1, -1, ctox3);
			Line3* r6 = new Line3(ctox2, -1, ctox1);
			Line3* r7 = new Line3(ctox3, -1, ctox4);
			Line3* r8 = new Line3(ctox4, -1, ctox2);

			Render::sentities.push_back(r1);
			Render::sentities.push_back(r2);
			Render::sentities.push_back(r3);
			Render::sentities.push_back(r4);
			Render::sentities.push_back(r5);
			Render::sentities.push_back(r6);
			Render::sentities.push_back(r7);
			Render::sentities.push_back(r8);

			}, "display_proj_fulcrum", olc::B, -1, 0, 0, 0,
			"Display camera's perspective fulcrum"));

	//// render options ////

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::WIRE_FRAME = !Render::WIRE_FRAME;
			std::string output = (Render::WIRE_FRAME) ? "true" : "false";
			if (debugInput) std::cout << "Toggling wireframe to: " + output << std::endl;
			}, "TOGGLE_WIRE_FRAME", olc::C, -1, 0, 0, 0,
		"Toggles whether the wireframe of objects should be rendered"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::DISP_EDGES = !Render::DISP_EDGES;
			std::string output = (Render::DISP_EDGES) ? "true" : "false";
			if (debugInput) std::cout << "Toggling wireframe to: " + output << std::endl;
			}, "TOGGLE_DISP_EDGES", olc::V, -1, 0, 0, 0,
		"Toggles whether the edges of objects are numbered"));

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
		
		//RMB press = select entity
		//TODO(si,delle,11/17/20) change this so it checks objects in screen space
		if (p->GetMouse(1).bPressed) {
			//reset selected
			if (selectedEntity) {
				selectedEntity->SetColor(olc::WHITE);
			}
			selectedEntity = nullptr;
			
			//check if mouse click contains an entity
			Vector3 mousePos = GetMousePos(p);
			for (auto& entity : Physics::physEntities) {
				if (entity->ContainsScreenPoint(mousePos)) {
					selectedEntity = entity;
					break;
				}
			}
			
			//set selected to red
			if (selectedEntity) {
				selectedEntity->SetColor(olc::RED);
			}
		}
		
		//RMB hold = set the position of selected entity to mouse
		if (selectedEntity && p->GetMouse(1).bHeld) {
			selectedEntity->position = Math::vi2dToVector3(p->GetMousePos(), selectedEntity->position.z);
		}
		
		if (p->GetKey(olc::F).bPressed) { selectedEntity = Render::entities[0]; }

		//if (selectedEntity) {
		//	std::string text = "ID: " + std::to_string(selectedEntity->id) + "\tTag: " + selectedEntity->tag + "\n";
		//	text += "Position:" + selectedEntity->position.str() + "\nRotation:" + selectedEntity->rotation.str() + "\nScale:" + selectedEntity->scale.str() + "\n";
		//	//NOTE this checks if possible and casts Entity* to PhysEntity*
		//	if (PhysEntity* entity = dynamic_cast<PhysEntity*>(selectedEntity)) {
		//		text += "Is Static: "; text += entity->bStatic ? "true" : "false"; text += "\nMass: " + std::to_string(entity->mass) + "\n";
		//		text += "Velocity:" + entity->velocity.str() + " " + std::to_string(entity->velocity.mag()) + "\nAcceleration:" + entity->acceleration.str() + "\n";
		//		text += "rotVelocity:" + entity->rotVelocity.str() + "\nrotAcceleration:" + entity->rotAcceleration.str() + "\n";
		//	}
		//
		//	p->DrawStringDecal(olc::vf2d(0, 0), text);
		//}

		//point debugging
		/* TODO(i, sushi) figure out how to make this work with only drawn triangles
		if (selectedEntity) {
			int i = 0;
			for (auto& pa : selectedEntity->mesh->drawnTriangles) {
				p->DrawString(olc::vd2d(1, i * 9),
					"Triangle " + std::to_string(i) + ": " +
					Math::append_decimal(std::to_string(pa.proj_points[0].x)) + "x " + Math::append_decimal(std::to_string(pa.proj_points[0].y)) + "y " + Math::append_decimal(std::to_string(pa.proj_points[0].z)) + "z " +
					Math::append_decimal(std::to_string(pa.proj_points[1].x)) + "x " + Math::append_decimal(std::to_string(pa.proj_points[1].y)) + "y " + Math::append_decimal(std::to_string(pa.proj_points[1].z)) + "z " +
					Math::append_decimal(std::to_string(pa.proj_points[2].x)) + "x " + Math::append_decimal(std::to_string(pa.proj_points[2].y)) + "y " + Math::append_decimal(std::to_string(pa.proj_points[2].z)) + "z ");
					i++;
			}

			//p->DrawStringDecal(olc::vf2d(0, 0), text);
		}*/

		//point debugging
		//if (selectedEntity) {
		//	for (int i = 0; i < selectedEntity->mesh.triangles.size(); i++) {
		//		auto pa = selectedEntity->mesh.triangles;
		//		p->DrawString(olc::vd2d(1, i * 9),
		//			"Triangle: " + std::to_string(i) + " " +
		//			Math::append_decimal(std::to_string(pa[i].proj_points[0].x)) + "x " + Math::append_decimal(std::to_string(pa[i].proj_points[0].y)) + "y " + Math::append_decimal(std::to_string(pa[i].proj_points[0].z)) + "z " +
		//			Math::append_decimal(std::to_string(pa[i].proj_points[1].x)) + "x " + Math::append_decimal(std::to_string(pa[i].proj_points[1].y)) + "y " + Math::append_decimal(std::to_string(pa[i].proj_points[1].z)) + "z " +
		//			Math::append_decimal(std::to_string(pa[i].proj_points[2].x)) + "x " + Math::append_decimal(std::to_string(pa[i].proj_points[2].y)) + "y " + Math::append_decimal(std::to_string(pa[i].proj_points[2].z)) + "z "
		//		);
		//	}
		//}
	}
	
	//NOTE: selected entity should never point to a NEW object, since Input shouldnt own that object
	static void Cleanup() {
		//delete selectedEntity;
		selectedEntity = nullptr;
	}
};