#include "RenderSceneSystem.h"
#include "../internal/olcPixelGameEngine.h"
#include "../math/Math.h"
#include "../geometry/Triangle.h"
#include "../EntityAdmin.h"
#include "../utils/Debug.h"

#include "../components/Scene.h"
#include "../components/Mesh.h"
#include "../components/Camera.h"
#include "../components/InputSingleton.h"
#include "../components/Keybinds.h"
#include "../components/Light.h"
#include "../components/ScreenSingleton.h"
#include "../components/TimeSingleton.h"

//TODO(or,delle) this took about 50% CPU time (in release), look into optimizing this
//TODO(rc, sushi) change this to take in types and not individual values
//in essence this algorithm scans down a triangle and fills each row it occupies
//with the texture. this is necessary to account for us clipping triangles.
void RenderSceneSystem::TexturedTriangle(Scene* scene, ScreenSingleton* screen, olc::PixelGameEngine* p,
										int x1, int y1, float u1, float v1, float w1,
										int x2, int y2, float u2, float v2, float w2,
										int x3, int y3, float u3, float v3, float w3,
										olc::Sprite* tex) {
	if (y2 < y1) { std::swap(y1, y2); std::swap(x1, x2); std::swap(u1, u2); std::swap(v1, v2); std::swap(w1, w2); }
	if (y3 < y1) { std::swap(y1, y3); std::swap(x1, x3); std::swap(u1, u3); std::swap(v1, v3); std::swap(w1, w3); }
	if (y3 < y2) { std::swap(y2, y3); std::swap(x2, x3); std::swap(u2, u3); std::swap(v2, v3); std::swap(w2, w3); }

	int   dy1 = y2 - y1;	int dx1 = x2 - x1;
	float dv1 = v2 - v1;	float du1 = u2 - u1;
	float dw1 = w2 - w1;

	int	  dy2 = y3 - y1;	int dx2 = x3 - x1;
	float dv2 = v3 - v1;	float du2 = u3 - u1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float	dax_step = 0, dbx_step = 0,
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
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				if (tex_w > scene->pixelDepthBuffer[i * screen->width + j]) {
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;

					p->Draw(j, i, tex->Sample(tex_u / tex_w, tex_v / tex_w));
					scene->pixelDepthBuffer[i * screen->width + j] = tex_w;
				}
				t += tstep;
			}

		}
	}

	dy1 = y3 - y2; dx1 = x3 - x2;
	dv1 = v3 - v2; du1 = u3 - u2;
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

				if (tex_w > scene->pixelDepthBuffer[i * screen->width + j]) {
					p->Draw(j, i, tex->Sample(tex_u / tex_w, tex_v / tex_w));
					scene->pixelDepthBuffer[i * screen->width + j] = tex_w;
				}
				t += tstep;
			}
		}
	}
} //TexturedTriangle


int RenderSceneSystem::ClipTriangles(const Vector3& plane_p, Vector3 plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2) {
	plane_n.normalize();

	//temp storage to classify points on either side of plane
	Vector3* inside_points[3];  int nInsidePointCount = 0;
	Vector3* outside_points[3]; int nOutsidePointCount = 0;
	Vector3* inside_tex[3];		int nInsideTexCount = 0;
	Vector3* outside_tex[3];	int nOutsideTexCount = 0;

	auto dist = [&](const Vector3& p)
	{
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
		out_tri1.e = 0; //set pointer to zero, so we can mark it for deletion

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
		out_tri1.e = 0; //set pointer to zero, so we can mark it for deletion
		out_tri2.e = 0; //set pointer to zero, so we can mark it for deletion

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
} //ClipTriangles


void RenderSceneSystem::Update(float deltaTime, olc::PixelGameEngine* p) {
	Scene* scene = admin->singletonScene;
	Camera* camera = admin->singletonCamera;
	InputSingleton* input = admin->singletonInput;
	Keybinds* binds = admin->singletonKeybinds;
	ScreenSingleton* screen = admin->singletonScreen;
	TimeSingleton* time = admin->singletonTime;

	//toggle wireframe
	if(input->KeyPressed(binds->debugRenderWireframe) && !(input->KeyHeld(olc::CTRL) || input->KeyHeld(olc::SHIFT))) {
		scene->WIRE_FRAME = scene->WIRE_FRAME ? false : true;
	}

	//toggle edge display
	if(input->KeyPressed(binds->debugRenderDisplayEdges) && !(input->KeyHeld(olc::CTRL) || input->KeyHeld(olc::SHIFT))) {
		scene->DISPLAY_EDGES = scene->DISPLAY_EDGES ? false : true;
	}

	//clear screen to black
	p->Clear(olc::BLACK);

	//collect all triangles
	scene->triangles.clear();
	float totalTriCount = 0;
	for(auto pair : admin->entities) {
		for(Component* comp : pair.second->components) {
			if(Mesh* mesh = dynamic_cast<Mesh*>(comp)) {
				for(Triangle& t : mesh->triangles) {
					scene->triangles.push_back(&t);
				}
			}
			/*if(SpriteRenderer* sr = dynamic_cast<SpriteRenderer*>(comp)) { //idea for 2d drawing
			
			}*/
		}
	}

	//std::vector<std::pair<bool, Triangle*>> zClippedTris;
	std::vector<Triangle> zClippedTris;
	scene->lights.push_back(&Light(Vector3(0, 0, 1), Vector3(0, 0, 0))); //TODO replace this with light components on entities
	scene->lights.push_back(&Light(Vector3(1, 0, 0), Vector3(0, 0, 0)));

	//filter out triangles that dont need drawing and clip triangles that do
	for(Triangle* t0 : scene->triangles) {
		Triangle t = *t0;
		t.copy_points(); //copy worldspace points to proj_points
		Vector3 triNormal = t.get_normal();
		float light_ray1 = (scene->lights[0]->position - t.points[0]).dot(triNormal);

		//if the angle between the middle of the triangle and the camera is greater less 90 degrees, it should show
		if(triNormal.dot(t.midpoint() - camera->position) < 0) { 
			//TODO(r,sushi) explain this to delle
			//set color at the beginning?
			t.set_color(olc::Pixel(std::clamp(50.f * light_ray1, 0.f, 50.f), 
									std::clamp(75.f * light_ray1, 0.f, 50.f),
									std::clamp(200.f * light_ray1, 0.f, 50.f)));

			//project points to view/camera space and clip them to the nearZ plane
			for(Vector3& pp : t.proj_points) {
				pp = Math::WorldToCamera(pp, camera->viewMatrix).ToVector3();
			}
			Triangle* clipped = new Triangle[2];
			int numClipped = ClipTriangles(Vector3(0, 0, camera->nearZ), Vector3::FORWARD, t, clipped[0], clipped[1]);

			//loop thru nearZ-clipped triangles to project them to screen and adjust texture points, 
			//then set the darkness of sprite's pixels based on light
			for(int i = 0; i < numClipped; ++i) {
				//project to screen
				for(int j = 0; j < 3; ++j) {
					float w;
					clipped[i].proj_points[j] = Math::CameraToScreen(clipped[i].proj_points[j], camera->projectionMatrix, screen->dimensions, w);
					clipped[i].tex_points[j].x /= w;
					clipped[i].tex_points[j].y /= w;
					clipped[i].tex_points[j].z = 1.f / w;
				}
				clipped[i].sprite = t.sprite;

				//darken less-lit pixels
				for(int x = 0; x < 25; ++x) {
					for(int y = 0; y < 25; ++y) {
						float dist = (clipped[i].sprite_pixel_location(x, y) - scene->lights[0]->position).mag();
						float dp = (scene->lights[0]->position - clipped[i].points[0]).dot(clipped[i].get_normal());

						clipped[i].sprite->SetPixel(Vector2(x, y), olc::Pixel(
							floor(std::clamp(255 * (1 /  (2 * dist)), 0.f, 255.f)),
							floor(std::clamp(255 * (1 /  (2 * dist)), 0.f, 255.f)),
							floor(std::clamp(255 * (1 /  (2 * dist)), 0.f, 255.f))));
					}
				}

				//if its a new triangle, mark it for deletion since its not owned by a mesh
				//if(clipped[i].e == 0) {
				//	zClippedTris.push_back(std::make_pair(true, &clipped[i])); //TODO test this
				//} else {
				//	zClippedTris.push_back(std::make_pair(false, &clipped[i]));
				//}
				zClippedTris.push_back(clipped[i]);
			}

		}

		//loop through nearZ-clipped triangles and clip them to the screen borders
		//then draw the triangles in the desired mode
		for(auto pair : zClippedTris) {
			//clip to screen borders
			Triangle* clipped = new Triangle[2];
			//std::list<std::pair<bool, Triangle*>> borderClippedTris;
			//borderClippedTris.push_back(std::make_pair(pair.first, pair.second));
			std::list<Triangle> borderClippedTris;
			borderClippedTris.push_back(pair);

			int newTris = 1;
			for(int i = 0; i < 4; ++i) {
				int trisToAdd = 0;
				while(newTris > 0) {
					//Triangle tri = *borderClippedTris.front().second;
					Triangle tri = borderClippedTris.front();
					borderClippedTris.pop_front(); //TODO(ro,delle) test if its better/works to use vector and pop_back
					newTris--;

					switch (i) {
						case 0:	trisToAdd = ClipTriangles(Vector3::ZERO,					Vector3::UP,	tri, clipped[0], clipped[1]); break;
						case 1:	trisToAdd = ClipTriangles(Vector3(0, screen->height-1, 0),	Vector3::DOWN,	tri, clipped[0], clipped[1]); break;
						case 2:	trisToAdd = ClipTriangles(Vector3::ZERO,					Vector3::RIGHT, tri, clipped[0], clipped[1]); break;
						case 3: trisToAdd = ClipTriangles(Vector3(screen->width-1, 0, 0),	Vector3::LEFT,	tri, clipped[0], clipped[1]); break;
					}

					for(int j = 0; j < trisToAdd; ++j) {
						clipped[j].sprite = t.sprite;

						//if its a new triangle, mark it for deletion since its not owned by a mesh
						//if(clipped[j].e == 0) {
						//	borderClippedTris.push_back(std::make_pair(true, &clipped[j])); //TODO test this
						//} else {
						//	borderClippedTris.push_back(std::make_pair(false, &clipped[j]));
						//}
						borderClippedTris.push_back(clipped[j]);
					}
				}
				newTris = borderClippedTris.size();
			}

			//draw in the desired mode
			for(auto pair2 : borderClippedTris) {
				//Triangle* tr = pair2.second;
				if(scene->WIRE_FRAME) {				//draw wireframed
					p->DrawTriangle(pair2.proj_points[0].x, pair2.proj_points[0].y,
									pair2.proj_points[1].x, pair2.proj_points[1].y,
									pair2.proj_points[2].x, pair2.proj_points[2].y,
									olc::WHITE);
				} else {							//draw textured
					TexturedTriangle(scene, screen, p,
						pair2.proj_points[0].x, pair2.proj_points[0].y, pair2.tex_points[0].x, pair2.tex_points[0].y, pair2.tex_points[0].z,
						pair2.proj_points[1].x, pair2.proj_points[1].y, pair2.tex_points[1].x, pair2.tex_points[1].y, pair2.tex_points[1].z,
						pair2.proj_points[2].x, pair2.proj_points[2].y, pair2.tex_points[2].x, pair2.tex_points[2].y, pair2.tex_points[2].z,
						pair2.sprite);
				}

				//display edges if enabled
				if(scene->DISPLAY_EDGES) {
					pair2.display_edges(p);
				}

				////delete created border-clipped triangles
				//if(pair2.first && pair2.second) {
				//	//delete pair2.second; //TODO test this
				//}
				++totalTriCount;
			}


			//delete created nearZ-clipped triangles
			//if(pair.first && pair.second) {
			//	//delete pair.second; //TODO test this
			//}
		}
	}

	p->DrawCircle(Math::WorldToScreen2D(scene->lights[0]->position, camera->projectionMatrix, camera->viewMatrix, screen->dimensions), 10);

	//cleanup after drawing
	scene->lights.clear();
	zClippedTris.clear();

	p->DrawStringDecal(olc::vf2d(screen->width-300, screen->height - 10), "Tri Count: " + std::to_string(totalTriCount));

} //Update
