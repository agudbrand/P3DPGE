#pragma once
#include "UI.h"

enum struct UIAnchor {
	NONE, TOPLEFT, TOPMIDDLE, TOPRIGHT, MIDDLELEFT, MIDDLE, MIDDLERIGHT, BOTTOMLEFT, BOTTOMMIDDLE, BOTTOMRIGHT
};

struct UI;
struct UIContainer {
	Vector2 position;
	UIAnchor anchor;
	std::vector<UI*> container;
	~UIContainer();
};

struct UIRow : public UIContainer {
	float uniformWidth, uniformHeight;
	bool growRight;
	UI* activeUI;

	UIRow(Vector2 position, UIAnchor anchor = UIAnchor::NONE, bool growRight = true, float uniformWidth = 0, float uniformHeight = 0);

	//Adds UI element to the end
	void Add(UI* element); //TEST(delle) test if this degens by 1 pixel
	//Inserts UI element at specified index
	void Insert(uint32 index, UI* element);

	float Width();
	float Height();
};