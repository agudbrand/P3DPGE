#include "CameraSystem.h"
#include "../math/Math.h"

#include "../components/Camera.h"
#include "../components/Screen.h"

#include "../utils/Command.h"

void AddCameraCommands(EntityAdmin* admin) {
	admin->commands["reset_camera"] = new Command([](EntityAdmin* admin) {
		admin->currentCamera->position = Vector3::ZERO;
		admin->currentCamera->rotation = Vector3::ZERO;
	}, "reset_camera", "reset_camera");
}

void CameraSystem::Init() {
	AddCameraCommands(admin);
}

Matrix4 MakeViewMatrix(Camera* camera) {
	//camera->lookDir = Vector3::FORWARD * Matrix3::RotationMatrixY(camera->rotation.y);
	camera->lookDir = (Vector3::FORWARD * Matrix4::RotationMatrix(camera->rotation.xInvert()));
	//BUFFERLOG(1, camera->lookDir);
	return Math::PointAtMatrix(camera->position, camera->position+camera->lookDir).Inverse();
}

Matrix4 MakeProjectionMatrix(Camera* camera, Screen* screen) {
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
	Camera* camera = admin->currentCamera;
	Screen* screen = admin->singletonScreen;

	camera->viewMatrix = MakeViewMatrix(camera);
	camera->projectionMatrix = MakeProjectionMatrix(camera, screen);

	admin->p->DrawStringDecal(olc::vf2d(screen->width-300, screen->height - 30), "Camera Pos: " + camera->position.str2f());
	admin->p->DrawStringDecal(olc::vf2d(screen->width-300, screen->height - 40), "Camera Rot: " + camera->rotation.str2f());
}