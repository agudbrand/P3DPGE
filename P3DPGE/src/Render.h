#pragma once
#include "Entities.h"
#include "Mesh.h"
#include "internal/olcPixelGameEngine.h"

namespace Render {
	//NOTE sushi: do we want Render to be this bloated? Or organized it elsewhere?

	//TODO(r, sushi) separate objects into different layers like debug, main, etc. to be drawn in certain orders

	internal bool DEBUG_RENDER = 1;

#define DEBUGR DEBUG if(DEBUG_RENDER)

	static std::vector<Entity*> entities;

	//special entities that should not be interacted with like
	//normal entities. this is probably temporary
	static std::vector<Entity*> sentities;

	//this list can most likely be optimized by keeping triangles in it
	//that we know havent been deleted but do that later
	//implement pointers here at some point 
	static std::vector<Triangle> triangles;

	static Timer* timer;

	static mat<float, 4, 4> view;
	static Camera camera;
	static float yaw;
	static float pitch;

	Vector3 light_direction(0, 0, -1);

	//TODO(r, sushi) set up a way that debug can be applied to certain objects. this is important in the case of edge numbers
	//debug booleans go here
	bool WIRE_FRAME = false;
	bool DISP_EDGES = false;

	//booleans for pausing and advancing by frame
	//pausing is sort of archaic right now but should be fleshed out later
	//there are some in Physics as well
	static bool paused = false;
	static bool frame = false;

	//just shove all entities into here, then draw them
	static void AddEntity(Entity* e) { entities.push_back(e); }

	static Camera* GetCamera() { return &camera; }

	static void Init() {
		
		//test lines
		//Line2* l1 = new Line2(Vector3(100, 100, 0), 1);
		//Line3* l2 = new Line3(Vector3(10, 10, 4), 1, Vector3(0, 2, 1));
		//entities.push_back(l1);
		//entities.push_back(l2);

		//test triangle
		//DebugTriangle* test = new DebugTriangle(Triangle(Vector3(0, 0, 0), Vector3(2, 0, 1), Vector3(3, 3, 1)), 1);
		//temporary permanent Sprite because with the way we are drawing triangles 
		//currently theres no way to know what the Entity's sprite is and I have a couple
		//of ideas on how to solve this but later.
		//Box* b = new Box(V3NULL, -1, V3NULL);
		//b->sprite = new olc::Sprite("sprites/UV_Grid_Sm.jpg");
		//entities.push_back(b);

		//Box box(Vector3(1, 1, 1), -1, Vector3(0, 0, 0));
		//SUCCESS(Vector3(1, 1, 1), 1);
		//LOG("Log! ", 23.90);
		//ERROR("box \n ", "nevermind");

		timer = new Timer;

		int a = 1;


	}

	using namespace boost::qvm;

	//TODO(rc, sushi) change this to take in types and not individual values
	//in essence this algorithm scans down a triangle and fills each row it occupies
	//with the texture. this is necessary to account for us clipping triangles.
	static void TexturedTriangle(olc::PixelGameEngine* p,
		int x1, int y1, float u1, float v1, float w1,
		int x2, int y2, float u2, float v2, float w2,
		int x3, int y3, float u3, float v3, float w3, 
		olc::Sprite* tex) {

		//manually sort y coords using swap(didn't know about swap its kinda cool)
		if (y2 < y1) { std::swap(y1, y2); std::swap(x1, x2); std::swap(u1, u2); std::swap(v1, v2); std::swap(w1, w2); }
		if (y3 < y1) { std::swap(y1, y3); std::swap(x1, x3); std::swap(u1, u3); std::swap(v1, v3); std::swap(w1, w3); }
		if (y3 < y2) { std::swap(y2, y3); std::swap(x2, x3); std::swap(u2, u3); std::swap(v2, v3); std::swap(w2, w3); }

		int dy1 = y2 - y1;
		int dx1 = x2 - x1;
		float dv1 = v2 - v1;
		float du1 = u2 - u1;
		float dw1 = w2 - w1;

		int dy2 = y3 - y1;
		int dx2 = x3 - x1;
		float dv2 = v3 - v1;
		float du2 = u3 - u1;
		float dw2 = w3 - w1;

		float tex_u, tex_v, tex_w;

		float dax_step = 0, dbx_step = 0,
			  du1_step = 0, dv1_step = 0,
			  du2_step = 0, dv2_step = 0,
			  dw1_step = 0, dw2_step = 0;

		//idk why javid wrote these this way
		//they can probably be compressed but im not gonna do that until im done
		//following the video just in case.
		if (dy1) { dax_step = dx1 / (float)std::abs(dy1); }
		if (dy2) { dbx_step = dx2 / (float)std::abs(dy2); }

		if (dy1) { du1_step = du1 / (float)std::abs(dy1); }
		if (dy1) { dv1_step = dv1 / (float)std::abs(dy1); }
		if (dy1) { dw1_step = dw1 / (float)std::abs(dy1); }

		if (dy2) { du2_step = du1 / (float)std::abs(dy2); }
		if (dy2) { dv2_step = dv2 / (float)std::abs(dy2); }
		if (dy2) { dw2_step = dw2 / (float)std::abs(dy2); }

		//scanline fill triangles
		if (dy1) {
			//scan down y
			for (int scany = y1; scany < y2; scany++) {
				int ax = x1 + (float)(scany - y1) * dax_step;
				int bx = x1 + (float)(scany - y1) * dbx_step;

				float tex_su = u1 + (float)(scany - y1) * du1_step;
				float tex_sv = v1 + (float)(scany - y1) * dv1_step;
				float tex_sw = w1 + (float)(scany - y1) * dv1_step;

				float tex_eu = u1 + (float)(scany - y1) * du2_step;
				float tex_ev = v1 + (float)(scany - y1) * dv2_step;
				float tex_ew = w1 + (float)(scany - y1) * dw2_step;

				//ensure we are actually drawing from start to end
				if (ax > bx) { std::swap(ax, bx); std::swap(tex_su, tex_eu); std::swap(tex_sv, tex_ev); std::swap(tex_sw, tex_ew); }

				tex_u = tex_su; tex_v = tex_sv; tex_w = tex_sw;

				float tstep = 1.f / ((float)(bx - ax));
				float t = 0.f;

				//fill over x
				for (int scanx = ax; scanx < bx; scanx++) {
					tex_u = Math::lerpf(tex_su, tex_eu, t);
					tex_v = Math::lerpf(tex_sv, tex_ev, t);
					tex_w = Math::lerpf(tex_sw, tex_ew, t);
					p->Draw(scanx, scany, tex->Sample(tex_u / tex_w, tex_v / tex_w));

					t += tstep;
				}
			}

			//here the path along one line changes and must be accounted for
			//also means we have to do the entire loop again which means 
			//it will probably be abstracted out at some point
			dy1 = y3 - y2;
			dx1 = x3 - x2;
			dv1 = v3 - v2;
			du1 = u3 - u2;
			dw1 = w3 - w2;

			if (dy1) { dax_step = dx1 / (float)std::abs(dy1); }
			if (dy2) { dbx_step = dx2 / (float)std::abs(dy2); }

			du1_step = 0; dv1_step = 0;
			if (dy1) { du1_step = du1 / (float)std::abs(dy1); }
			if (dy1) { dv1_step = dv1 / (float)std::abs(dy1); }
			if (dy1) { dw1_step = dw1 / (float)std::abs(dy1); }

			for (int scany = y2; scany < y3; scany++) {
				int ax = x2 + (float)(scany - y2) * dax_step;
				int bx = x1 + (float)(scany - y1) * dbx_step;

				float tex_su = u2 + (float)(scany - y2) * du1_step;
				float tex_sv = v2 + (float)(scany - y2) * dv1_step;
				float tex_sw = w2 + (float)(scany - y2) * dw1_step;

				float tex_eu = u1 + (float)(scany - y1) * du2_step;
				float tex_ev = v1 + (float)(scany - y1) * dv2_step;
				float tex_ew = w1 + (float)(scany - y1) * dw2_step;

				//ensure we are actually drawing from start to end
				if (ax > bx) { std::swap(ax, bx); std::swap(tex_su, tex_eu); std::swap(tex_sv, tex_ev); std::swap(tex_sw, tex_ew); }

				tex_u = tex_su; tex_v = tex_sv; tex_w = tex_ew;

				float tstep = 1.f / ((float)(bx - ax));
				float t = 0.f;

				//fill over x
				for (int scanx = ax; scanx < bx; scanx++) {
					tex_u = Math::lerpf(tex_su, tex_eu, t);
					tex_v = Math::lerpf(tex_sv, tex_ev, t);
					tex_w = Math::lerpf(tex_sw, tex_ew, t);
					p->Draw(scanx, scany, tex->Sample(tex_u / tex_w, tex_v / tex_w));

					t += tstep;
				}
			}


		}



	}

	static int ClipTriangles(Vector3 plane_p, Vector3 plane_n, Triangle in_tri, Triangle& out_tri1, Triangle& out_tri2) {
		plane_n.normalize();

		//temp storage to classify points on either side of plane
		Vector3* inside_points[3];  int nInsidePointCount = 0;
		Vector3* outside_points[3]; int nOutsidePointCount = 0;
		Vector3* inside_tex[3]; int nInsideTexCount = 0;
		Vector3* outside_tex[3]; int nOutsideTexCount = 0;
		
		auto dist = [&](Vector3& p)
		{
			Vector3 n = p.normalized();
			return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dot(plane_p));
		};

		//signed distance of each point in triangle to plane
		float d0 = dist(in_tri.proj_points[0]);
		float d1 = dist(in_tri.proj_points[1]);
		float d2 = dist(in_tri.proj_points[2]);


		if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.proj_points[0]; inside_tex[nInsideTexCount++] = &in_tri.tex_points[0]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.proj_points[0]; outside_tex[nOutsideTexCount++] = &in_tri.tex_points[0]; }
		if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.proj_points[1]; inside_tex[nInsideTexCount++] = &in_tri.tex_points[1]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.proj_points[1];  outside_tex[nOutsideTexCount++] = &in_tri.tex_points[1]; }
		if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.proj_points[2]; inside_tex[nInsideTexCount++] = &in_tri.tex_points[2]; }
		else { outside_points[nOutsidePointCount++] = &in_tri.proj_points[2];  outside_tex[nOutsideTexCount++] = &in_tri.tex_points[2]; }

		//classify points and break input triangle into smaller trangles if
		//required. there are four possible outcomes

		//all points lie outside the plane
		if (nInsidePointCount == 0) { return 0; }
		//all points lie inside the plane so do nothing and allow triangle to pass
		if (nInsidePointCount == 3) { out_tri1 = in_tri; return 1; }
		if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
			out_tri1.color = in_tri.color;

			//the inside point is valid so we keep it
			out_tri1.proj_points[0] = *inside_points[0];
			out_tri1.tex_points[0] = *inside_tex[0];

			//but the two new points are not where the original triangle intersects with the plane
			float t;
			out_tri1.proj_points[1] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
			out_tri1.tex_points[1].x = t * (outside_tex[0]->x - inside_tex[0]->x) + inside_tex[0]->x;
			out_tri1.tex_points[1].y = t * (outside_tex[0]->y - inside_tex[0]->y) + inside_tex[0]->y;

			out_tri1.proj_points[2] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
			out_tri1.tex_points[2].x = t * (outside_tex[1]->x - inside_tex[0]->x) + inside_tex[0]->x;
			out_tri1.tex_points[2].y = t * (outside_tex[1]->y - inside_tex[0]->y) + inside_tex[0]->y;

			return 1; //return new triangle
		}
		if (nInsidePointCount == 2 && nOutsidePointCount == 1) {
			//triangle will be clipped and becomes a quad which is
			//cut into two more triagles.

			

			out_tri1.color = in_tri.color;
			out_tri2.color = in_tri.color;

			out_tri1.proj_points[0] = *inside_points[0];
			out_tri1.proj_points[1] = *inside_points[1];
			out_tri1.tex_points[0] = *inside_tex[0];
			out_tri1.tex_points[1] = *inside_tex[1];

			float t;
			out_tri1.proj_points[2] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
			out_tri1.tex_points[2].x = t * (outside_tex[0]->x - inside_tex[0]->x) + inside_tex[0]->x;
			out_tri1.tex_points[2].y = t * (outside_tex[0]->y - inside_tex[0]->y) + inside_tex[0]->y;


			out_tri2.proj_points[0] = *inside_points[1];
			out_tri2.proj_points[1] = out_tri1.proj_points[2];
			out_tri2.proj_points[2] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
			return 2;
		}

		

		

	}//ClipTriangles

	//TODO(r, sushi) this still needs abstracted
	static void Draw(olc::PixelGameEngine* p) {
		std::vector<Triangle> visibleTriangles;
		std::vector<Triangle> drawnTriangles;

		//cool light rotation effect 
		//light_direction = V3FORWARD * Math::GetRotateV3_Y(200 * Time::totalTime);
		light_direction = light_direction.normalized();

		//store triangles we want to draw for sorting and copy world points to projected points
		for (auto& t : triangles) {
			t.copy_points();
			float dp = light_direction.dot(t.get_normal());
			if (t.get_normal().dot(t.midpoint() - camera.position) < 0) {
				t.set_color(olc::Pixel(
					std::clamp(50 * dp,  0.f, 50.f),
					std::clamp(75 * dp,  0.f, 75.f),
					std::clamp(200 * dp, 0.f, 200.f)
				));
				visibleTriangles.push_back(t);
			}
		}

		//project triangles to screen and add them to the draw vector
		for (Triangle t : visibleTriangles) {
			for (Vector3& n : t.proj_points) {
				n.M1x4ToVector3(n.proj_mult(n.ConvertToM1x4(), view));

			}

			int clippedTriangles = 0;
			Triangle clipped[2];
			clippedTriangles = ClipTriangles(Vector3(0, 0, 0.01), Vector3(0, 0, 1), t, clipped[0], clipped[1]);

			for (int i = 0; i < clippedTriangles; i++) {
				float w;
				for (Vector3& n : clipped[i].proj_points) {
					n.ProjToScreen(Render::camera.ProjectionMatrix(p), p, w);
				}
				clipped[i].e = t.e;
				
				//projecting texture
				for (Vector3& v : clipped[i].tex_points) {
					v.x /= w; v.y /= w;
					v.z = 1.f / w;
				}
				drawnTriangles.push_back(clipped[i]);
			}
		}

		std::sort(drawnTriangles.begin(), drawnTriangles.end(), [](Triangle& t1, Triangle& t2) {
			float mp1 = Math::DistTwoPoints(t1.midpoint(), camera.position);
			float mp2 = Math::DistTwoPoints(t2.midpoint(), camera.position);
			return mp1 > mp2;
			});

		//TODO(o, sushi) optimize this for loop 
		for (const Triangle& t : drawnTriangles) {
			Triangle clipped[2];
			std::list<Triangle> listTriangles;

			listTriangles.push_back(t);
			int newTriangles = 1;

			for (int a = 0; a < 4; a++) {
				int trisToAdd = 0;
				while (newTriangles > 0) {
					Triangle test = listTriangles.front();
					listTriangles.pop_front();
					newTriangles--;

					switch (a) {
					case 0:	trisToAdd = ClipTriangles(Vector3(0, 0, 0), Vector3(0, 1, 0), test, clipped[0], clipped[1]); break;
					case 1:	trisToAdd = ClipTriangles(Vector3(0, (float)p->ScreenHeight() - 1, 0), Vector3(0, -1, 0), test, clipped[0], clipped[1]); break;
					case 2:	trisToAdd = ClipTriangles(Vector3(0, 0, 0), Vector3(1, 0, 0), test, clipped[0], clipped[1]); break;
					case 3: trisToAdd = ClipTriangles(Vector3((float)p->ScreenWidth() - 1, 0, 0), Vector3(-1, 0, 0), test, clipped[0], clipped[1]); break;
					}

					for (int w = 0; w < trisToAdd; w++) {
						clipped[w].e = t.e;
						listTriangles.push_back(clipped[w]);
					}
				}
				newTriangles = listTriangles.size();
			}

			for (Triangle& t : listTriangles) {

				//TexturedTriangle(p,
				//	t.proj_points[0].x, t.proj_points[0].y, t.tex_points[0].x, t.tex_points[0].y, t.tex_points[0].z,
				//	t.proj_points[1].x, t.proj_points[1].y, t.tex_points[1].x, t.tex_points[1].y, t.tex_points[1].z,
				//	t.proj_points[2].x, t.proj_points[2].y, t.tex_points[2].x, t.tex_points[2].y, t.tex_points[2].z,
				//	t.e->sprite);


				//This has been rendered (lol) useless by textures but
				//could be used for debugging or something later
				p->FillTriangle(
					t.proj_points[0].x, t.proj_points[0].y,
					t.proj_points[1].x, t.proj_points[1].y,
					t.proj_points[2].x, t.proj_points[2].y,
					t.get_color());
			}
		}

		//debug drawing
		DEBUGR{
			if (WIRE_FRAME) {
				for (auto& t : drawnTriangles) {
					p->DrawTriangle(
						t.proj_points[0].x, t.proj_points[0].y,
						t.proj_points[1].x, t.proj_points[1].y,
						t.proj_points[2].x, t.proj_points[2].y,
						olc::WHITE);
				}
			}

			if (DISP_EDGES) {
				for (Triangle& t : drawnTriangles) {
					t.display_edges(p);
				}
			}
		};

		
	}//Draw

	static void ConsoleHandler(olc::PixelGameEngine* p){

		ContainerManager<std::string> last = g_cBuffer_last;

		RUN_ONCE g_cBuffer_last.copy(g_cBuffer);

		std::vector<std::pair<std::string, int>> inv_indexes;
		ContainerManager<std::string> new_cBuffer;

		//debug please remove if i dont
		ContainerManager<std::string> now  = g_cBuffer;
		last = g_cBuffer_last;

		//THIS ALMOST WORKS, there's just some data syncing i still need to do
		//please don't use this yet except for what I already have placed
		
		//Print last buffer at index if it doesn't match new else print new
		//then print all additions to buffer
		//TODO(g, sushi) fix this so it works with additions 
		for (int i = 0; i < g_cBuffer_last.size(); i++) {
			if (g_cBuffer.container[i].first) {
				if (i < g_cBuffer_last.size()) {
					if (g_cBuffer.container[i].second != g_cBuffer_last.container[i].second) {
						//new_cBuffer.add_to(g_cBuffer_last[i]);
						//LOG(g_cBuffer_last[i].first.value(), " ", g_cBuffer_last[i].second);
						p->DrawString(Vector2(p->ScreenWidth() / 2, 0 + 11 * i), g_cBuffer_last[i].first.value());
						//inv_indexes.push_back(std::pair<std::string, int>(g_cBuffer[i].first.value(), g_cBuffer[i].second));
					}
					else {
						//new_cBuffer.add_to(g_cBuffer[i]);
						//LOG(g_cBuffer[i].first.value(), " ", g_cBuffer[i].second);
						p->DrawString(Vector2(p->ScreenWidth() / 2, 0 + 9 * i), g_cBuffer[i].first.value());
					}
				}
				else {
					//LOG(g_cBuffer[i].first.value(), " ", g_cBuffer[i].second);
					p->DrawString(Vector2(p->ScreenWidth() / 2, 0 + 9 * i), g_cBuffer[i].first.value());
				}
			}
		}

		g_cBuffer_last.copy(g_cBuffer);

		/*if index is less than last buffer's size then it is a previously defined
		//object in the buffer else it is new
		for (int i = 0; i < inv_indexes.size(); i++) {
			if (inv_indexes[i].second < g_cBuffer_last.size()) {
				if(inv_indexes)
			}
		}*/
		g_cBuffer.empty();
	}

	//draw all entities to screen
	static void Update(olc::PixelGameEngine* p) {
		//Debug::StartTimer();
		view = camera.MakeViewMatrix(yaw);
		
		DEBUG g_campos = camera.position;

		//get triangles from all entities
		for (auto& e : entities) {

			e->Update(Time::deltaTime);

			//SpecialDraw is used for determining if its just an object
			//drawn with triangles or if its special eg. a 2D object or Line3
			if (e->SpecialDraw()) {
				e->Draw(p, Render::camera.ProjectionMatrix(p), view);
			}
			else {
				DEBUG e->Draw(p, Render::camera.ProjectionMatrix(p), view); //for accessing entity debug drawing
				for (auto& t : e->mesh->triangles) { triangles.push_back(t); }
			}
		}

		//draw special entities
		for (auto& se : sentities) {
			if (se->SpecialDraw()) {
				se->Draw(p, Render::camera.ProjectionMatrix(p), view);
			}
			else {
				for (auto& t : se->mesh->triangles) { triangles.push_back(t); }
			}
		}

		Draw(p);

		triangles.clear();

		ConsoleHandler(p);

		//debug
		DEBUGR p->DrawStringDecal(olc::vf2d(p->ScreenWidth() - 300, p->ScreenHeight() - 20), "Mouse: " + Vector3(p->GetMousePos()).str2f());
		DEBUGR p->DrawStringDecal(olc::vf2d(p->ScreenWidth()-300, p->ScreenHeight() - 10), "Camera: " + Render::camera.position.str2f());

		//Debug::EndTimerAverage(p, 10, "", 10);
	}

	static void Cleanup() {
		//TODO(r, sushi, 11/18/2020) writing this is probably important
	}

	

	
};