#pragma once
#include "Physics.h"
#include "Render.h"

namespace Input {
	static Entity* selectedEntity;
	static Vector3 leftClickPos = V3NULL;

	static Vector3 GetMousePos(olc::PixelGameEngine* p) {
		return Vector3(p->GetMouseX(), p->GetMouseY(), 0);
	}

	static void Update(olc::PixelGameEngine* p, float& deltaTimePtr) {
		//G press = pause
		if (p->GetKey(olc::G).bHeld) {
			deltaTimePtr = 0;
		}

		//Q press = spawn sphere
		if (p->GetKey(olc::Q).bPressed) {
			Vector3 pos = Vector3(p->GetMouseX(), p->GetMouseY(), 0);
			Sphere* sphere = new Sphere(10, -1, pos);
			Physics::AddEntity(sphere, true);
			Render::AddEntity(sphere);
			std::cout << "Creating Sphere at: " + pos.str() << std::endl;
		}

		//E press = spawn box
		if (p->GetKey(olc::E).bPressed) {
			Vector3 pos = GetMousePos(p);
			pos.z += pos.x + 10; //for z testing
			Box* box = new Box(Vector3(100, 130, 100), -1, pos);
			Physics::AddEntity(box, true);
			Render::AddEntity(box);
			std::cout << "Creating Box at: " + pos.str() << std::endl;
		}

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
				entity->AddForce(nullptr, GetMousePos(p)-leftClickPos, true);
			}

			leftClickPos = V3NULL;
		}

		//rotation over axes
		//TODO: make these work with selected entity 
		//I think the new set up for boxes
		//doesn't work with the current Box::ContainsPoint()
		if (p->GetKey(olc::K).bPressed) { 
			for (auto& e : Render::entities) {
				e->rotation.x = 0.15;
				e->RotateX();
			}
		}

		if (p->GetKey(olc::K).bReleased) {
			for (auto& e : Render::entities) {
				e->rotation.x = 0;
			}
		}

		if (p->GetKey(olc::L).bPressed) {
			for (auto& e : Render::entities) {
				e->rotation.z = 0.15;
				e->RotateZ();
			}
		}

		if (p->GetKey(olc::L).bReleased) {
			for (auto& e : Render::entities) {
				e->rotation.z = 0;
			}
		}

		if (p->GetKey(olc::J).bPressed) {
			for (auto& e : Render::entities) {
				e->rotation.y = 0.15;
				e->RotateY();
			}
		}

		if (p->GetKey(olc::J).bReleased) {
			for (auto& e : Render::entities) {
				e->rotation.y = 0;
			}
		}

		if (p->GetKey(olc::RIGHT).bHeld) {
			for (auto& e : Render::entities) {
				e->Translate(Vector3(70 * deltaTimePtr, 0, 0));
			}
		}
		if (p->GetKey(olc::LEFT).bHeld) {
			for (auto& e : Render::entities) {
				e->Translate(Vector3(-70 * deltaTimePtr, 0, 0));
			}
		}
		if (p->GetKey(olc::UP).bHeld) {
			for (auto& e : Render::entities) {
				e->Translate(Vector3(0, -70 * deltaTimePtr, 0));
			}
		}
		if (p->GetKey(olc::DOWN).bHeld) {
			for (auto& e : Render::entities) {
				e->Translate(Vector3(0, 70 * deltaTimePtr, 0));
			}
		}


		if (p->GetKey(olc::P).bPressed) { Render::projecting = !Render::projecting; }

		//RMB press = select entity
		if (p->GetMouse(1).bPressed) {
			//reset selected
			if (selectedEntity) {
				selectedEntity->SetColor(olc::WHITE);
			}
			selectedEntity = nullptr;

			//check if mouse click contains an entity
			Vector3 mousePos = GetMousePos(p);
			for (auto& entity : Physics::hotEntities) {
				if (entity->ContainsPoint(mousePos)) {
					selectedEntity = entity;
					break;
				}
			}
			if (!selectedEntity) {
				for (auto& entity : Physics::coldEntities) {
					if (entity->ContainsPoint(mousePos)) {
						selectedEntity = entity;
						break;
					}
				}
			}

			//set selected to red
			if (selectedEntity) {
				selectedEntity->SetColor(olc::RED);
			}
		}

		if (selectedEntity) {
			std::string text = "ID: " + std::to_string(selectedEntity->id) + "\tTag: " + selectedEntity->tag + "\n";
			text += "Position:" + selectedEntity->position.str() + "\nRotation:" + selectedEntity->rotation.str() + "\nScale:" + selectedEntity->scale.str() +  "\n";
			if (PhysEntity* entity = dynamic_cast<PhysEntity*>(selectedEntity)) {
				text += "Is Static: "; text += entity->bStatic ? "true" : "false"; text += "\nMass: " + std::to_string(entity->mass) + "\n";
				text += "Velocity:" + entity->velocity.str() + "\nAcceleration:" + entity->acceleration.str() + "\n";
				text += "rotVelocity:" + entity->rotVelocity.str() + "\nrotAcceleration:" + entity->rotAcceleration.str() + "\n";
			}

			p->DrawStringDecal(olc::vf2d(0, 0), text);
		}

		//point debugging
		if()
	}

	//NOTE: selected entity should never point to a NEW object, since Input shouldnt own that object
	static void Cleanup() {
		//delete selectedEntity;
		selectedEntity = nullptr;
	}
};