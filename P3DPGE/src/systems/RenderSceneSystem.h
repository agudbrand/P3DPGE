#pragma once
#include "System.h"

/*struct Triangle;
struct Vector3;
struct Scene;
struct ScreenSingleton;
struct Camera;

namespace olc {
	struct Sprite;
}

struct RenderSceneSystem : public System {
	//function description goes here
	void TexturedTriangle(
		Scene* scene, ScreenSingleton* screen, olc::PixelGameEngine* p,
		int x1, int y1, float u1, float v1, float w1,
		int x2, int y2, float u2, float v2, float w2,
		int x3, int y3, float u3, float v3, float w3,
		olc::Sprite* tex);

	//function description goes here
	int ClipTriangles(const Vector3& plane_p, Vector3 plane_n, Triangle* in_tri, std::array<Triangle*, 2>& out_tris);

	int RenderTriangles(Scene* scene, Camera* camera, ScreenSingleton* screen, olc::PixelGameEngine* p);

	//function description goes here
	void Update(float deltaTime, olc::PixelGameEngine* p) override;
};*/

struct RenderSceneSystem : public System {
	void Init() override;
	void Update() override;
};