#include "TimeSystem.h"

#include "../components/TimeSingleton.h"
#include "../utils/Command.h"

inline void AddTimeCommands(EntityAdmin* admin) {
	admin->commands["time_pause_engine"] = new Command([](EntityAdmin* admin) {
		admin->singletonTime->paused = !admin->singletonTime->paused;
		if(admin->singletonTime->paused) {
			admin->singletonTime->deltaTime = 0.f;
		}
	}, "time_pause_engine", "time_pause_engine");

	admin->commands["time_next_frame"] = new Command([](EntityAdmin* admin) {
		if(admin->singletonTime->paused) {
			admin->singletonTime->frame = true;
		}
	}, "time_next_frame", "time_next_frame");
}

void TimeSystem::Init() {
	AddTimeCommands(admin);
}

void TimeSystem::Update() {
	TimeSingleton* time = admin->singletonTime;

	if(!time->paused) {
		time->deltaTime = admin->p->GetElapsedTime();
		time->totalTime += time->deltaTime;
		++time->updateCount;

		time->physicsAccumulator += time->deltaTime;
	} else if(time->frame) {
		time->deltaTime = admin->p->GetElapsedTime();
		time->totalTime += time->deltaTime;
		++time->updateCount;

		time->physicsAccumulator += time->deltaTime;

		time->frame = false;
	} else {
		time->deltaTime = 0.f;
	}
}