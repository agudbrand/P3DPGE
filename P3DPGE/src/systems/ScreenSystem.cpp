#include "ScreenSystem.h"
#include "../EntityAdmin.h"

#include "../components/ScreenSingleton.h"

void ScreenSystem::Update(float deltaTime, olc::PixelGameEngine* p) {
	ScreenSingleton* screen = admin->singletonScreen;

	if(p->ScreenWidth() == screen->width && p->ScreenHeight() == screen->height) { //TODO check for pixel size change
		return;
	}

	screen->width = p->ScreenWidth();
	screen->height = p->ScreenHeight();
	screen->width = screen->width * screen->height;
	screen->dimensions = Vector2(screen->width, screen->height);
	SUCCESS("[ScreenSystem.cpp] Changing screen dimensions from ", screen->dimensionsV3, " to ", Vector3(screen->width, screen->height));
	screen->dimensionsV3 = Vector3(screen->width, screen->height);
}