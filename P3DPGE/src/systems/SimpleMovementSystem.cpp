#pragma once
#include "SimpleMovementSystem.h"
#include "../EntityAdmin.h"
#include "../internal/olcPixelGameEngine.h"

#include "../components/Input.h"
#include "../components/Keybinds.h"
#include "../components/Camera.h"
#include "../components/MovementState.h"
#include "../components/Time.h"
#include "../components/Screen.h"
#include "../internal/imgui/imgui.h"

#include "../math/Math.h"

EntityAdmin* ladmin; //im not rewriting every function to take admin
//turns out there wasnt that many functions 

void CameraMovement(float deltaTime, Camera* camera, Input* input, Keybinds* binds, uint32 moveState) {
	if (!ladmin->IMGUI_KEY_CAPTURE) {

		Vector3 inputs;

		if (moveState & MOVEMENT_FLYING) {
			//translate up
			if (input->KeyDown(binds->movementFlyingUp)) {  inputs.y += 1;  }

			//translate down
			if (input->KeyDown(binds->movementFlyingDown)) {  inputs.y -= 1; }
		}

		//translate forward
		if (input->KeyDown(binds->movementFlyingForward)) {  inputs += camera->lookDir.yInvert().normalized(); }

		//translate back
		if (input->KeyDown(binds->movementFlyingBack)) {  inputs -= camera->lookDir.yInvert().normalized(); }

		//translate right
		if (input->KeyDown(binds->movementFlyingRight)) {  inputs -= camera->lookDir.cross(Vector3::UP).normalized(); }

		//translate left
		if (input->KeyDown(binds->movementFlyingLeft)) { inputs += camera->lookDir.cross(Vector3::UP).normalized(); }


		if (input->KeyHeld(olc::SHIFT))		{ camera->position += inputs.normalized() * 16 * deltaTime; }
		else if (input->KeyHeld(olc::CTRL)) { camera->position += inputs.normalized() * 4 * deltaTime; }
		else								{ camera->position += inputs.normalized() * 8 * deltaTime; }
	}


	
}

void CameraRotation(float deltaTime, Camera* camera, Input* input, Keybinds* binds) {

	if (input->KeyPressed(olc::N)) { 
		ladmin->currentCamera->MOUSE_LOOK = !ladmin->currentCamera->MOUSE_LOOK; 
		ladmin->p->bMouseVisible = !ladmin->p->bMouseVisible;
	}
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
			ladmin->p->bMouseVisible = false; // i have to do this explicitly so the mouse is visible when ImGUI opens the console

			float x = (ladmin->input->mousePos.x * 2) - ladmin->screen->width;
			float y = (ladmin->input->mousePos.y * 2) - ladmin->screen->height;

			camera->target.z = Math::clamp(camera->target.z - 0.09 * y, 1, 179);
			camera->target.y = camera->target.y - 0.09 * x;
			
			ladmin->p->SetMousePositionLocal(ladmin->screen->dimensions / 2);
		}
	}
	else if(ladmin->currentCamera->MOUSE_LOOK) {
		ladmin->p->bMouseVisible = true;
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