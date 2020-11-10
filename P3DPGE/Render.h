#pragma once
#include "olcPixelGameEngine.h"
#include "Entities.h"

namespace Render {

	static std::vector<Entity*> entities;

	bool projecting = true;

	//just shove all entities into here, then draw them
	static void AddEntity(Entity* e) {
		entities.push_back(e);
	}

	static void Init() {
		//drawnEntities = std::vector<std::vector<Entity>*>();
		//AddEntityVector(&Physics::hotEntities);
		//AddEntityVector(&Physics::coldEntities);
	}

	using namespace boost::qvm;
	//projection matrix
	//made as function so that if the screen size changes the projection is recalculated
	//it may be beneficial to store this result and only recalculate when screen size changes
	//or when FOV is changed or something
	mat<float, 4, 4> ProjectionMatrix(olc::PixelGameEngine* p) {

		float nearz = 0.1;
		float farz = 1000.0;
		float fov = 90.0;
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

	//converts world points to points on the screen
	//hopefully
	/*
	static void WorldToScreen(olc::PixelGameEngine* p, Entity* e) {
		std::vector<Triangle> tris = e->mesh.triangles;
		for (Triangle t : tris) {
			t.points[0].M4x4ToVector3(t.points[0].ConvertToM4x4() * ProjectionMatrix(p));
			t.points[1].M4x4ToVector3(t.points[1].ConvertToM4x4() * ProjectionMatrix(p));
			t.points[2].M4x4ToVector3(t.points[2].ConvertToM4x4() * ProjectionMatrix(p));
		}
	}*/

	//TODO: make ortho projection
	static void WorldToScreenOrtho() {}

	//draw all entities to screen
	//Rotate calls are temporary
	//TODO: cull entites not on screen
	static void Update(olc::PixelGameEngine* p) {
		for (auto& e : entities) { 
			e->RotateX();
			e->RotateY();
			e->RotateZ();
			if (projecting) {
				e->ProjectToScreen(ProjectionMatrix(p), p);
			}
			
			e->Draw(p); 
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