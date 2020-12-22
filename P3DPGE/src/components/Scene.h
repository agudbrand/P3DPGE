#pragma once
#include "Component.h"

struct Light;
struct Edge3D;
struct Mesh;

struct Scene : public Component {
	std::vector<Mesh*> meshes;
	std::vector<Edge3D*> lines;
	std::vector<Light*> lights;
	std::vector<float> pixelDepthBuffer;

	bool RENDER_WIREFRAME				= false;
	bool RENDER_EDGE_NUMBERS			= false;
	bool RENDER_TEXTURES				= true;
	bool RENDER_LOCAL_AXIS				= false;
	bool RENDER_GLOBAL_AXIS				= false;

	Scene(olc::PixelGameEngine* p) {
		meshes = std::vector<Mesh*>();
		lights = std::vector<Light*>();
		lights = std::vector<Light*>();
		pixelDepthBuffer = std::vector<float>((size_t)p->ScreenWidth() * (size_t)p->ScreenHeight());
	}
};