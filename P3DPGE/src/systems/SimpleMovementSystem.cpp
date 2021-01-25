#pragma once
#include "SimpleMovementSystem.h"
#include "../EntityAdmin.h"
#include "../internal/olcPixelGameEngine.h"

#include "../components/Input.h"
#include "../components/Keybinds.h"
#include "../components/Camera.h"
#include "../components/MovementState.h"
#include "../components/Time.h"
#include "../internal/imgui/imgui.h"

#include "../math/Math.h"

EntityAdmin* ladmin; //im not rewriting every function to take admin

void CameraMovement(float deltaTime, Camera* camera, Input* input, Keybinds* binds, uint32 moveState) {
	if (!ladmin->IMGUI_KEY_CAPTURE) {
		if (moveState & MOVEMENT_FLYING) {
			//translate up
			if (input->KeyDown(binds->movementFlyingUp)) {
				if (input->KeyHeld(olc::SHIFT)) {
					camera->position.y += 16 * deltaTime;
				}
				else if (input->KeyHeld(olc::CTRL)) {
					camera->position.y += 4 * deltaTime;
				}
				else {
					camera->position.y += 8 * deltaTime;
				}
			}

			//translate down
			if (input->KeyDown(binds->movementFlyingDown)) {
				if (input->KeyHeld(olc::SHIFT)) {
					camera->position.y -= 16 * deltaTime;
				}
				else if (input->KeyHeld(olc::CTRL)) {
					camera->position.y -= 4 * deltaTime;
				}
				else {
					camera->position.y -= 8 * deltaTime;
				}
			}
		}

		//translate forward
		if (input->KeyDown(binds->movementFlyingForward)) {
			if (input->KeyHeld(olc::SHIFT)) {
				camera->position += camera->lookDir.yInvert() * 16 * deltaTime;
			}
			else if (input->KeyHeld(olc::CTRL)) {
				camera->position += camera->lookDir.yInvert() * 4 * deltaTime;
			}
			else {
				camera->position += camera->lookDir.yInvert() * 8 * deltaTime;
			}
		}

		//translate back
		if (input->KeyDown(binds->movementFlyingBack)) {
			if (input->KeyHeld(olc::SHIFT)) {
				camera->position -= camera->lookDir.yInvert() * 16 * deltaTime;
			}
			else if (input->KeyHeld(olc::CTRL)) {
				camera->position -= camera->lookDir.yInvert() * 4 * deltaTime;
			}
			else {
				camera->position -= camera->lookDir.yInvert() * 8 * deltaTime;
			}
		}

		//translate right
		if (input->KeyDown(binds->movementFlyingRight)) {
			if (input->KeyHeld(olc::SHIFT)) {
				camera->position -= camera->lookDir.cross(Vector3::UP) * 16 * deltaTime;
			}
			else if (input->KeyHeld(olc::CTRL)) {
				camera->position -= camera->lookDir.cross(Vector3::UP) * 4 * deltaTime;
			}
			else {
				camera->position -= camera->lookDir.cross(Vector3::UP) * 8 * deltaTime;
			}
		}

		//translate left
		if (input->KeyDown(binds->movementFlyingLeft)) {
			if (input->KeyHeld(olc::SHIFT)) {
				camera->position += camera->lookDir.cross(Vector3::UP) * 16 * deltaTime;
			}
			else if (input->KeyHeld(olc::CTRL)) {
				camera->position += camera->lookDir.cross(Vector3::UP) * 4 * deltaTime;
			}
			else {
				camera->position += camera->lookDir.cross(Vector3::UP) * 8 * deltaTime;
			}
		}
	}

	
}

void CameraRotation(float deltaTime, Camera* camera, Input* input, Keybinds* binds) {

	if (input->KeyPressed(olc::M)) { ladmin->currentCamera->MOUSE_LOOK = !ladmin->currentCamera->MOUSE_LOOK; }
	if (!ladmin->IMGUI_KEY_CAPTURE) {
		if (!ladmin->currentCamera->MOUSE_LOOK) {
			//camera rotation up
			if (input->KeyPressed(binds->cameraRotateUp) || input->KeyHeld(binds->cameraRotateUp)) {
				if (input->KeyHeld(olc::SHIFT))		{ camera->target.z = Math::clamp(camera->target.z + 50 * deltaTime, 1,179); }
				else if (input->KeyHeld(olc::CTRL)) { camera->target.z = Math::clamp(camera->target.z + 5 * deltaTime, 1,179); }
				else								{ camera->target.z = Math::clamp(camera->target.z + 25 * deltaTime, 1,179); }
			}

			//camera rotation down
			if (input->KeyPressed(binds->cameraRotateDown) || input->KeyHeld(binds->cameraRotateDown)) {
				if (input->KeyHeld(olc::SHIFT))		{ camera->target.z = Math::clamp(camera->target.z - 50 * deltaTime, 1,179); }
				else if (input->KeyHeld(olc::CTRL)) { camera->target.z = Math::clamp(camera->target.z - 5 * deltaTime, 1,179); }
				else								{ camera->target.z = Math::clamp(camera->target.z - 25 * deltaTime, 1, 179); }
			}

			//camera rotation right
			if (input->KeyPressed(binds->cameraRotateRight) || input->KeyHeld(binds->cameraRotateRight)) {
				if (input->KeyHeld(olc::SHIFT))		{ camera->target.y -= 50 * deltaTime; }
				else if (input->KeyHeld(olc::CTRL)) { camera->target.y -= 5 * deltaTime; }
				else								{ camera->target.y -= 25 * deltaTime; }
			}

			//camera rotation left
			if (input->KeyPressed(binds->cameraRotateLeft) || input->KeyHeld(binds->cameraRotateLeft)) {
				if (input->KeyHeld(olc::SHIFT))		{ camera->target.y += 50 * deltaTime; }
				else if (input->KeyHeld(olc::CTRL)) { camera->target.y += 5 * deltaTime; }
				else								{ camera->target.y += 25 * deltaTime; }
			}
		}
		else {
			
		}
	}
}

void SimpleMovementSystem::Update() {
	Camera*				camera = admin->currentCamera;
	Input*				input = admin->input;
	Keybinds*			binds = admin->currentKeybinds;
	uint32				moveState = admin->tempMovementState->movementState;
	float				deltaTime = admin->time->deltaTime;
	ladmin = admin;
	CameraMovement(deltaTime, camera, input, binds, moveState);
	CameraRotation(deltaTime, camera, input, binds);
}