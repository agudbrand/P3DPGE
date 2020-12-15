#pragma once
#include "Component.h"
#include "../internal/olcPixelGameEngine.h"

#define LEFT_CLICK		0
#define RIGHT_CLICK		1
#define MIDDLE_CLICK	2
#define MOUSE_BUTTON_4	3
#define MOUSE_BUTTON_5	4

struct InputSingleton : public Component {
	olc::HWButton (*keyboardState)[256];
	olc::HWButton (*mouseState)[olc::nMouseButtons];

	InputSingleton(olc::PixelGameEngine* p) {
		keyboardState = &p->pKeyboardState;
		mouseState = &p->pMouseState;
	}

	inline bool KeyPressed(olc::Key key) {
		return (*keyboardState)[key].bPressed;
	}

	inline bool KeyHeld(olc::Key key) {
		return (*keyboardState)[key].bHeld;
	}

	inline bool KeyReleased(olc::Key key) {
		return (*keyboardState)[key].bReleased;
	}

	inline bool MousePressed(uint32_t button) {
		return (*keyboardState)[button].bPressed;
	}

	inline bool MouseHeld(uint32_t button) {
		return (*keyboardState)[button].bHeld;
	}

	inline bool MouseReleased(uint32_t button) {
		return (*keyboardState)[button].bReleased;
	}
};
