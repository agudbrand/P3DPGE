#pragma once
#include "olcPixelGameEngine.h"
#include "Entities.h"

namespace Render {

	static std::vector<Entity*> entities;

	bool projecting = true;

	//booleans for pausing and advancing by frame
	//pausing is sort of archaic right now but should be fleshed out later
	//there are some in Physics as well
	static bool paused = false;
	static bool frame = false;

	//just shove all entities into here, then draw them
	static void AddEntity(Entity* e) {
		entities.push_back(e);
	}

	static void Init() {
		
	}

	using namespace boost::qvm;
	//projection matrix
	//made as function so that if the screen size changes the projection is recalculated
	//it may be beneficial to store this result and only recalculate when screen size changes
	//or when FOV is changed or something
	//this matrix seems to only work well with 1:1 aspect ratios I think its cause FOV is set to 90
	mat<float, 4, 4> ProjectionMatrix(olc::PixelGameEngine* p) {

		float nearz = 0.1;
		float farz = 1000.0;
		float fov = 90;
		float aspectRatio = (float)p->ScreenHeight() / (float)p->ScreenWidth();
		float fovRad = 1.0 / tanf(fov * 0.5 / 180.0 * 3.14159);

		mat<float, 4, 4> proj{
			aspectRatio * fovRad, 0,	  0,								0,
			0,					  fovRad, 0,								0,
			0,					  0,	  farz / (farz - nearz),			1,
			0,					  0,	  (-farz * nearz) / (farz - nearz), 0
		};

		return proj;
	}

	//TODO: make ortho projection
	static void WorldToScreenOrtho() {}

	//draw all entities to screen
	//TODO: cull entites not on screen
	static void Update(olc::PixelGameEngine* p) {
		for (auto& e : entities) { 
			if (!paused || frame) {
				if (projecting) { e->ProjectToScreen(ProjectionMatrix(p), p); }
				if (frame) { frame = !frame; }
			}
			e->Draw(p);
			Vector3 normal = e->mesh.triangles[0].get_normal();
			normal.ProjToScreen(ProjectionMatrix(p), p, e->position);
			Vector3 origin = V3ZERO;
			origin.ProjToScreen(ProjectionMatrix(p), p, V3ZERO);
			//p->DrawLine(olc::vd2d(origin.x, origin.y), olc::vd2d(normal.x, normal.y));
		}
	}

	

	static void Cleanup() {
		//for (auto vecPtr : drawnEntities) {
		//	delete vecPtr;
		//	vecPtr = nullptr;
		//}
		//drawnEntities.clear();
	}

};