#pragma once
#include "SimpleMovementSystem.h"
#include "../EntityAdmin.h"
#include "../internal/olcPixelGameEngine.h"

#include "../components/InputSingleton.h"
#include "../components/Camera.h"
#include "../components/Keybinds.h"
#include "../components/MovementState.h"

void SimpleMovementSystem::CameraMovement(float deltaTime, Camera* camera, InputSingleton* input, Keybinds* binds, uint32 moveState) {
	if(moveState & MOVEMENT_FLYING) {
		//translate up
		if(input->KeyPressed(binds->movementFlyingUp) || input->KeyHeld(binds->movementFlyingUp)) {
			if(input->KeyHeld(olc::SHIFT)) {
				camera->position.y += 16 * deltaTime;
			} else if(input->KeyHeld(olc::CTRL)) {
				camera->position.y += 4 * deltaTime;
			} else {
				camera->position.y += 8 * deltaTime;
			}
		}

		//translate down
		if(input->KeyPressed(binds->movementFlyingDown) || input->KeyHeld(binds->movementFlyingDown)) {
			if(input->KeyHeld(olc::SHIFT)) {
				camera->position.y -= 16 * deltaTime;
			} else if(input->KeyHeld(olc::CTRL)) {
				camera->position.y -= 4 * deltaTime;
			} else {
				camera->position.y -= 8 * deltaTime;
			}
		}
	} else {
		camera->position.y -= GRAVITY * deltaTime;
	}

	//translate forward
	if(input->KeyPressed(binds->movementFlyingForward) || input->KeyHeld(binds->movementFlyingForward)) {
		if(input->KeyHeld(olc::SHIFT)) {
			camera->position += camera->lookDir * 16 * deltaTime;
		} else if(input->KeyHeld(olc::CTRL)) {
			camera->position += camera->lookDir * 4 * deltaTime;
		} else {
			camera->position += camera->lookDir * 8 * deltaTime;
		}
	}

	//translate back
	if(input->KeyPressed(binds->movementFlyingBack) || input->KeyHeld(binds->movementFlyingBack)) {
		if(input->KeyHeld(olc::SHIFT)) {
			camera->position -= camera->lookDir * 16 * deltaTime;
		} else if(input->KeyHeld(olc::CTRL)) {
			camera->position -= camera->lookDir * 4 * deltaTime;
		} else {
			camera->position -= camera->lookDir * 8 * deltaTime;
		}
	}

	//translate right
	if(input->KeyPressed(binds->movementFlyingRight) || input->KeyHeld(binds->movementFlyingRight)) {
		if(input->KeyHeld(olc::SHIFT)) {
			camera->position -= camera->lookDir.cross(Vector3::UP) * 16 * deltaTime;
		} else if(input->KeyHeld(olc::CTRL)) {
			camera->position -= camera->lookDir.cross(Vector3::UP) * 4 * deltaTime;
		} else {
			camera->position -= camera->lookDir.cross(Vector3::UP) * 8 * deltaTime;
		}
	}

	//translate left
	if(input->KeyPressed(binds->movementFlyingLeft) || input->KeyHeld(binds->movementFlyingLeft)) {
		if(input->KeyHeld(olc::SHIFT)) {
			camera->position += camera->lookDir.cross(Vector3::UP) * 16 * deltaTime;
		} else if(input->KeyHeld(olc::CTRL)) {
			camera->position += camera->lookDir.cross(Vector3::UP) * 4 * deltaTime;
		} else {
			camera->position += camera->lookDir.cross(Vector3::UP) * 8 * deltaTime;
		}
	}

	
}

void SimpleMovementSystem::Update(float deltaTime, olc::PixelGameEngine* p) {
	Camera* camera = admin->singletonCamera;
	InputSingleton* input = admin->singletonInput;
	Keybinds* binds = admin->singletonKeybinds;
	uint32 moveState = admin->singletonMovementState->movementState;

	CameraMovement(deltaTime, camera, input, binds, moveState);

	//camera rotation up
	if(input->KeyPressed(binds->cameraRotateUp) || input->KeyHeld(binds->cameraRotateUp)) {
		if(input->KeyHeld(olc::SHIFT)) {
			camera->rotation.x += 100 * deltaTime;
		} else if(input->KeyHeld(olc::CTRL)) {
			camera->rotation.x += 25 * deltaTime;
		} else {
			camera->rotation.x += 50 * deltaTime;
		}
	}

	//camera rotation down
	if(input->KeyPressed(binds->cameraRotateDown) || input->KeyHeld(binds->cameraRotateDown)) {
		if(input->KeyHeld(olc::SHIFT)) {
			camera->rotation.x -= 100 * deltaTime;
		} else if(input->KeyHeld(olc::CTRL)) {
			camera->rotation.x -= 25 * deltaTime;
		} else {
			camera->rotation.x -= 50 * deltaTime;
		}
	}

	//camera rotation right
	if(input->KeyPressed(binds->cameraRotateRight) || input->KeyHeld(binds->cameraRotateRight)) {
		if(input->KeyHeld(olc::SHIFT)) {
			camera->rotation.y -= 100 * deltaTime;
		} else if(input->KeyHeld(olc::CTRL)) {
			camera->rotation.y -= 25 * deltaTime;
		} else {
			camera->rotation.y -= 50 * deltaTime;
		}
	}

	//camera rotation left
	if(input->KeyPressed(binds->cameraRotateLeft) || input->KeyHeld(binds->cameraRotateLeft)) {
		if(input->KeyHeld(olc::SHIFT)) {
			camera->rotation.y += 100 * deltaTime;
		} else if(input->KeyHeld(olc::CTRL)) {
			camera->rotation.y += 25 * deltaTime;
		} else {
			camera->rotation.y += 50 * deltaTime;
		}
	}
	
	
}