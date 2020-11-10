#pragma once
#include "olcPixelGameEngine.h"
#include "Entities.h"

namespace Render {

	static std::vector<Entity*> entities;

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

		float fNear = 0.1;
		float fFar = 1000.0;
		float fFov = 90.0;
		float fAspectRatio = (float)p->ScreenHeight() / (float)p->ScreenWidth();
		float fFovRad = 1.0 / tanf(fFov * 0.5 / 180.0 * 3.14159);

		mat<float, 4, 4> proj;
		proj.a[0][0] = fAspectRatio * fFovRad;
		proj.a[1][1] = fFovRad;
		proj.a[2][2] = fFar / (fFar - fNear);
		proj.a[3][2] = (-fFar * fNear) / (fFar - fNear);
		proj.a[2][3] = 1.0f;
		proj.a[3][3] = 0.0f;

		return proj;
	}

	//converts world points to points on the screen
	//hopefully
	static void WorldToScreen(olc::PixelGameEngine* p, Entity* e) {
		std::vector<Triangle> tris = e->mesh.triangles;
		for (Triangle t : tris) {
			t.points[0].M4x4ToVector3(t.points[0].ConvertToM4x4() * ProjectionMatrix(p));
			t.points[1].M4x4ToVector3(t.points[1].ConvertToM4x4() * ProjectionMatrix(p));
			t.points[2].M4x4ToVector3(t.points[2].ConvertToM4x4() * ProjectionMatrix(p));
		}
	}

	//TODO: make ortho projection
	static void WorldToScreenOrtho() {}

	static void Update(olc::PixelGameEngine* p) {
		for (auto& e : entities) { 
			WorldToScreen(p, e);
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