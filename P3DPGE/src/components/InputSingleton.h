#pragma once
#include "Component.h"
#include "../math/Vector3.h"

struct UI;
struct Entity;

struct InputSingleton : public Component {
	olc::HWButton (*keyboardState)[256];
	olc::HWButton (*mouseState)[olc::nMouseButtons];

	Vector2 mousePos;
	Vector2 mouseClickPos;

	//ui stuff
	bool ui_drag_latch = false;
	bool ui_clicked = false;
	UI* selectedUI = nullptr;

	//entity stuff
	Entity* selectedEntity = nullptr;

	InputSingleton(olc::PixelGameEngine* p) {
		keyboardState = &p->pKeyboardState;
		mouseState = &p->pMouseState;
		mousePos = Vector2(p->ScreenWidth(), p->ScreenHeight());
		mouseClickPos = mousePos;
	}

	inline bool KeyPressed(olc::Key key) {
		return (*keyboardState)[key].bPressed;
	}

	bool KeyPressed(olc::Key key, uint32 modifiers) {
		switch(modifiers) {
			case(INPUT_NONE_HELD): {				//!shift && !ctrl && pressed
				return (*keyboardState)[key].bPressed && 
					!((*keyboardState)[olc::CTRL].bHeld || (*keyboardState)[olc::SHIFT].bHeld);
			}
			case(INPUT_CTRL_HELD): {				//!shift && ctrl && pressed
				return (*keyboardState)[key].bPressed &&
					(*keyboardState)[olc::CTRL].bHeld && !((*keyboardState)[olc::SHIFT].bHeld);
			}
			case(INPUT_SHIFT_HELD): {				//shift && !ctrl && pressed
				return (*keyboardState)[key].bPressed && 
					(*keyboardState)[olc::SHIFT].bHeld && !((*keyboardState)[olc::CTRL].bHeld);
			}
			case(INPUT_CTRL_HELD | INPUT_SHIFT_HELD): { //shift && ctrl && pressed
				return (*keyboardState)[key].bPressed &&
					((*keyboardState)[olc::SHIFT].bHeld && (*keyboardState)[olc::CTRL].bHeld);
			}
			default: {								//pressed
				return (*keyboardState)[key].bPressed;
			}
		}
	}

	inline bool KeyHeld(olc::Key key) {
		return (*keyboardState)[key].bHeld;
	}

	bool KeyDown(olc::Key key, uint32 modifiers = INPUT_ANY_HELD) {
		switch(modifiers) {
			case(INPUT_NONE_HELD): {				//!shift && !ctrl && (pressed || held)
				return !(*keyboardState)[olc::CTRL].bHeld && !(*keyboardState)[olc::SHIFT].bHeld &&
					((*keyboardState)[key].bPressed || (*keyboardState)[key].bHeld);
			}
			case(INPUT_CTRL_HELD): {				//!shift && ctrl && (pressed || held)
				return (*keyboardState)[olc::CTRL].bHeld && !((*keyboardState)[olc::SHIFT].bHeld) &&
					((*keyboardState)[key].bPressed || (*keyboardState)[key].bHeld);
			}
			case(INPUT_SHIFT_HELD): {				//shift && !ctrl && (pressed || held)
				return (*keyboardState)[olc::SHIFT].bHeld && !((*keyboardState)[olc::CTRL].bHeld) &&
					((*keyboardState)[key].bPressed || (*keyboardState)[key].bHeld);
			}
			case(INPUT_CTRL_HELD | INPUT_SHIFT_HELD): { //shift && ctrl && (pressed || held)
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

	bool MouseDown(uint32_t button, uint32 modifiers = INPUT_ANY_HELD) {
		switch(modifiers) {
			case(INPUT_NONE_HELD): {				//!shift && !ctrl && (pressed || held)
				return !((*keyboardState)[olc::CTRL].bHeld || (*keyboardState)[olc::SHIFT].bHeld) &&
					((*mouseState)[button].bPressed || (*mouseState)[button].bHeld);
			}
			case(INPUT_CTRL_HELD): {				//!shift && ctrl && (pressed || held)
				return (*keyboardState)[olc::CTRL].bHeld && !((*keyboardState)[olc::SHIFT].bHeld) &&
					((*mouseState)[button].bPressed || (*mouseState)[button].bHeld);
			}
			case(INPUT_SHIFT_HELD): {				//shift && !ctrl && (pressed || held)
				return (*keyboardState)[olc::SHIFT].bHeld && !((*keyboardState)[olc::CTRL].bHeld) &&
					((*mouseState)[button].bPressed || (*mouseState)[button].bHeld);
			}
			case(INPUT_CTRL_HELD | INPUT_SHIFT_HELD): { //shift && ctrl && (pressed || held)
				return ((*keyboardState)[olc::SHIFT].bHeld && (*keyboardState)[olc::CTRL].bHeld) &&
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
