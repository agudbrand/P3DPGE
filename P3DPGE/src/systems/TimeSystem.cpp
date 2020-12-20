#include "TimeSystem.h"
#include "../EntityAdmin.h"

#include "../components/TimeSingleton.h"

void TimeSystem::Update() {
	TimeSingleton* time = admin->singletonTime;

	if(!time->paused) {
		time->deltaTime = admin->p->GetElapsedTime();
		time->totalTime += time->deltaTime;
		++time->updateCount;

		time->physicsCounter += time->deltaTime;
		if(time->physicsCounter > time->physicsDeltaTime) {
			time->physicsCounter = 0; //TODO(pr,delle) separate physics ticks from render ticks
		}
	} else if(time->frame) {
		time->deltaTime = admin->p->GetElapsedTime();
		time->totalTime += time->deltaTime;
		++time->updateCount;

		time->physicsCounter += time->deltaTime;
		if(time->physicsCounter > time->physicsDeltaTime) {
			time->physicsCounter = 0; //TODO(pr,delle) separate physics ticks from render ticks
		}
		time->frame = false;
	} else {
		time->deltaTime = 0.f;
	}
}