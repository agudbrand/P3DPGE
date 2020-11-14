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
		////    Keyboard Input    /////
		//G press = pause
		if (p->GetKey(olc::G).bHeld) {
			deltaTimePtr = 0;
		}

		//Space = full pause
		if (p->GetKey(olc::SPACE).bPressed) {
			Render::paused = !Render::paused;
			Physics::paused = !Physics::paused;
		}

		//F = advance frame
		if (p->GetKey(olc::F).bPressed) {
			Render::frame = !Render::frame;
			Physics::frame = !Physics::frame;
		}

		//Q press = spawn sphere
		//SHIFT+Q press = spawn large sphere
		if (p->GetKey(olc::Q).bPressed) {
			Vector3 pos = Vector3(p->GetMouseX(), p->GetMouseY(), 0);
			Sphere* sphere;
			if (p->GetKey(olc::SHIFT).bHeld) {
				sphere = new Sphere(100, 0, pos);
				sphere->mass = sphere->radius;
			}
			else {
				sphere = new Sphere(10, 0, pos);
				sphere->mass = sphere->radius;
			}
			Physics::AddEntity(sphere);
			Render::AddEntity(sphere);
			std::cout << "Creating Sphere at: " + pos.str() << std::endl;
		}

		//T press = spawn complex test object
		if (p->GetKey(olc::T).bPressed) {
			Vector3 pos = Vector3(p->GetMouseX(), p->GetMouseY(), 0);
			Complex* complex = new Complex("test_object.obj", 0, Vector3(0, 0, 0));
			selectedEntity = complex;
			Physics::AddEntity(complex);
			Render::AddEntity(complex);
			std::cout << "Creating " + complex->model_name + " at: " + pos.str() << std::endl;
		}

		//E press = spawn box
		if (p->GetKey(olc::E).bPressed) {
			Vector3 pos = GetMousePos(p);
			pos.z = 1;
			Box* box = new Box(Vector3(1, 1, 1), -1, Vector3(0, 0, 3));
			selectedEntity = box;
			Physics::AddEntity(box);
			Render::AddEntity(box);
			std::cout << "Creating Box at: " + pos.str() << std::endl;
		}

		//rotation over axes
		if (p->GetKey(olc::J).bHeld) {
			for (auto& e : Render::entities) {
				e->rotation.x = 0.6;
				e->RotateX();
			}
		}

		//K held = rotate everything in the positive y
		if (p->GetKey(olc::K).bHeld) {
			for (auto& e : Render::entities) {
				e->rotation.y = 0.6;
				e->RotateY();
			}
		}

		//L held = rotate everything in the positive z
		if (p->GetKey(olc::L).bHeld) {
			for (auto& e : Render::entities) {
				e->rotation.z = 0.6;
				e->RotateZ();
			}
		}

		//M held = rotate everything in the negative x
		if (p->GetKey(olc::M).bHeld) {
			for (auto& e : Render::entities) {
				e->rotation.x = -0.6;
				e->RotateX();
			}
		}

		//COMMA held = rotate everything in the negative y
		if (p->GetKey(olc::COMMA).bHeld) {
			for (auto& e : Render::entities) {
				e->rotation.y = -0.6;
				e->RotateY();
			}
		}

		//PERIOD held = rotate everything in the negative z
		if (p->GetKey(olc::PERIOD).bHeld) {
			for (auto& e : Render::entities) {
				e->rotation.z = -0.6;
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
		if (p->GetKey(olc::W).bHeld) { Render::camera.position.y -= 8 * deltaTimePtr; }
		if (p->GetKey(olc::S).bHeld) { Render::camera.position.y += 8 * deltaTimePtr; }
		if (p->GetKey(olc::A).bHeld) { Render::camera.position.x -= 8 * deltaTimePtr; }
		if (p->GetKey(olc::D).bHeld) { Render::camera.position.x += 8 * deltaTimePtr; }
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
				entity->AddImpulse(nullptr, GetMousePos(p) - leftClickPos, true);
			}

			leftClickPos = V3NULL;
		}

		//RMB press = select entity
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

		if (selectedEntity) {
			std::string text = "ID: " + std::to_string(selectedEntity->id) + "\tTag: " + selectedEntity->tag + "\n";
			text += "Position:" + selectedEntity->position.str() + "\nRotation:" + selectedEntity->rotation.str() + "\nScale:" + selectedEntity->scale.str() + "\n";
			//why are you checking if a pointer to a PhysEntity is equal to an entity dynamically
			//casted to PhysEntity? also should it be = and not ==?
			// V answer to above V
			//im casting entity* to physentity* and if it works, then i print physentity info
			//it just verifies that its a physentity and creates a new physentity pointer to the physentity
			if (PhysEntity* entity = dynamic_cast<PhysEntity*>(selectedEntity)) {
				text += "Is Static: "; text += entity->bStatic ? "true" : "false"; text += "\nMass: " + std::to_string(entity->mass) + "\n";
				text += "Velocity:" + entity->velocity.str() + "\nAcceleration:" + entity->acceleration.str() + "\n";
				text += "rotVelocity:" + entity->rotVelocity.str() + "\nrotAcceleration:" + entity->rotAcceleration.str() + "\n";
			}

			p->DrawStringDecal(olc::vf2d(0, 0), text);
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
	}

	//NOTE: selected entity should never point to a NEW object, since Input shouldnt own that object
	static void Cleanup() {
		//delete selectedEntity;
		selectedEntity = nullptr;
	}
};