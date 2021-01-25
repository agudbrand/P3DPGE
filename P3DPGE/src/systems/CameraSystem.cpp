#include "CameraSystem.h"
#include "../math/Math.h"

#include "../components/Camera.h"
#include "../components/Screen.h"
#include "../components/Scene.h"

#include "../utils/Command.h"

void AddCameraCommands(EntityAdmin* admin) {
	admin->commands["reset_camera"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentCamera->position = Vector3::ZERO;
		admin->currentCamera->rotation = Vector3::ZERO;
		return "";
	}, "reset_camera", "reset_camera");
}

void CameraSystem::Init() {
	AddCameraCommands(admin);
}

Matrix4 MakeViewMatrix(Camera* camera) {
	//camera->lookDir = Vector3::FORWARD * Matrix3::RotationMatrixY(camera->rotation.y);
	
	camera->lookDir = Math::SphericalToRectangularCoords(camera->target);
	return Math::LookAtMatrix(camera->position, camera->lookDir + camera->position).Inverse();
}

Matrix4 MakeProjectionMatrix(Camera* camera, Screen* screen) {
	float renderDistance = camera->farZ - camera->nearZ;
	float aspectRatio = screen->height / screen->width;
	float fovRad = 1.f / tanf(camera->fieldOfView * .5f * TO_RADIANS);

	return Matrix4( //NOTE setting (1,1) to negative flips the y-axis
		aspectRatio * fovRad,	0,			0,												0,
		0,						-fovRad,	0,												0,
		0,						0,			camera->farZ / renderDistance,					1,
		0,						0,			(-camera->farZ*camera->nearZ) / renderDistance,	0);
}

Matrix4 MakeOrthoProjectionMatrix(Scene* s, Camera* c, Screen* sc) {
	std::pair<Vector3, Vector3> bbox = s->SceneBoundingBox();

	//convert bounding box to camera space
	Vector3 maxcam = Math::WorldToCamera(bbox.first,  c->viewMatrix).ToVector3();
	Vector3 mincam = Math::WorldToCamera(bbox.second, c->viewMatrix).ToVector3(); 
	
	//make screen box from camera space bounding box
	float maxx = std::max(fabs(mincam.x), fabs(maxcam.x));
	float maxy = std::max(fabs(mincam.y), fabs(maxcam.y));
	float max = std::max(maxx, maxy);

	float aspectRatio = sc->height / sc->width;
	float r = max * aspectRatio, t = max;
	float l = -r, b = -t;

	//TODO(r, sushi) implement ortho zooming by adjusting either the scale of objects or adjusting the bounding box
	//r += 10; l -= 10;
	//t += 10; b -= 10;

	return Matrix4(
		2 / (r - l),			0,						0,												0,
		0,						2 / (t - b),			0,												0,
		0,						0,						-2 / (c->farZ - c->nearZ),						0,
		-(r + l) / (r - l),		-(t + b) / (t - b),		-(c->farZ + c->nearZ) / (c->farZ - c->nearZ),	1);

}

void CameraSystem::Update() {
	Camera* camera = admin->currentCamera;
	Screen* screen = admin->screen;
	Scene*  scene  = admin->currentScene;

	camera->viewMatrix = MakeViewMatrix(camera);

	//TODO(r, sushi) figure out how to make the camera screen work with ortho nicely
	//TODO(r, sushi) implement ortho triangle clipping
	if (USE_ORTHO) camera->projectionMatrix = MakeOrthoProjectionMatrix(scene, camera, screen);
	else		   camera->projectionMatrix = MakeProjectionMatrix(camera, screen);

	admin->p->DrawStringDecal(olc::vf2d(screen->width-300, screen->height - 30), "Camera Pos: " + camera->position.str2f());
	admin->p->DrawStringDecal(olc::vf2d(screen->width-300, screen->height - 40), "Camera Rot: " + camera->target.str2f());
}