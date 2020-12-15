#include "TimeSystem.h"
#include "../EntityAdmin.h"

#include "../components/TimeSingleton.h"

void TimeSystem::Update(float deltaTime, olc::PixelGameEngine* p) {
	TimeSingleton* time = admin->singletonTime;

	time->totalTime += deltaTime;
	++time->updateCount;

	time->physicsCounter += deltaTime;
	if(time->physicsCounter > time->physicsDeltaTime) {
		time->physicsCounter = 0; //TODO(pr,delle) separate physics ticks from render ticks
	}
}