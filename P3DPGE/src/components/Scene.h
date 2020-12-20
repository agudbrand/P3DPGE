#pragma once
#include "Component.h"
#include "../internal/olcPixelGameEngine.h"

struct Triangle;
struct Light;
struct Edge3D;

struct Scene : public Component {
	std::vector<Triangle*> triangles;
	std::vector<Edge3D*> lines;
	std::vector<Light*> lights;
	std::vector<float> pixelDepthBuffer;

	bool WIRE_FRAME				= false;
	bool DISPLAY_EDGES			= false;
	bool WIRE_FRAME_NO_TEXTURE	= false;
	bool LOCAL_AXIS				= false;
	bool GLOBAL_AXIS			= false;

	Scene(olc::PixelGameEngine* p) {
		triangles = std::vector<Triangle*>();
		lights = std::vector<Light*>();
		pixelDepthBuffer = std::vector<float>((size_t)p->ScreenWidth() * (size_t)p->ScreenHeight());
	}
};