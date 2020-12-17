#pragma once
#include "Component.h"
#include "../internal/olcPixelGameEngine.h"

#define MOUSE_BUTTON_LEFT		0
#define MOUSE_BUTTON_RIGHT		1
#define MOUSE_BUTTON_MIDDLE		2
#define MOUSE_BUTTON_4			3
#define MOUSE_BUTTON_5			4

#ifdef KEYBOARD_LAYOUT_US_UK
#define KEY_SEMICOLON		olc::OEM_1
#define KEY_BACKSLASH		olc::OEM_2
#define KEY_TILDE			olc::OEM_3
#define KEY_LEFT_BRACKET	olc::OEM_4
#define KEY_FORWARD_SLASH	olc::OEM_5
#define KEY_RIGHT_BRACKET	olc::OEM_6
#define KEY_SINGLE_QUOTE	olc::OEM_7
#endif

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
