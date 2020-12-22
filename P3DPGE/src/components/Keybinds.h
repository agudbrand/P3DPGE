#pragma once
#include "Component.h"

//TODO(i,delle) look into bit combining so that CTRL = 1024 and SHIFT = 2048

struct Keybinds : public Component {
	//flying movement
	olc::Key movementFlyingUp;
	olc::Key movementFlyingDown;
	olc::Key movementFlyingForward;
	olc::Key movementFlyingBack;
	olc::Key movementFlyingRight;
	olc::Key movementFlyingLeft;

	//camera rotation
	olc::Key cameraRotateUp;
	olc::Key cameraRotateDown;
	olc::Key cameraRotateRight;
	olc::Key cameraRotateLeft;

	//render debug
	olc::Key debugRenderWireframe;
	olc::Key debugRenderEdgesNumbers;
	olc::Key debugRenderDisplayAxis;

	Keybinds() {
		movementFlyingUp =		olc::E;
		movementFlyingDown =	olc::Q;
		movementFlyingForward =	olc::W;
		movementFlyingBack =	olc::S;
		movementFlyingRight =	olc::D;
		movementFlyingLeft =	olc::A;
		
		cameraRotateUp =	olc::UP;
		cameraRotateDown =	olc::DOWN;
		cameraRotateRight =	olc::RIGHT;
		cameraRotateLeft =	olc::LEFT;

		debugRenderWireframe =		olc::COMMA;
		debugRenderEdgesNumbers =	olc::PERIOD;
		debugRenderDisplayAxis =	INPUT_FORWARDSLASH;
	}

	//Keybinds(file ...) {} //TODO(i,delle) look into saving/loading keybinds with a file

};