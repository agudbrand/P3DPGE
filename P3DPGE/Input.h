#pragma once
#include "Physics.h"
#include "Render.h"

namespace Input {
	static Entity* selectedEntity;

	static Vector3 GetMousePos(olc::PixelGameEngine* p) {
		return Vector3(p->GetMouseX(), p->GetMouseY(), 0);
	}

	static void Update(olc::PixelGameEngine* p, float& deltaTimePtr) {
		//G press = pause
		if (p->GetKey(olc::G).bHeld) {
			deltaTimePtr = 0;
		}

		if (p->GetKey(olc::M).bPressed) {
			std::cout << "Entities to draw: " << std::endl;
			std::cout << "test" << Render::drawnEntities.size() << std::endl;
			for (auto &entity : Render::drawnEntities) {
				if (entity) {
					printf("ID: %d, Pos: %s\n", entity->id, entity->position.str());
				} else {
					break;
				}
			}
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
			Vector3 pos = Vector3(p->GetMouseX(), p->GetMouseY(), 0);
			Box* box = new Box(Vector3(10, 10, 0), -1, pos);
			Physics::AddEntity(box, true);
			Render::AddEntity(box);
			std::cout << "Creating Box at: " + pos.str() << std::endl;
		}

		//LMB press = select entity
		if (p->GetMouse(0).bPressed) {
			//reset selected
			if (selectedEntity) {
				selectedEntity->SetColor(olc::WHITE);
			}
			selectedEntity = nullptr;

			//check if mouse click contains
			Vector3 mousePos = GetMousePos(p);
			std::cout << mousePos.str() << std::endl;
			for (auto &entity : Physics::hotEntities) {
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

		//LMB hold = add force to selectd entity
		if (p->GetMouse(0).bHeld && selectedEntity) {
			
		}
	}

	//NOTE: selected entity should never point to a NEW object, since Input shouldnt own that object
	static void Cleanup() {
		//delete selectedEntity;
		selectedEntity = nullptr;
	}
};