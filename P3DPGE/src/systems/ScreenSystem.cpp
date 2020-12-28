#include "ScreenSystem.h"

#include "../components/Screen.h"

void ScreenSystem::Update() {
	Screen*		screen = admin->singletonScreen;
	olc::PixelGameEngine*	p = admin->p;

	//clear the screen to black
	p->Clear(olc::BLACK);

	if(p->ScreenWidth() == screen->width && p->ScreenHeight() == screen->height) { //TODO check for pixel size change
		if(screen->changedResolution){ screen->changedResolution = false; }
		return;
	}

	screen->changedResolution = true;
	screen->width = p->ScreenWidth();
	screen->height = p->ScreenHeight();
	screen->width = screen->width * screen->height;
	screen->dimensions = Vector2(screen->width, screen->height);
	SUCCESS(__FILENAME__, ": Changing resolution from ", screen->dimensionsV3, " to ", Vector3(screen->width, screen->height));
	screen->dimensionsV3 = Vector3(screen->width, screen->height);
}