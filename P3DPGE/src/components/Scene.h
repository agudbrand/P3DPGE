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

	bool RENDER_WIREFRAME				= true;
	bool RENDER_EDGE_NUMBERS			= false;
	bool RENDER_TEXTURES				= false;
	bool RENDER_LOCAL_AXIS				= true;
	bool RENDER_GLOBAL_AXIS				= false; //TODO(r,delle) implement global axis like in blender
	bool RENDER_TRANSFORMS				= false;
	bool RENDER_PHYSICS					= true;
	bool RENDER_SCREEN_BOUNDING_BOX		= false; 
	bool RENDER_MESH_VERTICES			= false; 
	bool RENDER_GRID					= false; //TODO(r,delle) upgrade grid to follow camera in smart way
	bool RENDER_LIGHT_RAYS				= false;

	Scene(olc::PixelGameEngine* p) {
		meshes = std::vector<Mesh*>();
		lights = std::vector<Light*>();
		lights = std::vector<Light*>();
		pixelDepthBuffer = std::vector<float>((size_t)p->ScreenWidth() * (size_t)p->ScreenHeight());
	}
};