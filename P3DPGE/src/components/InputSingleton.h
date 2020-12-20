#pragma once
#include "Component.h"
#include "../internal/olcPixelGameEngine.h"
#include "../math/Vector3.h"

struct UI;

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

//input modifiers
#define NONE_HELD	0
#define ANY_HELD	1
#define CTRL_HELD	2
#define SHIFT_HELD	4
//#define ALT_HELD	8 //ALT not supported by PGE v2.09

struct InputSingleton : public Component {
	olc::HWButton (*keyboardState)[256];
	olc::HWButton (*mouseState)[olc::nMouseButtons];

	Vector2 mousePos;
	bool ui_drag_latch;
	bool ui_clicked;
	UI* selectedUI;

	InputSingleton(olc::PixelGameEngine* p) {
		keyboardState = &p->pKeyboardState;
		mouseState = &p->pMouseState;
		mousePos = Vector2(p->ScreenWidth(), p->ScreenHeight());
		ui_drag_latch = false;
		ui_clicked = false;
		selectedUI = 0;
	}

	inline bool KeyPressed(olc::Key key) {
		return (*keyboardState)[key].bPressed;
	}

	bool KeyPressed(olc::Key key, uint32 modifiers) {
		switch(modifiers) {
			case(NONE_HELD): {				//!shift && !ctrl && (pressed || held)
				return (*keyboardState)[key].bPressed && 
					!((*keyboardState)[olc::CTRL].bHeld || (*keyboardState)[olc::SHIFT].bHeld);
			}
			case(CTRL_HELD): {				//!shift && ctrl && (pressed || held)
				return (*keyboardState)[key].bPressed &&
					(*keyboardState)[olc::CTRL].bHeld && !((*keyboardState)[olc::SHIFT].bHeld);
			}
			case(SHIFT_HELD): {				//shift && !ctrl && (pressed || held)
				return (*keyboardState)[key].bPressed && 
					(*keyboardState)[olc::SHIFT].bHeld && !((*keyboardState)[olc::CTRL].bHeld);
			}
			case(CTRL_HELD | SHIFT_HELD): { //shift && ctrl && (pressed || held)
				return (*keyboardState)[key].bPressed &&
					((*keyboardState)[olc::SHIFT].bHeld && (*keyboardState)[olc::CTRL].bHeld);
			}
			default: {								//pressed || held
				return (*keyboardState)[key].bPressed;
			}
		}
	}

	inline bool KeyHeld(olc::Key key) {
		return (*keyboardState)[key].bHeld;
	}

	bool KeyDown(olc::Key key, uint32 modifiers = ANY_HELD) {
		switch(modifiers) {
			case(NONE_HELD): {				//!shift && !ctrl && (pressed || held)
				return !((*keyboardState)[olc::CTRL].bHeld || (*keyboardState)[olc::SHIFT].bHeld) &&
					(*keyboardState)[key].bPressed || (*keyboardState)[key].bHeld;
			}
			case(CTRL_HELD): {				//!shift && ctrl && (pressed || held)
				return (*keyboardState)[olc::CTRL].bHeld && !((*keyboardState)[olc::SHIFT].bHeld) &&
					((*keyboardState)[key].bPressed || (*keyboardState)[key].bHeld);
			}
			case(SHIFT_HELD): {				//shift && !ctrl && (pressed || held)
				return (*keyboardState)[olc::SHIFT].bHeld && !((*keyboardState)[olc::CTRL].bHeld) &&
					((*keyboardState)[key].bPressed || (*keyboardState)[key].bHeld);
			}
			case(CTRL_HELD | SHIFT_HELD): { //shift && ctrl && (pressed || held)
				return ((*keyboardState)[olc::SHIFT].bHeld && (*keyboardState)[olc::CTRL].bHeld) &&
					((*keyboardState)[key].bPressed || (*keyboardState)[key].bHeld);
			}
			default: {								//pressed || held
				return (*keyboardState)[key].bPressed || (*keyboardState)[key].bHeld;
			}
		}
	}

	inline bool KeyReleased(olc::Key key) {
		return (*keyboardState)[key].bReleased;
	}

	inline bool MousePressed(uint32_t button) {
		return (*mouseState)[button].bPressed;
	}

	inline bool MouseHeld(uint32_t button) {
		return (*mouseState)[button].bHeld;
	}

	bool MouseDown(uint32_t button, uint32 modifiers = ANY_HELD) {
		switch(modifiers) {
			case(NONE_HELD): {				//!shift && !ctrl && (pressed || held)
				return !((*mouseState)[olc::CTRL].bHeld || (*mouseState)[olc::SHIFT].bHeld) &&
					(*mouseState)[button].bPressed || (*mouseState)[button].bHeld;
			}
			case(CTRL_HELD): {				//!shift && ctrl && (pressed || held)
				return (*mouseState)[olc::CTRL].bHeld && !((*mouseState)[olc::SHIFT].bHeld) &&
					((*mouseState)[button].bPressed || (*mouseState)[button].bHeld);
			}
			case(SHIFT_HELD): {				//shift && !ctrl && (pressed || held)
				return (*mouseState)[olc::SHIFT].bHeld && !((*mouseState)[olc::CTRL].bHeld) &&
					((*mouseState)[button].bPressed || (*mouseState)[button].bHeld);
			}
			case(CTRL_HELD | SHIFT_HELD): { //shift && ctrl && (pressed || held)
				return ((*mouseState)[olc::SHIFT].bHeld && (*mouseState)[olc::CTRL].bHeld) &&
					((*mouseState)[button].bPressed || (*mouseState)[button].bHeld);
			}
			default: {								//pressed || held
				return (*mouseState)[button].bPressed || (*mouseState)[button].bHeld;
			}
		}
	}

	inline bool MouseReleased(uint32_t button) {
		return (*mouseState)[button].bReleased;
	}
};
