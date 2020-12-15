#pragma once
#include "Component.h"
#include "../internal/olcPixelGameEngine.h"
#include "../geometry/Triangle.h"
#include "Light.h"

struct Scene : public Component {
	std::vector<Triangle*> triangles;
	std::vector<Light*> lights;
	std::vector<float> pixelDepthBuffer;

	bool WIRE_FRAME		= false;
	bool DISPLAY_EDGES	= false;

	Scene(olc::PixelGameEngine* p) {
		triangles = std::vector<Triangle*>();
		lights = std::vector<Light*>();
		pixelDepthBuffer.reserve((size_t)p->ScreenWidth() * p->ScreenHeight());
	}
};