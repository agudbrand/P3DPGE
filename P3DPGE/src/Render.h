#pragma once
#include "Entities.h"
#include "olcPixelGameEngine.h"

namespace Render {
	static std::vector<Entity*> entities;

	static mat<float, 4, 4> view;
	static Camera camera;
	static float yaw;
	static float pitch;

	bool wireframe = false;

	//booleans for pausing and advancing by frame
	//pausing is sort of archaic right now but should be fleshed out later
	//there are some in Physics as well
	static bool paused = false;
	static bool frame = false;

	//just shove all entities into here, then draw them
	static void AddEntity(Entity* e) { entities.push_back(e); }

	static void Init() {
		//test lines, remove them if i forget pls
		//Line2* l1 = new Line2(Vector3(100, 100, 0), 1);
		//Line3* l2 = new Line3(Vector3(10, 10, 4), 1, Vector3(0, 2, 1));
		//entities.push_back(l1);
		//entities.push_back(l2);

		//test triangle
		//DebugTriangle* test = new DebugTriangle(1);
		//test->SetColor(olc::BLUE);
		//entities.push_back(test);
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

	//draw all entities to screen
	static void Update(olc::PixelGameEngine* p) {
		view = camera.MakeViewMatrix(yaw);

		//draw all entities
		for (auto& e : entities) {
			e->mesh->Update(camera.position, ProjectionMatrix(p), view);
			e->Draw(p, wireframe);

			//draw normal lines
			//this really needs to be somewhere else
			for (Triangle t : e->mesh->drawnTriangles) {
				//Line3 line = Line3(t.midpoint() + t.get_normal(), -1, t.midpoint());
				//Line3 line2 = Line3(t.midpoint(), -1, V3ZERO);
				//line.color = olc::RED;
				//line2.color = olc::CYAN;
				//line.mesh->Update(camera.position, ProjectionMatrix(p), view);
				//line.Draw(p);
				//line2.mesh->Update(camera.position, ProjectionMatrix(p), view);
				//line2.Draw(p);
			}
		}

		//debug
		//p->DrawStringDecal(olc::vf2d(0, p->ScreenHeight() - 10), "Mouse Pos: " + p->GetMousePos().str());
		p->DrawStringDecal(olc::vf2d(0, p->ScreenHeight() - 10), "Camera Pos: " + camera.position.str());
	}

	static void Cleanup() {
		//TODO(r, sushi, 11/18/2020) writing this is probably important
	}
};