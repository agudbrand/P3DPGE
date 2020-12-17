#pragma once
#include "Component.h"
#include "../math/Vector3.h"
#include "../internal/olcPixelGameEngine.h"

struct ScreenSingleton : public Component {
	float width;
	float height;
	float resolution;
	Vector2 dimensions;
	Vector3 dimensionsV3;
	Vector2 mousePos;
	Vector3 mousePosV3;
	bool changedResolution;

	ScreenSingleton(olc::PixelGameEngine* p) {
		width = p->ScreenWidth();
		height = p->ScreenHeight();
		resolution = width * height;
		dimensions = Vector2(width, height);
		dimensionsV3 = Vector3(width, height);
		mousePos = p->GetMousePos();
		mousePosV3 = Vector3(mousePos);
		changedResolution = true;
	}
};