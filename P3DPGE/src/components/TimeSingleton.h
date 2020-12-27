#pragma once
#include "Component.h"
#include <time.h>

struct TimeSingleton : public Component {
	float deltaTime;
	float totalTime;
	uint64 updateCount;

	float physicsTimeStep;
	float physicsDeltaTime;
	float physicsTotalTime;
	float physicsAccumulator;

	bool paused;
	bool frame;

	TimeSingleton() {
		deltaTime	= 0.f;
		totalTime	= 0.f;
		updateCount = 0.f;

		physicsTimeStep		= 300.f;
		physicsDeltaTime	= 1.f / physicsTimeStep;
		physicsTotalTime	= 0.f;
		physicsAccumulator	= 0.f;

		paused	= false;
		frame	= false;
	}

	TimeSingleton(float physicsTicksPerSecond) {
		deltaTime	= 0.f;
		totalTime	= 0.f;
		updateCount = 0.f;

		physicsTimeStep		= physicsTicksPerSecond;
		physicsDeltaTime	= 1.f / physicsTimeStep;
		physicsTotalTime	= 0.f;
		physicsAccumulator		= 0.f;

		paused	= false;
		frame	= false;
	}
};