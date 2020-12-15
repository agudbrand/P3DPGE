#pragma once
#include "System.h"

struct Camera;
struct Matrix4;
struct ScreenSingleton;

namespace olc {
	template<typename V>
	struct v2d_generic;
}
typedef olc::v2d_generic<float> Vector2;

struct CameraSystem : public System {
	Matrix4 MakeViewMatrix(Camera* camera);
	Matrix4 MakeProjectionMatrix(Camera* camera, ScreenSingleton* screen);

	void Update(float deltaTime, olc::PixelGameEngine* p) override;
};