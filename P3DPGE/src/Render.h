#pragma once
#include "Scene.h"
#include "internal/olcPixelGameEngine.h"

namespace Render {
	//NOTE sushi: do we want Render to be this bloated? Or organized it elsewhere?

	//TODO(r, sushi) separate objects into different layers like debug, main, etc. to be drawn in certain orders

	internal bool DEBUG_RENDER = 1;

#define DEBUGR DEBUG if(DEBUG_RENDER)

	//this list can most likely be optimized by keeping triangles in it
	//that we know havent been deleted but do that later
	//implement pointers here at some point 
	static std::vector<Triangle> triangles;

	//debug booleans go here
	bool WIRE_FRAME = false;
	bool DISP_EDGES = false;

	static Matrix4 viewMatrix;
	static Matrix4 projectionMatrix;

	static float* pDepthBuffer;

	static Camera* GetCamera() { return &Scene::camera; }

	static Timer* timer;

	static int x0 = 0;
	static int y0 = 0;
	static bool up = true;

	static bool ui_toggle = 1;

	static void Init() {
		timer = new Timer;
		Scene::ui_layer.push_back(new Menu(Vector2(80, 50), "BUFFERLOG", "buflog", std::vector<Button*>{}));
		TIMER_S;
	}

	//TODO(rc, sushi) change this to take in types and not individual values
	//in essence this algorithm scans down a triangle and fills each row it occupies
	//with the texture. this is necessary to account for us clipping triangles.
	void TexturedTriangle(olc::PixelGameEngine* p,
		int x1, int y1, float u1, float v1, float w1,
		int x2, int y2, float u2, float v2, float w2,
		int x3, int y3, float u3, float v3, float w3,
		olc::Sprite* tex){

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

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);
		if (dy1) dw1_step = dw1 / (float)abs(dy1);

		if (dy2) du2_step = du2 / (float)abs(dy2);
		if (dy2) dv2_step = dv2 / (float)abs(dy2);
		if (dy2) dw2_step = dw2 / (float)abs(dy2);

		if (dy1){
			for (int i = y1; i <= y2; i++){
				int ax = x1 + (float)(i - y1) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u1 + (float)(i - y1) * du1_step;
				float tex_sv = v1 + (float)(i - y1) * dv1_step;
				float tex_sw = w1 + (float)(i - y1) * dw1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;
				float tex_ew = w1 + (float)(i - y1) * dw2_step;

				if (ax > bx){
					std::swap(ax, bx);
					std::swap(tex_su, tex_eu);
					std::swap(tex_sv, tex_ev);
					std::swap(tex_sw, tex_ew);
				}

				tex_u = tex_su;
				tex_v = tex_sv;
				tex_w = tex_sw;

				float tstep = 1.0f / ((float)(bx - ax));
				float t = 0.0f;

				for (int j = ax; j < bx; j++){
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;

					if (tex_w > pDepthBuffer[i * screenWidth + j]) {
						p->Draw(j, i, tex->Sample(tex_u / tex_w, tex_v / tex_w));
						pDepthBuffer[i * screenWidth + j] = tex_w;
					}
					t += tstep;
				}

			}
		}

		dy1 = y3 - y2;
		dx1 = x3 - x2;
		dv1 = v3 - v2;
		du1 = u3 - u2;
		dw1 = w3 - w2;

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		du1_step = 0, dv1_step = 0;
		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);
		if (dy1) dw1_step = dw1 / (float)abs(dy1);

		if (dy1){
			for (int i = y2; i <= y3; i++){
				int ax = x2 + (float)(i - y2) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u2 + (float)(i - y2) * du1_step;
				float tex_sv = v2 + (float)(i - y2) * dv1_step;
				float tex_sw = w2 + (float)(i - y2) * dw1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;
				float tex_ew = w1 + (float)(i - y1) * dw2_step;

				if (ax > bx){
					std::swap(ax, bx);
					std::swap(tex_su, tex_eu);
					std::swap(tex_sv, tex_ev);
					std::swap(tex_sw, tex_ew);
				}

				tex_u = tex_su;
				tex_v = tex_sv;
				tex_w = tex_sw;

				float tstep = 1.0f / ((float)(bx - ax));
				float t = 0.0f;

				for (int j = ax; j < bx; j++){
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;

					if (tex_w > pDepthBuffer[i * screenWidth + j]) {
						p->Draw(j, i, tex->Sample(tex_u / tex_w, tex_v / tex_w));
						pDepthBuffer[i * screenWidth + j] = tex_w;
					}
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

		if (d0 >= 0) { inside_points[nInsidePointCount++]   = &in_tri.proj_points[0]; inside_tex[nInsideTexCount++]   = &in_tri.tex_points[0]; }
		else         { outside_points[nOutsidePointCount++] = &in_tri.proj_points[0]; outside_tex[nOutsideTexCount++] = &in_tri.tex_points[0]; }
		if (d1 >= 0) { inside_points[nInsidePointCount++]   = &in_tri.proj_points[1]; inside_tex[nInsideTexCount++]   = &in_tri.tex_points[1]; }
		else         { outside_points[nOutsidePointCount++] = &in_tri.proj_points[1]; outside_tex[nOutsideTexCount++] = &in_tri.tex_points[1]; }
		if (d2 >= 0) { inside_points[nInsidePointCount++]   = &in_tri.proj_points[2]; inside_tex[nInsideTexCount++]   = &in_tri.tex_points[2]; }
		else         { outside_points[nOutsidePointCount++] = &in_tri.proj_points[2]; outside_tex[nOutsideTexCount++] = &in_tri.tex_points[2]; }

		//classify points and break input triangle into smaller trangles if
		//required. there are four possible outcomes

		//all points lie outside the plane
		if (nInsidePointCount == 0) { return 0; }
		//all points lie inside the plane so do nothing and allow triangle to pass
		if (nInsidePointCount == 3) { out_tri1 = in_tri; return 1; }
		if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
			out_tri1.color = in_tri.color;
			out_tri1.e = in_tri.e;

			//the inside point is valid so we keep it
			out_tri1.proj_points[0] = *inside_points[0];
			out_tri1.tex_points[0] = *inside_tex[0];

			//but the two new points are not where the original triangle intersects with the plane
			float t;
			out_tri1.proj_points[1] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
			out_tri1.tex_points[1].x = t * (outside_tex[0]->x - inside_tex[0]->x) + inside_tex[0]->x;
			out_tri1.tex_points[1].y = t * (outside_tex[0]->y - inside_tex[0]->y) + inside_tex[0]->y;
			out_tri1.tex_points[1].z = t * (outside_tex[0]->z - inside_tex[0]->z) + inside_tex[0]->z;

			out_tri1.proj_points[2] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
			out_tri1.tex_points[2].x = t * (outside_tex[1]->x - inside_tex[0]->x) + inside_tex[0]->x;
			out_tri1.tex_points[2].y = t * (outside_tex[1]->y - inside_tex[0]->y) + inside_tex[0]->y;
			out_tri1.tex_points[2].z = t * (outside_tex[1]->z - inside_tex[0]->z) + inside_tex[0]->z;

			return 1; //return new triangle
		}
		if (nInsidePointCount == 2 && nOutsidePointCount == 1) {
			//triangle will be clipped and becomes a quad which is
			//cut into two more triagles.

			out_tri1.color = in_tri.color;
			out_tri2.color = in_tri.color;
			out_tri1.e = in_tri.e;
			out_tri2.e = in_tri.e;

			out_tri1.proj_points[0] = *inside_points[0];
			out_tri1.proj_points[1] = *inside_points[1];
			out_tri1.tex_points[0] = *inside_tex[0];
			out_tri1.tex_points[1] = *inside_tex[1];

			float t;
			out_tri1.proj_points[2] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
			out_tri1.tex_points[2].x = t * (outside_tex[0]->x - inside_tex[0]->x) + inside_tex[0]->x;
			out_tri1.tex_points[2].y = t * (outside_tex[0]->y - inside_tex[0]->y) + inside_tex[0]->y;
			out_tri1.tex_points[2].z = t * (outside_tex[0]->z - inside_tex[0]->z) + inside_tex[0]->z;


			out_tri2.proj_points[0] = *inside_points[1];
			out_tri2.tex_points[0] = *inside_tex[1];
			out_tri2.proj_points[1] = out_tri1.proj_points[2];
			out_tri2.tex_points[1] = out_tri1.tex_points[2];
			out_tri2.proj_points[2] = Math::VectorPlaneIntersect(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
			out_tri2.tex_points[2].x = t * (outside_tex[0]->x - inside_tex[1]->x) + inside_tex[1]->x;
			out_tri2.tex_points[2].y = t * (outside_tex[0]->y - inside_tex[1]->y) + inside_tex[1]->y;
			out_tri2.tex_points[2].z = t * (outside_tex[0]->z - inside_tex[1]->z) + inside_tex[1]->z;
			return 2;
		}

		

		

	}//ClipTriangles

	//TODO(r, sushi) this still needs abstracted
	static void Draw(olc::PixelGameEngine* p) {
		std::vector<Triangle> drawnTriangles;

		Vector3 light_direction(0, 0, -1);

		

		//store triangles we want to draw for sorting and copy world points to projected points
		for (auto& t : triangles) {
			
			t.copy_points();
			float light_ray1 = (Scene::light.position - t.points[0]).dot(t.get_normal());
			//float light_ray2 = (t.points[0] - Scene::light2.position).dot(t.get_normal());
			
			//Line3 ray = Line3(t.points[0], Scene::light.position);
			//ray.Draw(p, Scene::camera.ProjectionMatrix(p), viewMatrix);

			float dp = light_ray1;
			//float dp = light_direction.dot(t.get_normal());
			if (t.get_normal().dot(t.midpoint() - Scene::camera.position) < 0) {
				t.set_color(olc::Pixel(
					std::clamp(50 * dp,  0.f, 50.f),
					std::clamp(75 * dp,  0.f, 75.f),
					std::clamp(200 * dp, 0.f, 200.f)
				));
				for (Vector3& n : t.proj_points) {
					n = Math::WorldToCamera(n, viewMatrix).ToVector3();
				}

				int clippedTriangles = 0;
				Triangle clipped[2];
				clippedTriangles = ClipTriangles(Vector3(0, 0, 0.1f), Vector3(0, 0, 1), t, clipped[0], clipped[1]);
				
				
				for (int i = 0; i < clippedTriangles; i++) {
					//for (Vector3& n : clipped[i].proj_points) {
					//	n.ProjToScreen(Scene::camera.ProjectionMatrix(p), p, w);
					//}
					float w;
					for (int o = 0; o < 3; o++) {
						clipped[i].proj_points[o] = Math::CameraToScreen(clipped[i].proj_points[o], projectionMatrix, w);
						clipped[i].tex_points[o].x /= w;
						clipped[i].tex_points[o].y /= w;
						clipped[i].tex_points[o].z = 1 / w;
					}

					clipped[i].e = t.e;
					clipped[i].sprite = t.sprite;
					
					float ti = g_totalTime;
					for (int ox = 0; ox < 100; ox++) {
						for (int oy = 0; oy < 100; oy++) {
							ti += 0.1;

							float dist = (clipped[i].sprite_pixel_location(ox, oy) - Scene::light.position).mag();
							float dp = Scene::light.direction.dot(clipped[i].get_normal());
							//std::clamp(50 * dp, 0.f, 50.f),
							//std::clamp(75 * dp, 0.f, 75.f),
							//std::clamp(200 * dp, 0.f, 200.f)

							clipped[i].sprite->SetPixel(Vector2(ox, oy),
								olc::Pixel(
									floor(255 * 1 / dist),
									floor(255 * 1 / dist),
									floor(255 * 1 / dist)));
							

						}
					}

					
					drawnTriangles.push_back(clipped[i]);
				}
			}
		}

		//std::sort(drawnTriangles.begin(), drawnTriangles.end(), [](Triangle& t1, Triangle& t2) {
		//	float mp1 = Math::DistTwoPoints(t1.midpoint(), Scene::camera.position);
		//	float mp2 = Math::DistTwoPoints(t2.midpoint(), Scene::camera.position);
		//	return mp1 > mp2;
		//	});

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
					case 1:	trisToAdd = ClipTriangles(Vector3(0, (float)screenHeight - 1, 0), Vector3(0, -1, 0), test, clipped[0], clipped[1]); break;
					case 2:	trisToAdd = ClipTriangles(Vector3(0, 0, 0), Vector3(1, 0, 0), test, clipped[0], clipped[1]); break;
					case 3: trisToAdd = ClipTriangles(Vector3((float)screenWidth - 1, 0, 0), Vector3(-1, 0, 0), test, clipped[0], clipped[1]); break;
					}

					for (int wa = 0; wa < trisToAdd; wa++) {
						clipped[wa].e = t.e;
						clipped[wa].sprite = t.sprite;
						listTriangles.push_back(clipped[wa]);
					}
				}
				newTriangles = listTriangles.size();
			}

			for (Triangle& tr : listTriangles) {

				
				

				TexturedTriangle(p,
					tr.proj_points[0].x, tr.proj_points[0].y, tr.tex_points[0].x, tr.tex_points[0].y, tr.tex_points[0].z,
					tr.proj_points[1].x, tr.proj_points[1].y, tr.tex_points[1].x, tr.tex_points[1].y, tr.tex_points[1].z,
					tr.proj_points[2].x, tr.proj_points[2].y, tr.tex_points[2].x, tr.tex_points[2].y, tr.tex_points[2].z,
					tr.sprite);

				p->DrawCircle(Math::WorldToScreen2D(tr.sprite_pixel_location(x0, y0), Scene::camera.ProjectionMatrix(), viewMatrix), 10);


				//This has been rendered (lol) useless by textures but
				//could be used for debugging or something later
				//p->FillTriangle(
				//	t.proj_points[0].x, t.proj_points[0].y,
				//	t.proj_points[1].x, t.proj_points[1].y,
				//	t.proj_points[2].x, t.proj_points[2].y,
				//	t.get_color());
			}
		}

		p->DrawCircle(Math::WorldToScreen2D(Scene::light.position, Scene::camera.ProjectionMatrix(), viewMatrix), 10);


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
		int index = 0;
		//BUFFERLOG_UI->update_dyn_strings(g_cBuffer);
		//for (auto e : g_cBuffer.container) {
		//	if (e.has_value()) {
		//		p->DrawString(Vector2(100, 0 + index * 9), e.value());
		//		index++;
		//	}
		//}
	}

	

	//draw all entities to screen
	static void Update(olc::PixelGameEngine* p) {

		viewMatrix = Scene::camera.MakeViewMatrix(Scene::yaw);
		projectionMatrix = Scene::camera.ProjectionMatrix();

		for (int i = 0; i < screenWidth * screenHeight; i++) {
			pDepthBuffer[i] = 0.f;
		}

		DEBUG g_campos = Scene::camera.position;

		BUFFERLOG(0, Scene::camera.position.str2f());

		//get triangles from all entities
		for (Entity* e : Scene::entities) {
			e->Update(Time::deltaTime);

			//SpecialDraw is used for determining if its just an object
			//drawn with triangles or if its special eg. a 2D object or Line3
			if (e->SpecialDraw()) {
				e->Draw(p, projectionMatrix, viewMatrix);
			}
			else {
				DEBUG e->Draw(p, projectionMatrix, viewMatrix); //for accessing entity debug drawing
				for (auto& t : e->mesh->triangles) { triangles.push_back(t); }
			}
		}

		if (TIMER_GET > 0.01) {
			TIMER_E;
			if (up ? x0 >= 100 - y0 : x0 <= 0) {
				if (y0 >= 100) {
					y0 = 0;
					x0 = 0;
				}
				y0 += 5;
				up = !up;
			}
			else {
				if (up) { x0 += 5; }
				else { x0 -= 5; }
			}
			TIMER_S;
		}
		
		Draw(p);

		if (ui_toggle) {
			for (UI* u : Scene::ui_layer) {
				if (Menu* m = dynamic_cast<Menu*>(u)) {
					if (m->title == "BUFFERLOG") {
						m->update_dyn_strings(g_cBuffer);
					}
				}
				u->Draw(p);
			}
		}
		
		Scene::debug_layer.clear();
		triangles.clear();
		ConsoleHandler(p);

		//debug
		DEBUGR p->DrawStringDecal(olc::vf2d(screenWidth - 300, screenHeight - 20), "Mouse: " + Vector3(p->GetMousePos()).str2f());
		DEBUGR p->DrawStringDecal(olc::vf2d(screenWidth-300, screenHeight - 10), "Camera: " + Scene::camera.position.str2f());

		//Debug::EndTimerAverage(p, 10, "", 10);
	}

	static void Cleanup() {
		if(timer) delete timer;
	}

	

	
};