#include "CameraSystem.h"
#include "../EntityAdmin.h"
#include "../math/Math.h"
#include "../internal/olcPixelGameEngine.h"

#include "../components/Camera.h"
#include "../components/ScreenSingleton.h"

Matrix4 MakeViewMatrix(Camera* camera) {
	//camera->lookDir = Vector3::FORWARD * Matrix3::RotationMatrixY(camera->rotation.y);
	camera->lookDir = (Vector3::FORWARD * Matrix4::RotationMatrix(camera->rotation.xInvert()));
	//BUFFERLOG(1, camera->lookDir);
	return Math::PointAtMatrix(camera->position, camera->position+camera->lookDir).Inverse();
}

Matrix4 MakeProjectionMatrix(Camera* camera, ScreenSingleton* screen) {
	float renderDistance = camera->farZ - camera->nearZ;
	float aspectRatio = screen->height / screen->width;
	float fovRad = 1.f / tanf(camera->fieldOfView * .5f * TO_RADIANS);

	return Matrix4( //NOTE setting (1,1) to negative flips the y-axis, but it might cause problems somewhere
		aspectRatio * fovRad,	0,		0,												0,
		0,						-fovRad, 0,												0,
		0,						0,		camera->farZ / renderDistance,					1,
		0,						0,		(-camera->farZ*camera->nearZ) / renderDistance,	0);
}

void CameraSystem::Update() {
	Camera* camera = admin->tempCamera;
	ScreenSingleton* screen = admin->singletonScreen;

	camera->viewMatrix = MakeViewMatrix(camera);
	camera->projectionMatrix = MakeProjectionMatrix(camera, screen);

	//DEBUGR p->DrawStringDecal(olc::vf2d(screenWidth-300, screenHeight - 10), "Camera: " + Scene::camera.position.str2f());
	admin->p->DrawStringDecal(olc::vf2d(screen->width-300, screen->height - 30), "Camera Pos: " + camera->position.str2f());
	admin->p->DrawStringDecal(olc::vf2d(screen->width-300, screen->height - 40), "Camera Rot: " + camera->rotation.str2f());
}