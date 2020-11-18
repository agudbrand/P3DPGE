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
		} else if (inputState == 2 && p->GetKey(key).bReleased) {
			action(p);
		} else if (p->GetKey(key).bPressed) {
			action(p);
		}
	}
    
	void CheckMouse(olc::PixelGameEngine* p) {
		if (inputState == 1 && p->GetMouse(mouseButton).bHeld) {
			action(p);
		} else if (inputState == 2 && p->GetMouse(mouseButton).bReleased) {
			action(p);
		} else if (p->GetMouse(mouseButton).bPressed) {
			action(p);
		}
	}
    
	void Update(olc::PixelGameEngine* p) {
		if (key != olc::NONE) {
			if (bShiftHeld) {
				if (p->GetKey(olc::SHIFT).bHeld) {
					CheckKeyboard(p);
				}
			} else if (bCtrlHeld) {
				if (p->GetKey(olc::CTRL).bHeld) {
					CheckKeyboard(p);
				}
			//} else if (bAltHeld) {
			//	if (p->GetKey(olc::ALT).bHeld) {
			//		CheckKeyboard(p);
			//	}
			} else if(!(p->GetKey(olc::SHIFT).bHeld || p->GetKey(olc::CTRL).bHeld)){
				CheckKeyboard(p);
			}
		} else if (mouseButton != -1) {
			if (bShiftHeld) {
				if (p->GetKey(olc::SHIFT).bHeld) {
					CheckMouse(p);
				}
			} else if (bCtrlHeld) {
				if (p->GetKey(olc::CTRL).bHeld) {
					CheckMouse(p);
				}
			//} else if (bAltHeld) {
			//	if (p->GetKey(olc::ALT).bHeld) {
			//		CheckMouse(p);
			//	}
			} else if (!(p->GetKey(olc::SHIFT).bHeld || p->GetKey(olc::CTRL).bHeld)){
				CheckMouse(p);
			}
		}
	};
};

namespace Input {
	static std::vector<InputAction> inputActions;
    
	static Entity* selectedEntity;
	static Vector3 leftClickPos = V3NULL;
    
	static Vector3 GetMousePos(olc::PixelGameEngine* p) {
		return Vector3(p->GetMouseX(), p->GetMouseY(), 0);
	}
    
	static void Init() {
		inputActions = std::vector<InputAction>();
		//NOTE InputAction: function, name, key, mouseButton, inputState, shift, ctrl, description
        
		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			//Time::deltaTime = 0;
			//TODO(i, sushi) set up pausing to also pause moving/rotating objects manually
		}, "pause_game_held", olc::P, -1, 1, 0, 0, 
		"Pauses the game while button is held."));
        
        
		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::paused = !Render::paused;
			Physics::paused = !Physics::paused;
		}, "pause_game", olc::SPACE, -1, 0, 0, 0,
        "Pauses the game on press."));
        
        inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Render::frame = !Render::frame;
			Physics::frame = !Physics::frame;
		}, "next_frame", olc::F, -1, 0, 0, 0,
		"Advances to the next frame if paused."));
        

        inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = GetMousePos(p);
			Sphere* sphere = new Sphere(10, 0, pos);
			sphere->mass = 10;
			sphere->mesh = new CircleMesh(10);
			Physics::AddEntity(sphere);
			Render::AddEntity(sphere);
			std::cout << "Creating Sphere at: " + pos.str() << std::endl;
		}, "spawn_sphere", olc::Q, -1, 0, 0, 0,
        "Spawns a sphere of radius/mass 10 at the mouse."));
        
        inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = GetMousePos(p);
			Sphere* sphere = new Sphere(100, 0, pos);
			sphere->mass = 100;
			sphere->mesh = new CircleMesh(100);
			Physics::AddEntity(sphere);
			Render::AddEntity(sphere);
			std::cout << "Creating Large Sphere at: " + pos.str() << std::endl;
		}, "spawn_sphere_large", olc::Q, -1, 0, 1, 0,
		"Spawns a large sphere of radius/mass 100 at the mouse."));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Complex* complex = new Complex("objects/whale_ship.obj", 0, V3ZERO);
			selectedEntity = complex;
			Physics::AddEntity(complex);
			Render::AddEntity(complex);
			std::cout << "Creating " + complex->model_name + " at: " + V3ZERO.str() << std::endl;
		}, "spawn_complex", olc::T, -1, 0, 0, 0,
		"Spawns a complex object at (0,0,0)"));

		inputActions.push_back(InputAction([](olc::PixelGameEngine* p) {
			Vector3 pos = Vector3(0, 0, 3);
			Box* box = new Box(Vector3(1, 1, 1), 0, pos);
			selectedEntity = box;
			Physics::AddEntity(box);
			Render::AddEntity(box);
			std::cout << "Creating Box at: " + pos.str() << std::endl;
		}, "spawn_complex", olc::E, -1, 0, 0, 0,
		"Spawns a box at (0,0,3)"));
	}
    
	static void Update(olc::PixelGameEngine* p, float& deltaTimePtr) {
		for (InputAction action : inputActions) {
			action.Update(p);
		}
        
		////    Keyboard Input    /////
		//G press = pause
		if (p->GetKey(olc::G).bHeld) {
			deltaTimePtr = 0;
		}

		//rotation over axes
		if (p->GetKey(olc::J).bHeld) {
			for (auto& e : Render::entities) {
				e->rotation.x = 40 * deltaTimePtr;
				e->RotateX();
			}
		}
        
		//K held = rotate everything in the positive y
		if (p->GetKey(olc::K).bHeld) {
			for (auto& e : Render::entities) {
				e->rotation.y = 40 * deltaTimePtr;
				e->RotateY();
			}
		}
        
		//L held = rotate everything in the positive z
		if (p->GetKey(olc::L).bHeld) {
			for (auto& e : Render::entities) {
				e->rotation.z = 40 * deltaTimePtr;
				e->RotateZ();
			}
		}
        
		//M held = rotate everything in the negative x
		if (p->GetKey(olc::M).bHeld) {
			for (auto& e : Render::entities) {
				e->rotation.x = -40 * deltaTimePtr;
				e->RotateX();
			}
		}
        
		//COMMA held = rotate everything in the negative y
		if (p->GetKey(olc::COMMA).bHeld) {
			for (auto& e : Render::entities) {
				e->rotation.y = -40 * deltaTimePtr;
				e->RotateY();
			}
		}
        
		//PERIOD held = rotate everything in the negative z
		if (p->GetKey(olc::PERIOD).bHeld) {
			for (auto& e : Render::entities) {
				e->rotation.z = -40 * deltaTimePtr;
				e->RotateZ();
			}
		}
        
		//SHIFT held =
		if (p->GetKey(olc::SHIFT).bHeld) {
			for (auto& e : Render::entities) {
				e->Translate(Vector3(0, 0, 10 * deltaTimePtr));
			}
		}
        
		//CTRL held =
		if (p->GetKey(olc::CTRL).bHeld) {
			for (auto& e : Render::entities) {
				e->Translate(Vector3(0, 0, -10 * deltaTimePtr));
			}
		}
        
		//Camera movement
        
		//translation
		if (p->GetKey(olc::W).bHeld)  { Render::camera.position.y -= 8 * deltaTimePtr; }
		if (p->GetKey(olc::S).bHeld)  { Render::camera.position.y += 8 * deltaTimePtr; }
		if (p->GetKey(olc::A).bHeld)  { Render::camera.position.x -= 8 * deltaTimePtr; }
		if (p->GetKey(olc::D).bHeld)  { Render::camera.position.x += 8 * deltaTimePtr; }
		if (p->GetKey(olc::UP).bHeld) {
			Vector3 forward = Render::camera.lookDir * 8 * deltaTimePtr;
			Render::camera.position += forward;
		}
		if (p->GetKey(olc::DOWN).bHeld) {
			Vector3 forward = Render::camera.lookDir * 8 * deltaTimePtr;
			Render::camera.position -= forward;
		}
        
		//rotation
		if (p->GetKey(olc::RIGHT).bHeld) { Render::yaw -= 50 * deltaTimePtr; }
		if (p->GetKey(olc::LEFT).bHeld) { Render::yaw += 50 * deltaTimePtr; }
        
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
				entity->AddForce(nullptr, (GetMousePos(p) - leftClickPos)*100, true);
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
				if (entity->ContainsPoint(mousePos)) {
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
		if (selectedEntity) {
			for (int i = 0; i < selectedEntity->mesh->drawnTriangles_Debug.size(); i++) {
				auto pa = selectedEntity->mesh->drawnTriangles_Debug;
				p->DrawString(olc::vd2d(1, i * 9),
					"Triangle: " + std::to_string(i) + " " +
					Math::append_decimal(std::to_string(pa[i].projectedPoints[0].x)) + "x " + Math::append_decimal(std::to_string(pa[i].projectedPoints[0].y)) + "y " + Math::append_decimal(std::to_string(pa[i].projectedPoints[0].z)) + "z " +
					Math::append_decimal(std::to_string(pa[i].projectedPoints[1].x)) + "x " + Math::append_decimal(std::to_string(pa[i].projectedPoints[1].y)) + "y " + Math::append_decimal(std::to_string(pa[i].projectedPoints[1].z)) + "z " +
					Math::append_decimal(std::to_string(pa[i].projectedPoints[2].x)) + "x " + Math::append_decimal(std::to_string(pa[i].projectedPoints[2].y)) + "y " + Math::append_decimal(std::to_string(pa[i].projectedPoints[2].z)) + "z "
				);
			}
            
			//p->DrawStringDecal(olc::vf2d(0, 0), text);
		}
        
		//point debugging
		//if (selectedEntity) {
		//	for (int i = 0; i < selectedEntity->mesh.triangles.size(); i++) {
		//		auto pa = selectedEntity->mesh.triangles;
		//		p->DrawString(olc::vd2d(1, i * 9),
		//			"Triangle: " + std::to_string(i) + " " +
		//			Math::append_decimal(std::to_string(pa[i].projectedPoints[0].x)) + "x " + Math::append_decimal(std::to_string(pa[i].projectedPoints[0].y)) + "y " + Math::append_decimal(std::to_string(pa[i].projectedPoints[0].z)) + "z " +
		//			Math::append_decimal(std::to_string(pa[i].projectedPoints[1].x)) + "x " + Math::append_decimal(std::to_string(pa[i].projectedPoints[1].y)) + "y " + Math::append_decimal(std::to_string(pa[i].projectedPoints[1].z)) + "z " +
		//			Math::append_decimal(std::to_string(pa[i].projectedPoints[2].x)) + "x " + Math::append_decimal(std::to_string(pa[i].projectedPoints[2].y)) + "y " + Math::append_decimal(std::to_string(pa[i].projectedPoints[2].z)) + "z "
		//		);
		//	}
		//}
        

		if (p->GetKey(olc::C).bPressed) { Render::wireframe = !Render::wireframe; }
        
	}
    
	//NOTE: selected entity should never point to a NEW object, since Input shouldnt own that object
	static void Cleanup() {
		//delete selectedEntity;
		selectedEntity = nullptr;
	}
};